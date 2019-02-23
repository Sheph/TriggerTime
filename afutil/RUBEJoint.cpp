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

#include "af/RUBEJoint.h"
#include "Logger.h"
#include <boost/make_shared.hpp>
#include <json/json.h>

#define AF_RUBE_JOINT_CLASS_IMPL(className) \
    RUBE##className::RUBE##className(const std::string& name) \
    : RUBEJoint(name) \
    { \
    } \
    RUBE##className::~RUBE##className() \
    { \
    }

namespace af
{
    RUBEJoint::RUBEJoint(const std::string& name)
    : RUBEBase(name)
    {
    }

    RUBEJoint::~RUBEJoint()
    {
    }

    RUBEJointPtr RUBEJoint::fromJsonValue(const Json::Value& jsonValue)
    {
        RUBEJointPtr joint;

        std::string name = jsonValue.get("name", "").asString();
        std::string type = jsonValue["type"].asString();

        if (type == "revolute") {
            RUBERevoluteJointPtr revoluteJoint = boost::make_shared<RUBERevoluteJoint>(name);
            joint = revoluteJoint;

            revoluteJoint->jointDef().localAnchorA = json2vec2(jsonValue["anchorA"]);
            revoluteJoint->jointDef().localAnchorB = json2vec2(jsonValue["anchorB"]);
            revoluteJoint->jointDef().referenceAngle = json2float(jsonValue["refAngle"]);
            revoluteJoint->jointDef().enableLimit = jsonValue["enableLimit"].asBool();
            revoluteJoint->jointDef().lowerAngle = json2float(jsonValue["lowerLimit"]);
            revoluteJoint->jointDef().upperAngle = json2float(jsonValue["upperLimit"]);
            revoluteJoint->jointDef().enableMotor = jsonValue["enableMotor"].asBool();
            revoluteJoint->jointDef().motorSpeed = json2float(jsonValue["motorSpeed"]);
            revoluteJoint->jointDef().maxMotorTorque = json2float(jsonValue["maxMotorTorque"]);
        } else if (type == "prismatic") {
            RUBEPrismaticJointPtr prismaticJoint = boost::make_shared<RUBEPrismaticJoint>(name);
            joint = prismaticJoint;

            prismaticJoint->jointDef().localAnchorA = json2vec2(jsonValue["anchorA"]);
            prismaticJoint->jointDef().localAnchorB = json2vec2(jsonValue["anchorB"]);
            if (!jsonValue["localAxisA"].isNull()) {
                prismaticJoint->jointDef().localAxisA = json2vec2(jsonValue["localAxisA"]);
            } else {
                prismaticJoint->jointDef().localAxisA = json2vec2(jsonValue["localAxis1"]);
            }
            prismaticJoint->jointDef().referenceAngle = json2float(jsonValue["refAngle"]);
            prismaticJoint->jointDef().enableLimit = jsonValue["enableLimit"].asBool();
            prismaticJoint->jointDef().lowerTranslation = json2float(jsonValue["lowerLimit"]);
            prismaticJoint->jointDef().upperTranslation = json2float(jsonValue["upperLimit"]);
            prismaticJoint->jointDef().enableMotor = jsonValue["enableMotor"].asBool();
            prismaticJoint->jointDef().motorSpeed = json2float(jsonValue["motorSpeed"]);
            prismaticJoint->jointDef().maxMotorForce = json2float(jsonValue["maxMotorForce"]);
        } else if (type == "distance") {
            RUBEDistanceJointPtr distanceJoint = boost::make_shared<RUBEDistanceJoint>(name);
            joint = distanceJoint;

            distanceJoint->jointDef().localAnchorA = json2vec2(jsonValue["anchorA"]);
            distanceJoint->jointDef().localAnchorB = json2vec2(jsonValue["anchorB"]);
            distanceJoint->jointDef().length = json2float(jsonValue["length"]);
            distanceJoint->jointDef().frequencyHz = json2float(jsonValue["frequency"]);
            distanceJoint->jointDef().dampingRatio = json2float(jsonValue["dampingRatio"]);
        } else if (type == "pulley") {
            RUBEPulleyJointPtr pulleyJoint = boost::make_shared<RUBEPulleyJoint>(name);
            joint = pulleyJoint;

            pulleyJoint->jointDef().groundAnchorA = json2vec2(jsonValue["groundAnchorA"]);
            pulleyJoint->jointDef().groundAnchorB = json2vec2(jsonValue["groundAnchorB"]);
            pulleyJoint->jointDef().localAnchorA = json2vec2(jsonValue["anchorA"]);
            pulleyJoint->jointDef().localAnchorB = json2vec2(jsonValue["anchorB"]);
            pulleyJoint->jointDef().lengthA = json2float(jsonValue["lengthA"]);
            pulleyJoint->jointDef().lengthB = json2float(jsonValue["lengthB"]);
            pulleyJoint->jointDef().ratio = json2float(jsonValue["ratio"]);
        } else if (type == "wheel") {
            RUBEWheelJointPtr wheelJoint = boost::make_shared<RUBEWheelJoint>(name);
            joint = wheelJoint;

            wheelJoint->jointDef().localAnchorA = json2vec2(jsonValue["anchorA"]);
            wheelJoint->jointDef().localAnchorB = json2vec2(jsonValue["anchorB"]);
            wheelJoint->jointDef().localAxisA = json2vec2(jsonValue["localAxisA"]);
            wheelJoint->jointDef().enableMotor = jsonValue["enableMotor"].asBool();
            wheelJoint->jointDef().motorSpeed = json2float(jsonValue["motorSpeed"]);
            wheelJoint->jointDef().maxMotorTorque = json2float(jsonValue["maxMotorTorque"]);
            wheelJoint->jointDef().frequencyHz = json2float(jsonValue["springFrequency"]);
            wheelJoint->jointDef().dampingRatio = json2float(jsonValue["springDampingRatio"]);
        } else if (type == "motor") {
            RUBEMotorJointPtr motorJoint = boost::make_shared<RUBEMotorJoint>(name);
            joint = motorJoint;

            motorJoint->jointDef().linearOffset = json2vec2(jsonValue["anchorA"]);
            motorJoint->jointDef().angularOffset = json2float(jsonValue["refAngle"]);
            motorJoint->jointDef().maxForce = json2float(jsonValue["maxForce"]);
            motorJoint->jointDef().maxTorque = json2float(jsonValue["maxTorque"]);
            motorJoint->jointDef().correctionFactor = json2float(jsonValue["correctionFactor"]);
        } else if (type == "weld") {
            RUBEWeldJointPtr weldJoint = boost::make_shared<RUBEWeldJoint>(name);
            joint = weldJoint;

            weldJoint->jointDef().localAnchorA = json2vec2(jsonValue["anchorA"]);
            weldJoint->jointDef().localAnchorB = json2vec2(jsonValue["anchorB"]);
            weldJoint->jointDef().referenceAngle = json2float(jsonValue["refAngle"]);
            weldJoint->jointDef().frequencyHz = json2float(jsonValue["frequency"]);
            weldJoint->jointDef().dampingRatio = json2float(jsonValue["dampingRatio"]);
        } else if (type == "friction") {
            RUBEFrictionJointPtr frictionJoint = boost::make_shared<RUBEFrictionJoint>(name);
            joint = frictionJoint;

            frictionJoint->jointDef().localAnchorA = json2vec2(jsonValue["anchorA"]);
            frictionJoint->jointDef().localAnchorB = json2vec2(jsonValue["anchorB"]);
            frictionJoint->jointDef().maxForce = json2float(jsonValue["maxForce"]);
            frictionJoint->jointDef().maxTorque = json2float(jsonValue["maxTorque"]);
        } else if (type == "rope") {
            RUBERopeJointPtr ropeJoint = boost::make_shared<RUBERopeJoint>(name);
            joint = ropeJoint;

            ropeJoint->jointDef().localAnchorA = json2vec2(jsonValue["anchorA"]);
            ropeJoint->jointDef().localAnchorB = json2vec2(jsonValue["anchorB"]);
            ropeJoint->jointDef().maxLength = json2float(jsonValue["maxLength"]);
        } else if (type == "gear") {
            RUBEGearJointPtr gearJoint = boost::make_shared<RUBEGearJoint>(name);
            joint = gearJoint;

            gearJoint->setJoint1Index(jsonValue["joint1"].asInt());
            gearJoint->setJoint2Index(jsonValue["joint2"].asInt());
            gearJoint->jointDef().ratio = json2float(jsonValue["ratio"]);
        } else {
            LOG4CPLUS_WARN(afutil::logger(),
                           "Ignoring due to unknown joint type");
            return RUBEJointPtr();
        }

        joint->setBodyAIndex(jsonValue["bodyA"].asInt());
        joint->setBodyBIndex(jsonValue["bodyB"].asInt());
        joint->jointDef().collideConnected = jsonValue.get("collideConnected", false).asBool();

        joint->setPath(jsonValue.get("path", "").asString());
        joint->setProps(jsonValue);

        return joint;
    }

    AF_RUBE_JOINT_CLASS_IMPL(RevoluteJoint);
    AF_RUBE_JOINT_CLASS_IMPL(PrismaticJoint);
    AF_RUBE_JOINT_CLASS_IMPL(DistanceJoint);
    AF_RUBE_JOINT_CLASS_IMPL(PulleyJoint);
    AF_RUBE_JOINT_CLASS_IMPL(WheelJoint);
    AF_RUBE_JOINT_CLASS_IMPL(MotorJoint);
    AF_RUBE_JOINT_CLASS_IMPL(WeldJoint);
    AF_RUBE_JOINT_CLASS_IMPL(FrictionJoint);
    AF_RUBE_JOINT_CLASS_IMPL(RopeJoint);
    AF_RUBE_JOINT_CLASS_IMPL(GearJoint);
}
