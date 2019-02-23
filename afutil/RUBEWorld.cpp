/*
 * Copyright (c) 2014, Stanislav Vorobiov
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "af/RUBEWorld.h"
#include "af/Utils.h"
#include "Logger.h"
#include <boost/make_shared.hpp>
#include <json/json.h>
#include <log4cplus/ndc.h>

namespace af
{
    RUBEWorld::RUBEWorld(const std::string& name)
    : RUBEBase(name)
    {
    }

    RUBEWorld::~RUBEWorld()
    {
    }

    RUBEWorldPtr RUBEWorld::fromStream(const std::string& fileName, std::istream& is)
    {
        std::string json;

        {
            log4cplus::NDCContextCreator ndc(fileName);

            if (!is) {
                LOG4CPLUS_ERROR(afutil::logger(), "Cannot open file");

                return RUBEWorldPtr();
            }

            if (!readStream(is, json)) {
                LOG4CPLUS_ERROR(afutil::logger(), "Error reading file");

                return RUBEWorldPtr();
            }
        }

        return fromString(fileName, json);
    }

    RUBEWorldPtr RUBEWorld::fromString(const std::string& fileName, const std::string& json)
    {
        Json::Value jsonValue;
        Json::Reader reader;

        if (!reader.parse(json, jsonValue)) {
            log4cplus::NDCContextCreator ndc(fileName);

            LOG4CPLUS_ERROR(afutil::logger(),
                            "Failed to parse JSON: " << reader.getFormattedErrorMessages());
            return RUBEWorldPtr();
        }

        return fromJsonValue(fileName, jsonValue);
    }

    RUBEWorldPtr RUBEWorld::fromJsonValue(const std::string& fileName, const Json::Value& jsonValue)
    {
        log4cplus::NDCContextCreator ndc(fileName);

        LOG4CPLUS_DEBUG(afutil::logger(), "Processing...");

        RUBEWorldPtr world = boost::make_shared<RUBEWorld>(fileName);

        world->setProps(jsonValue);

        int i = 0;
        Json::Value tmp = jsonValue["object"][i];
        while (!tmp.isNull()) {
            std::ostringstream os;
            os << "object " << i++;
            log4cplus::NDCContextCreator ndc(os.str());

            RUBEObjectPtr obj = RUBEObject::fromJsonValue(tmp);
            if (obj) {
                world->addObject(obj);
            }

            tmp = jsonValue["object"][i];
        }

        i = 0;
        tmp = jsonValue["body"][i];
        while (!tmp.isNull()) {
            std::ostringstream os;
            os << "body " << i++;
            log4cplus::NDCContextCreator ndc(os.str());

            RUBEBodyPtr body = RUBEBody::fromJsonValue(tmp);
            if (body) {
                world->addBody(body);
                if (!body->path().empty()) {
                    RUBEObjectPtr obj = world->object(body->path());
                    if (obj) {
                        obj->addBody(i - 1);
                    } else {
                        LOG4CPLUS_ERROR(afutil::logger(), "Invalid path - " << body->path());
                    }
                }
            }

            tmp = jsonValue["body"][i];
        }

        int totalJoints = jsonValue["joint"].size();

        i = 0;
        tmp = jsonValue["joint"][i];
        while (!tmp.isNull()) {
            std::ostringstream os;
            os << "joint " << i++;
            log4cplus::NDCContextCreator ndc(os.str());

            RUBEJointPtr joint = RUBEJoint::fromJsonValue(tmp);
            if (joint) {
                if (!world->haveBody(joint->bodyAIndex()) ||
                    !world->haveBody(joint->bodyBIndex())) {
                    LOG4CPLUS_ERROR(afutil::logger(),
                                    "Invalid body indices - "
                                    << joint->bodyAIndex() << ", "
                                    << joint->bodyBIndex());
                } else {
                    RUBEGearJointPtr gearJoint =
                        boost::dynamic_pointer_cast<RUBEGearJoint>(joint);

                    if (gearJoint &&
                        ((gearJoint->joint1Index() < 0) ||
                         (gearJoint->joint1Index() >= totalJoints))) {
                        LOG4CPLUS_ERROR(afutil::logger(),
                                        "Invalid joint1 index - "
                                        << gearJoint->joint1Index());
                    } else if (gearJoint &&
                               ((gearJoint->joint2Index() < 0) ||
                                (gearJoint->joint2Index() >= totalJoints))) {
                        LOG4CPLUS_ERROR(afutil::logger(),
                                        "Invalid joint2 index - "
                                        << gearJoint->joint2Index());
                    } else {
                        world->addJoint(joint);
                        if (!joint->path().empty()) {
                            RUBEObjectPtr obj = world->object(joint->path());
                            if (obj) {
                                obj->addJoint(i - 1);
                            } else {
                                LOG4CPLUS_ERROR(afutil::logger(), "Invalid path - " << joint->path());
                            }
                        }
                    }
                }
            }

            tmp = jsonValue["joint"][i];
        }

        i = 0;
        tmp = jsonValue["image"][i];
        while (!tmp.isNull()) {
            std::ostringstream os;
            os << "image " << i++;
            log4cplus::NDCContextCreator ndc(os.str());

            RUBEImagePtr image = RUBEImage::fromJsonValue(tmp);
            if (image) {
                if (image->bodyIndex() < 0) {
                    world->addImage(image);
                } else if (world->haveBody(image->bodyIndex())) {
                    world->addImage(image);
                    world->body(image->bodyIndex())->addImage(image);
                } else {
                    LOG4CPLUS_ERROR(afutil::logger(),
                                    "Invalid body index - " << image->bodyIndex());
                }
            }

            tmp = jsonValue["image"][i];
        }

        return world;
    }

    void RUBEWorld::addBody(const RUBEBodyPtr& body)
    {
        bodies_.push_back(body);
    }

    void RUBEWorld::addJoint(const RUBEJointPtr& joint)
    {
        joints_.push_back(joint);
    }

    void RUBEWorld::addImage(const RUBEImagePtr& image)
    {
        images_.push_back(image);
    }

    void RUBEWorld::addObject(const RUBEObjectPtr& obj)
    {
        objects_.push_back(obj);
    }

    RUBEBodyPtr RUBEWorld::body(const std::string& bodyName) const
    {
        for (int i = 0; i < numBodies(); ++i) {
            if (bodies_[i]->name() == bodyName) {
                return bodies_[i];
            }
        }

        return RUBEBodyPtr();
    }

    RUBEJointPtr RUBEWorld::joint(const std::string& jointName) const
    {
        for (int i = 0; i < numJoints(); ++i) {
            if (joints_[i]->name() == jointName) {
                return joints_[i];
            }
        }

        return RUBEJointPtr();
    }

    RUBEImagePtr RUBEWorld::image(const std::string& imageName) const
    {
        for (int i = 0; i < numImages(); ++i) {
            if (images_[i]->name() == imageName) {
                return images_[i];
            }
        }

        return RUBEImagePtr();
    }

    RUBEObjectPtr RUBEWorld::object(const std::string& objName) const
    {
        for (int i = 0; i < numObjects(); ++i) {
            if (objects_[i]->name() == objName) {
                return objects_[i];
            }
        }

        return RUBEObjectPtr();
    }
}
