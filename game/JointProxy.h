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

#ifndef _JOINTPROXY_H_
#define _JOINTPROXY_H_

#include "af/Types.h"
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class SceneObject;

    class JointProxy;
    typedef boost::shared_ptr<JointProxy> JointProxyPtr;

    class JointProxy : boost::noncopyable
    {
    public:
        explicit JointProxy(const std::string& name);
        virtual ~JointProxy();

        virtual JointProxyPtr sharedThis() = 0;

        virtual b2Joint* joint() const = 0;

        virtual void setInvalid() = 0;

        virtual JointProxyPtr clone() = 0;

        inline bool valid() const { return joint() != NULL; }

        inline const std::string& name() const { return name_; }

        void remove();

        SceneObject* objectA();
        SceneObject* objectB();

        b2Vec2 getAnchorA() const;
        b2Vec2 getAnchorB() const;

        b2Vec2 getReactionForce(float32 inv_dt) const;
        float32 getReactionTorque(float32 inv_dt) const;

        bool active() const;

        bool collideConnected() const;

        boost::shared_ptr<SceneObject> script_objectA();
        boost::shared_ptr<SceneObject> script_objectB();

    private:
        std::string name_;
    };

    class RevoluteJointProxy : public boost::enable_shared_from_this<RevoluteJointProxy>,
                               public JointProxy
    {
    public:
        RevoluteJointProxy(b2RevoluteJoint* jointPtr, const std::string& name);
        ~RevoluteJointProxy();

        virtual JointProxyPtr sharedThis() { return shared_from_this(); }

        virtual b2Joint* joint() const { return joint_; }

        virtual void setInvalid() { joint_ = NULL; }

        virtual JointProxyPtr clone();

        b2Vec2 localAnchorA() const;
        b2Vec2 localAnchorB() const;

        float32 referenceAngle() const;
        float32 jointAngle() const;
        float32 jointSpeed() const;

        bool limitEnabled() const;
        void enableLimit(bool flag);

        float32 lowerLimit() const;
        float32 upperLimit() const;
        void setLimits(float32 lower, float32 upper);

        bool motorEnabled() const;
        void enableMotor(bool flag);

        float32 motorSpeed() const;
        void setMotorSpeed(float32 value);

        float32 maxMotorTorque() const;
        void setMaxMotorTorque(float32 value);

        float32 getMotorTorque(float32 inv_dt) const;

    private:
        b2RevoluteJoint* joint_;
    };

    typedef boost::shared_ptr<RevoluteJointProxy> RevoluteJointProxyPtr;

    class PrismaticJointProxy : public boost::enable_shared_from_this<PrismaticJointProxy>,
                                public JointProxy
    {
    public:
        PrismaticJointProxy(b2PrismaticJoint* jointPtr, const std::string& name);
        ~PrismaticJointProxy();

        virtual JointProxyPtr sharedThis() { return shared_from_this(); }

        virtual b2Joint* joint() const { return joint_; }

        virtual void setInvalid() { joint_ = NULL; }

        virtual JointProxyPtr clone();

        b2Vec2 localAnchorA() const;
        b2Vec2 localAnchorB() const;

        b2Vec2 localAxisA() const;

        float32 referenceAngle() const;

        float32 getJointTranslation() const;

        float32 getJointSpeed() const;

        bool limitEnabled() const;
        void enableLimit(bool flag);

        float32 lowerLimit() const;
        float32 upperLimit() const;
        void setLimits(float32 lower, float32 upper);

        bool motorEnabled() const;
        void enableMotor(bool flag);

        float32 motorSpeed() const;
        void setMotorSpeed(float32 value);

        float32 maxMotorForce() const;
        void setMaxMotorForce(float32 value);

        float32 getMotorForce(float32 inv_dt) const;

    private:
        b2PrismaticJoint* joint_;
    };

    typedef boost::shared_ptr<PrismaticJointProxy> PrismaticJointProxyPtr;

    class DistanceJointProxy : public boost::enable_shared_from_this<DistanceJointProxy>,
                               public JointProxy
    {
    public:
        DistanceJointProxy(b2DistanceJoint* jointPtr, const std::string& name);
        ~DistanceJointProxy();

        virtual JointProxyPtr sharedThis() { return shared_from_this(); }

        virtual b2Joint* joint() const { return joint_; }

        virtual void setInvalid() { joint_ = NULL; }

        virtual JointProxyPtr clone();

        b2Vec2 localAnchorA() const;
        b2Vec2 localAnchorB() const;

        float32 length() const;
        void setLength(float32 value);

        float32 frequency() const;
        void setFrequency(float32 value);

        float32 dampingRatio() const;
        void setDampingRatio(float32 value);

    private:
        b2DistanceJoint* joint_;
    };

    typedef boost::shared_ptr<DistanceJointProxy> DistanceJointProxyPtr;

    class PulleyJointProxy : public boost::enable_shared_from_this<PulleyJointProxy>,
                             public JointProxy
    {
    public:
        PulleyJointProxy(b2PulleyJoint* jointPtr, const std::string& name);
        ~PulleyJointProxy();

        virtual JointProxyPtr sharedThis() { return shared_from_this(); }

        virtual b2Joint* joint() const { return joint_; }

        virtual void setInvalid() { joint_ = NULL; }

        virtual JointProxyPtr clone();

        b2Vec2 groundAnchorA() const;
        b2Vec2 groundAnchorB() const;

        float32 lengthA() const;
        float32 lengthB() const;

        float32 ratio() const;

        float32 getCurrentLengthA() const;
        float32 getCurrentLengthB() const;

    private:
        b2PulleyJoint* joint_;
    };

    typedef boost::shared_ptr<PulleyJointProxy> PulleyJointProxyPtr;

    class WheelJointProxy : public boost::enable_shared_from_this<WheelJointProxy>,
                            public JointProxy
    {
    public:
        WheelJointProxy(b2WheelJoint* jointPtr, const std::string& name);
        ~WheelJointProxy();

        virtual JointProxyPtr sharedThis() { return shared_from_this(); }

        virtual b2Joint* joint() const { return joint_; }

        virtual void setInvalid() { joint_ = NULL; }

        virtual JointProxyPtr clone();

        b2Vec2 localAnchorA() const;
        b2Vec2 localAnchorB() const;

        b2Vec2 localAxisA() const;

        float32 getJointTranslation() const;

        float32 getJointSpeed() const;

        bool motorEnabled() const;
        void enableMotor(bool flag);

        float32 motorSpeed() const;
        void setMotorSpeed(float32 value);

        float32 maxMotorTorque() const;
        void setMaxMotorTorque(float32 value);

        float32 getMotorTorque(float32 inv_dt) const;

        float32 springFrequencyHz() const;
        void setSpringFrequencyHz(float32 value);

        float32 springDampingRatio() const;
        void setSpringDampingRatio(float32 value);

    private:
        b2WheelJoint* joint_;
    };

    typedef boost::shared_ptr<WheelJointProxy> WheelJointProxyPtr;

    class MotorJointProxy : public boost::enable_shared_from_this<MotorJointProxy>,
                            public JointProxy
    {
    public:
        MotorJointProxy(b2MotorJoint* jointPtr, const std::string& name);
        ~MotorJointProxy();

        virtual JointProxyPtr sharedThis() { return shared_from_this(); }

        virtual b2Joint* joint() const { return joint_; }

        virtual void setInvalid() { joint_ = NULL; }

        virtual JointProxyPtr clone();

        b2Vec2 linearOffset() const;
        void setLinearOffset(const b2Vec2& value);

        float32 angularOffset() const;
        void setAngularOffset(float32 value);

        float32 maxForce() const;
        void setMaxForce(float32 value);

        float32 maxTorque() const;
        void setMaxTorque(float32 value);

        float32 correctionFactor() const;
        void setCorrectionFactor(float32 value);

    private:
        b2MotorJoint* joint_;
    };

    typedef boost::shared_ptr<MotorJointProxy> MotorJointProxyPtr;

    class WeldJointProxy : public boost::enable_shared_from_this<WeldJointProxy>,
                           public JointProxy
    {
    public:
        WeldJointProxy(b2WeldJoint* jointPtr, const std::string& name);
        ~WeldJointProxy();

        virtual JointProxyPtr sharedThis() { return shared_from_this(); }

        virtual b2Joint* joint() const { return joint_; }

        virtual void setInvalid() { joint_ = NULL; }

        virtual JointProxyPtr clone();

        b2Vec2 localAnchorA() const;
        b2Vec2 localAnchorB() const;

        float32 referenceAngle() const;

        float32 frequency() const;
        void setFrequency(float32 value);

        float32 dampingRatio() const;
        void setDampingRatio(float32 value);

    private:
        b2WeldJoint* joint_;
    };

    typedef boost::shared_ptr<WeldJointProxy> WeldJointProxyPtr;

    class FrictionJointProxy : public boost::enable_shared_from_this<FrictionJointProxy>,
                               public JointProxy
    {
    public:
        FrictionJointProxy(b2FrictionJoint* jointPtr, const std::string& name);
        ~FrictionJointProxy();

        virtual JointProxyPtr sharedThis() { return shared_from_this(); }

        virtual b2Joint* joint() const { return joint_; }

        virtual void setInvalid() { joint_ = NULL; }

        virtual JointProxyPtr clone();

        b2Vec2 localAnchorA() const;
        b2Vec2 localAnchorB() const;

        float32 maxForce() const;
        void setMaxForce(float32 value);

        float32 maxTorque() const;
        void setMaxTorque(float32 value);

    private:
        b2FrictionJoint* joint_;
    };

    typedef boost::shared_ptr<FrictionJointProxy> FrictionJointProxyPtr;

    class RopeJointProxy : public boost::enable_shared_from_this<RopeJointProxy>,
                           public JointProxy
    {
    public:
        RopeJointProxy(b2RopeJoint* jointPtr, const std::string& name);
        ~RopeJointProxy();

        virtual JointProxyPtr sharedThis() { return shared_from_this(); }

        virtual b2Joint* joint() const { return joint_; }

        virtual void setInvalid() { joint_ = NULL; }

        virtual JointProxyPtr clone();

        b2Vec2 localAnchorA() const;
        b2Vec2 localAnchorB() const;

        float32 maxLength() const;
        void setMaxLength(float32 value);

        b2LimitState limitState() const;

    private:
        b2RopeJoint* joint_;
    };

    typedef boost::shared_ptr<RopeJointProxy> RopeJointProxyPtr;

    class GearJointProxy : public boost::enable_shared_from_this<GearJointProxy>,
                           public JointProxy
    {
    public:
        GearJointProxy(b2GearJoint* jointPtr,
                       const JointProxyPtr& jointProxy1,
                       const JointProxyPtr& jointProxy2,
                       const std::string& name);
        ~GearJointProxy();

        virtual JointProxyPtr sharedThis() { return shared_from_this(); }

        virtual b2Joint* joint() const { return joint_; }

        virtual void setInvalid() { joint_ = NULL; }

        virtual JointProxyPtr clone();

        inline const JointProxyPtr& joint1() const { return jointProxy1_; }
        inline const JointProxyPtr& joint2() const { return jointProxy2_; }

        float32 ratio() const;
        void setRatio(float32 value);

    private:
        b2GearJoint* joint_;
        JointProxyPtr jointProxy1_;
        JointProxyPtr jointProxy2_;
    };

    typedef boost::shared_ptr<GearJointProxy> GearJointProxyPtr;
}

#endif
