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

#ifndef _AF_RUBEJOINT_H_
#define _AF_RUBEJOINT_H_

#include "af/RUBEBase.h"
#include <boost/shared_ptr.hpp>

#define AF_RUBE_JOINT_CLASS(className) \
    class RUBE##className : public RUBEJoint \
    { \
    public: \
        explicit RUBE##className(const std::string& name); \
        ~RUBE##className(); \
        virtual b2##className##Def& jointDef() { return jointDef_; } \
    private: \
        b2##className##Def jointDef_; \
    }; \
    typedef boost::shared_ptr<RUBE##className> RUBE##className##Ptr

namespace af
{
    class RUBEJoint;
    typedef boost::shared_ptr<RUBEJoint> RUBEJointPtr;

    class RUBEJoint : public RUBEBase
    {
    public:
        explicit RUBEJoint(const std::string& name);
        virtual ~RUBEJoint();

        static RUBEJointPtr fromJsonValue(const Json::Value& jsonValue);

        virtual b2JointDef& jointDef() = 0;

        inline int bodyAIndex() const { return bodyAIndex_; }
        inline void setBodyAIndex(int value) { bodyAIndex_ = value; }

        inline int bodyBIndex() const { return bodyBIndex_; }
        inline void setBodyBIndex(int value) { bodyBIndex_ = value; }

        inline const std::string& path() const { return path_; }
        inline void setPath(const std::string& value) { path_ = value; }

    private:
        int bodyAIndex_;
        int bodyBIndex_;
        std::string path_;
    };

    AF_RUBE_JOINT_CLASS(RevoluteJoint);
    AF_RUBE_JOINT_CLASS(PrismaticJoint);
    AF_RUBE_JOINT_CLASS(DistanceJoint);
    AF_RUBE_JOINT_CLASS(PulleyJoint);
    AF_RUBE_JOINT_CLASS(WheelJoint);
    AF_RUBE_JOINT_CLASS(MotorJoint);
    AF_RUBE_JOINT_CLASS(WeldJoint);
    AF_RUBE_JOINT_CLASS(FrictionJoint);
    AF_RUBE_JOINT_CLASS(RopeJoint);

    class RUBEGearJoint : public RUBEJoint
    {
    public:
        explicit RUBEGearJoint(const std::string& name);
        ~RUBEGearJoint();

        virtual b2GearJointDef& jointDef() { return jointDef_; }

        inline int joint1Index() const { return joint1Index_; }
        inline void setJoint1Index(int value) { joint1Index_ = value; }

        inline int joint2Index() const { return joint2Index_; }
        inline void setJoint2Index(int value) { joint2Index_ = value; }

    private:
        b2GearJointDef jointDef_;
        int joint1Index_;
        int joint2Index_;
    };

    typedef boost::shared_ptr<RUBEGearJoint> RUBEGearJointPtr;
}

#endif
