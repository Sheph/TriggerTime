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

#include "af/RUBEBody.h"
#include "Logger.h"
#include <boost/make_shared.hpp>
#include <json/json.h>
#include <log4cplus/ndc.h>

namespace af
{
    RUBEBody::RUBEBody(const std::string& name, const b2BodyDef& bodyDef)
    : RUBEBase(name),
      bodyDef_(bodyDef)
    {
    }

    RUBEBody::~RUBEBody()
    {
    }

    RUBEBodyPtr RUBEBody::fromJsonValue(const Json::Value& jsonValue)
    {
        b2BodyDef bodyDef;

        bodyDef.type = json2enum<b2BodyType>(jsonValue["type"], b2_staticBody);
        bodyDef.position = json2vec2(jsonValue["position"]);
        bodyDef.angle = json2float(jsonValue["angle"]);
        bodyDef.linearVelocity = json2vec2(jsonValue["linearVelocity"]);
        bodyDef.angularVelocity = json2float(jsonValue["angularVelocity"]);
        bodyDef.linearDamping = json2float(jsonValue["linearDamping"]);
        bodyDef.angularDamping = json2float(jsonValue["angularDamping"]);
        bodyDef.gravityScale = json2float(jsonValue["gravityScale"], -1, 1.0f);
        bodyDef.allowSleep = jsonValue.get("allowSleep", true).asBool();
        bodyDef.awake = jsonValue.get("awake", false).asBool();
        bodyDef.fixedRotation = jsonValue.get("fixedRotation", false).asBool();
        bodyDef.bullet = jsonValue.get("bullet", false).asBool();
        bodyDef.active = jsonValue.get("active", true).asBool();

        std::string name = jsonValue.get("name", "").asString();

        RUBEBodyPtr body = boost::make_shared<RUBEBody>(name, bodyDef);

        body->setPath(jsonValue.get("path", "").asString());
        body->setProps(jsonValue);

        int i = 0;
        Json::Value tmp = jsonValue["fixture"][i];
        while (!tmp.isNull()) {
            std::ostringstream os;
            os << "fixture " << i++;
            log4cplus::NDCContextCreator ndc(os.str());

            RUBEFixturePtr fixture = RUBEFixture::fromJsonValue(tmp);
            if (fixture) {
                body->addFixture(fixture);
            }

            tmp = jsonValue["fixture"][i];
        }

        return body;
    }

    void RUBEBody::addFixture(const RUBEFixturePtr& fixture)
    {
        fixtures_.push_back(fixture);
    }

    b2AABB RUBEBody::computeAABB(const b2Transform &t) const
    {
        b2AABB aabb;

        aabb.lowerBound = aabb.upperBound = b2Mul(t, b2Vec2_zero);

        bool first = true;

        for (int i = 0; i < numFixtures(); ++i) {
            b2AABB tmp;

            if (fixture(i)->shape().GetType() == b2Shape::e_chain) {
                b2ChainShape& ch = static_cast<b2ChainShape&>(fixture(i)->shape());
                for (int j = 0; j < ch.GetChildCount(); ++j) {
                    ch.ComputeAABB(&tmp, t, j);

                    if (first) {
                        first = false;
                        aabb = tmp;
                    } else {
                        aabb.Combine(tmp);
                    }
                }
            } else {
                fixture(i)->shape().ComputeAABB(&tmp, t, 0);

                if (first) {
                    first = false;
                    aabb = tmp;
                } else {
                    aabb.Combine(tmp);
                }
            }
        }

        return aabb;
    }

    void RUBEBody::addImage(const RUBEImagePtr& image)
    {
        images_.push_back(image);
    }

    RUBEImagePtr RUBEBody::image(const std::string& imageName) const
    {
        for (int i = 0; i < numImages(); ++i) {
            if (images_[i]->name() == imageName) {
                return images_[i];
            }
        }

        return RUBEImagePtr();
    }
}
