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

#include "JointProxy.h"
#include "SceneObject.h"
#include "Scene.h"

namespace af
{
    /*
     * JointProxy.
     */

    JointProxy::JointProxy(const std::string& name)
    : name_(name)
    {
    }

    JointProxy::~JointProxy()
    {
    }

    void JointProxy::remove()
    {
        b2Joint* j = joint();

        if (j) {
            Scene* scene = static_cast<Scene*>(j->GetUserData());
            scene->removeJoint(sharedThis());
        }
    }

    SceneObject* JointProxy::objectA()
    {
        b2Joint* j = joint();
        return j ? SceneObject::fromBody(j->GetBodyA()) : NULL;
    }

    SceneObject* JointProxy::objectB()
    {
        b2Joint* j = joint();
        return j ? SceneObject::fromBody(j->GetBodyB()) : NULL;
    }

    b2Vec2 JointProxy::getAnchorA() const
    {
        b2Joint* j = joint();
        return j ? j->GetAnchorA() : b2Vec2_zero;
    }

    b2Vec2 JointProxy::getAnchorB() const
    {
        b2Joint* j = joint();
        return j ? j->GetAnchorB() : b2Vec2_zero;
    }

    b2Vec2 JointProxy::getReactionForce(float32 inv_dt) const
    {
        b2Joint* j = joint();
        return j ? j->GetReactionForce(inv_dt) : b2Vec2_zero;
    }

    float32 JointProxy::getReactionTorque(float32 inv_dt) const
    {
        b2Joint* j = joint();
        return j ? j->GetReactionTorque(inv_dt) : 0.0f;
    }

    bool JointProxy::active() const
    {
        b2Joint* j = joint();
        return j ? j->IsActive() : false;
    }

    bool JointProxy::collideConnected() const
    {
        b2Joint* j = joint();
        return j ? j->GetCollideConnected() : false;
    }

    boost::shared_ptr<SceneObject> JointProxy::script_objectA()
    {
        SceneObject* obj = objectA();

        if (obj) {
            return obj->shared_from_this();
        } else {
            return SceneObjectPtr();
        }
    }

    boost::shared_ptr<SceneObject> JointProxy::script_objectB()
    {
        SceneObject* obj = objectB();

        if (obj) {
            return obj->shared_from_this();
        } else {
            return SceneObjectPtr();
        }
    }

    /*
     * RevoluteJointProxy.
     */

    RevoluteJointProxy::RevoluteJointProxy(b2RevoluteJoint* jointPtr,
                                           const std::string& name)
    : JointProxy(name),
      joint_(jointPtr)
    {
    }

    RevoluteJointProxy::~RevoluteJointProxy()
    {
    }

    JointProxyPtr RevoluteJointProxy::clone()
    {
        b2RevoluteJoint* j = static_cast<b2RevoluteJoint*>(joint());

        if (!j) {
            return JointProxyPtr();
        }

        b2RevoluteJointDef jointDef;

        jointDef.bodyA = j->GetBodyA();
        jointDef.bodyB = j->GetBodyB();
        jointDef.collideConnected = j->GetCollideConnected();
        jointDef.localAnchorA = j->GetLocalAnchorA();
        jointDef.localAnchorB = j->GetLocalAnchorB();
        jointDef.referenceAngle = j->GetReferenceAngle();
        jointDef.enableLimit = j->IsLimitEnabled();
        jointDef.lowerAngle = j->GetLowerLimit();
        jointDef.upperAngle = j->GetUpperLimit();
        jointDef.enableMotor = j->IsMotorEnabled();
        jointDef.motorSpeed = j->GetMotorSpeed();
        jointDef.maxMotorTorque = j->GetMaxMotorTorque();

        return static_cast<Scene*>(j->GetUserData())->addJoint(&jointDef, name());
    }

    b2Vec2 RevoluteJointProxy::localAnchorA() const
    {
        return joint_ ? joint_->GetLocalAnchorA() : b2Vec2_zero;
    }

    b2Vec2 RevoluteJointProxy::localAnchorB() const
    {
        return joint_ ? joint_->GetLocalAnchorB() : b2Vec2_zero;
    }

    float32 RevoluteJointProxy::referenceAngle() const
    {
        return joint_ ? joint_->GetReferenceAngle() : 0.0f;
    }

    float32 RevoluteJointProxy::jointAngle() const
    {
        return joint_ ? joint_->GetJointAngle() : 0.0f;
    }

    float32 RevoluteJointProxy::jointSpeed() const
    {
        return joint_ ? joint_->GetJointSpeed() : 0.0f;
    }

    bool RevoluteJointProxy::limitEnabled() const
    {
        return joint_ ? joint_->IsLimitEnabled() : false;
    }

    void RevoluteJointProxy::enableLimit(bool flag)
    {
        if (joint_) {
            joint_->EnableLimit(flag);
        }
    }

    float32 RevoluteJointProxy::lowerLimit() const
    {
        return joint_ ? joint_->GetLowerLimit() : 0.0f;
    }

    float32 RevoluteJointProxy::upperLimit() const
    {
        return joint_ ? joint_->GetUpperLimit() : 0.0f;
    }

    void RevoluteJointProxy::setLimits(float32 lower, float32 upper)
    {
        if (joint_) {
            joint_->SetLimits(lower, upper);
        }
    }

    bool RevoluteJointProxy::motorEnabled() const
    {
        return joint_ ? joint_->IsMotorEnabled() : false;
    }

    void RevoluteJointProxy::enableMotor(bool flag)
    {
        if (joint_) {
            joint_->EnableMotor(flag);
        }
    }

    float32 RevoluteJointProxy::motorSpeed() const
    {
        return joint_ ? joint_->GetMotorSpeed() : 0.0f;
    }

    void RevoluteJointProxy::setMotorSpeed(float32 value)
    {
        if (joint_) {
            joint_->SetMotorSpeed(value);
        }
    }

    float32 RevoluteJointProxy::maxMotorTorque() const
    {
        return joint_ ? joint_->GetMaxMotorTorque() : 0.0f;
    }

    void RevoluteJointProxy::setMaxMotorTorque(float32 value)
    {
        if (joint_) {
            joint_->SetMaxMotorTorque(value);
        }
    }

    float32 RevoluteJointProxy::getMotorTorque(float32 inv_dt) const
    {
        return joint_ ? joint_->GetMotorTorque(inv_dt) : 0.0f;
    }

    /*
     * PrismaticJointProxy.
     */

    PrismaticJointProxy::PrismaticJointProxy(b2PrismaticJoint* jointPtr,
                                             const std::string& name)
    : JointProxy(name),
      joint_(jointPtr)
    {
    }

    PrismaticJointProxy::~PrismaticJointProxy()
    {
    }

    JointProxyPtr PrismaticJointProxy::clone()
    {
        assert(false);
        return JointProxyPtr();
    }

    b2Vec2 PrismaticJointProxy::localAnchorA() const
    {
        return joint_ ? joint_->GetLocalAnchorA() : b2Vec2_zero;
    }

    b2Vec2 PrismaticJointProxy::localAnchorB() const
    {
        return joint_ ? joint_->GetLocalAnchorB() : b2Vec2_zero;
    }

    b2Vec2 PrismaticJointProxy::localAxisA() const
    {
        return joint_ ? joint_->GetLocalAxisA() : b2Vec2_zero;
    }

    float32 PrismaticJointProxy::referenceAngle() const
    {
        return joint_ ? joint_->GetReferenceAngle() : 0.0f;
    }

    float32 PrismaticJointProxy::getJointTranslation() const
    {
        return joint_ ? joint_->GetJointTranslation() : 0.0f;
    }

    float32 PrismaticJointProxy::getJointSpeed() const
    {
        return joint_ ? joint_->GetJointSpeed() : 0.0f;
    }

    bool PrismaticJointProxy::limitEnabled() const
    {
        return joint_ ? joint_->IsLimitEnabled() : false;
    }

    void PrismaticJointProxy::enableLimit(bool flag)
    {
        if (joint_) {
            joint_->EnableLimit(flag);
        }
    }

    float32 PrismaticJointProxy::lowerLimit() const
    {
        return joint_ ? joint_->GetLowerLimit() : 0.0f;
    }

    float32 PrismaticJointProxy::upperLimit() const
    {
        return joint_ ? joint_->GetUpperLimit() : 0.0f;
    }

    void PrismaticJointProxy::setLimits(float32 lower, float32 upper)
    {
        if (joint_) {
            joint_->SetLimits(lower, upper);
        }
    }

    bool PrismaticJointProxy::motorEnabled() const
    {
        return joint_ ? joint_->IsMotorEnabled() : false;
    }

    void PrismaticJointProxy::enableMotor(bool flag)
    {
        if (joint_) {
            joint_->EnableMotor(flag);
        }
    }

    float32 PrismaticJointProxy::motorSpeed() const
    {
        return joint_ ? joint_->GetMotorSpeed() : 0.0f;
    }

    void PrismaticJointProxy::setMotorSpeed(float32 value)
    {
        if (joint_) {
            joint_->SetMotorSpeed(value);
        }
    }

    float32 PrismaticJointProxy::maxMotorForce() const
    {
        return joint_ ? joint_->GetMaxMotorForce() : 0.0f;
    }

    void PrismaticJointProxy::setMaxMotorForce(float32 value)
    {
        if (joint_) {
            joint_->SetMaxMotorForce(value);
        }
    }

    float32 PrismaticJointProxy::getMotorForce(float32 inv_dt) const
    {
        return joint_ ? joint_->GetMotorForce(inv_dt) : 0.0f;
    }

    /*
     * DistanceJointProxy.
     */

    DistanceJointProxy::DistanceJointProxy(b2DistanceJoint* jointPtr,
                                           const std::string& name)
    : JointProxy(name),
      joint_(jointPtr)
    {
    }

    DistanceJointProxy::~DistanceJointProxy()
    {
    }

    JointProxyPtr DistanceJointProxy::clone()
    {
        assert(false);
        return JointProxyPtr();
    }

    b2Vec2 DistanceJointProxy::localAnchorA() const
    {
        return joint_ ? joint_->GetLocalAnchorA() : b2Vec2_zero;
    }

    b2Vec2 DistanceJointProxy::localAnchorB() const
    {
        return joint_ ? joint_->GetLocalAnchorB() : b2Vec2_zero;
    }

    float32 DistanceJointProxy::length() const
    {
        return joint_ ? joint_->GetLength() : 0.0f;
    }

    void DistanceJointProxy::setLength(float32 value)
    {
        if (joint_) {
            joint_->SetLength(value);
        }
    }

    float32 DistanceJointProxy::frequency() const
    {
        return joint_ ? joint_->GetFrequency() : 0.0f;
    }

    void DistanceJointProxy::setFrequency(float32 value)
    {
        if (joint_) {
            joint_->SetFrequency(value);
        }
    }

    float32 DistanceJointProxy::dampingRatio() const
    {
        return joint_ ? joint_->GetDampingRatio() : 0.0f;
    }

    void DistanceJointProxy::setDampingRatio(float32 value)
    {
        if (joint_) {
            joint_->SetDampingRatio(value);
        }
    }

    /*
     * PulleyJointProxy.
     */

    PulleyJointProxy::PulleyJointProxy(b2PulleyJoint* jointPtr,
                                       const std::string& name)
    : JointProxy(name),
      joint_(jointPtr)
    {
    }

    PulleyJointProxy::~PulleyJointProxy()
    {
    }

    JointProxyPtr PulleyJointProxy::clone()
    {
        assert(false);
        return JointProxyPtr();
    }

    b2Vec2 PulleyJointProxy::groundAnchorA() const
    {
        return joint_ ? joint_->GetGroundAnchorA() : b2Vec2_zero;
    }

    b2Vec2 PulleyJointProxy::groundAnchorB() const
    {
        return joint_ ? joint_->GetGroundAnchorB() : b2Vec2_zero;
    }

    float32 PulleyJointProxy::lengthA() const
    {
        return joint_ ? joint_->GetLengthA() : 0.0f;
    }

    float32 PulleyJointProxy::lengthB() const
    {
        return joint_ ? joint_->GetLengthB() : 0.0f;
    }

    float32 PulleyJointProxy::ratio() const
    {
        return joint_ ? joint_->GetRatio() : 0.0f;
    }

    float32 PulleyJointProxy::getCurrentLengthA() const
    {
        return joint_ ? joint_->GetCurrentLengthA() : 0.0f;
    }

    float32 PulleyJointProxy::getCurrentLengthB() const
    {
        return joint_ ? joint_->GetCurrentLengthB() : 0.0f;
    }

    /*
     * WheelJointProxy.
     */

    WheelJointProxy::WheelJointProxy(b2WheelJoint* jointPtr,
                                     const std::string& name)
    : JointProxy(name),
      joint_(jointPtr)
    {
    }

    WheelJointProxy::~WheelJointProxy()
    {
    }

    JointProxyPtr WheelJointProxy::clone()
    {
        assert(false);
        return JointProxyPtr();
    }

    b2Vec2 WheelJointProxy::localAnchorA() const
    {
        return joint_ ? joint_->GetLocalAnchorA() : b2Vec2_zero;
    }

    b2Vec2 WheelJointProxy::localAnchorB() const
    {
        return joint_ ? joint_->GetLocalAnchorB() : b2Vec2_zero;
    }

    b2Vec2 WheelJointProxy::localAxisA() const
    {
        return joint_ ? joint_->GetLocalAxisA() : b2Vec2_zero;
    }

    float32 WheelJointProxy::getJointTranslation() const
    {
        return joint_ ? joint_->GetJointTranslation() : 0.0f;
    }

    float32 WheelJointProxy::getJointSpeed() const
    {
        return joint_ ? joint_->GetJointSpeed() : 0.0f;
    }

    bool WheelJointProxy::motorEnabled() const
    {
        return joint_ ? joint_->IsMotorEnabled() : false;
    }

    void WheelJointProxy::enableMotor(bool flag)
    {
        if (joint_) {
            joint_->EnableMotor(flag);
        }
    }

    float32 WheelJointProxy::motorSpeed() const
    {
        return joint_ ? joint_->GetMotorSpeed() : 0.0f;
    }

    void WheelJointProxy::setMotorSpeed(float32 value)
    {
        if (joint_) {
            joint_->SetMotorSpeed(value);
        }
    }

    float32 WheelJointProxy::maxMotorTorque() const
    {
        return joint_ ? joint_->GetMaxMotorTorque() : 0.0f;
    }

    void WheelJointProxy::setMaxMotorTorque(float32 value)
    {
        if (joint_) {
            joint_->SetMaxMotorTorque(value);
        }
    }

    float32 WheelJointProxy::getMotorTorque(float32 inv_dt) const
    {
        return joint_ ? joint_->GetMotorTorque(inv_dt) : 0.0f;
    }

    float32 WheelJointProxy::springFrequencyHz() const
    {
        return joint_ ? joint_->GetSpringFrequencyHz() : 0.0f;
    }

    void WheelJointProxy::setSpringFrequencyHz(float32 value)
    {
        if (joint_) {
            joint_->SetSpringFrequencyHz(value);
        }
    }

    float32 WheelJointProxy::springDampingRatio() const
    {
        return joint_ ? joint_->GetSpringDampingRatio() : 0.0f;
    }

    void WheelJointProxy::setSpringDampingRatio(float32 value)
    {
        if (joint_) {
            joint_->SetSpringDampingRatio(value);
        }
    }

    /*
     * MotorJointProxy.
     */

    MotorJointProxy::MotorJointProxy(b2MotorJoint* jointPtr,
                                     const std::string& name)
    : JointProxy(name),
      joint_(jointPtr)
    {
    }

    MotorJointProxy::~MotorJointProxy()
    {
    }

    JointProxyPtr MotorJointProxy::clone()
    {
        assert(false);
        return JointProxyPtr();
    }

    b2Vec2 MotorJointProxy::linearOffset() const
    {
        return joint_ ? joint_->GetLinearOffset() : b2Vec2_zero;
    }

    void MotorJointProxy::setLinearOffset(const b2Vec2& value)
    {
        if (joint_) {
            joint_->SetLinearOffset(value);
        }
    }

    float32 MotorJointProxy::angularOffset() const
    {
        return joint_ ? joint_->GetAngularOffset() : 0.0f;
    }

    void MotorJointProxy::setAngularOffset(float32 value)
    {
        if (joint_) {
            joint_->SetAngularOffset(value);
        }
    }

    float32 MotorJointProxy::maxForce() const
    {
        return joint_ ? joint_->GetMaxForce() : 0.0f;
    }

    void MotorJointProxy::setMaxForce(float32 value)
    {
        if (joint_) {
            joint_->SetMaxForce(value);
        }
    }

    float32 MotorJointProxy::maxTorque() const
    {
        return joint_ ? joint_->GetMaxTorque() : 0.0f;
    }

    void MotorJointProxy::setMaxTorque(float32 value)
    {
        if (joint_) {
            joint_->SetMaxTorque(value);
        }
    }

    float32 MotorJointProxy::correctionFactor() const
    {
        return joint_ ? joint_->GetCorrectionFactor() : 0.0f;
    }

    void MotorJointProxy::setCorrectionFactor(float32 value)
    {
        if (joint_) {
            joint_->SetCorrectionFactor(value);
        }
    }

    /*
     * WeldJointProxy.
     */

    WeldJointProxy::WeldJointProxy(b2WeldJoint* jointPtr,
                                   const std::string& name)
    : JointProxy(name),
      joint_(jointPtr)
    {
    }

    WeldJointProxy::~WeldJointProxy()
    {
    }

    JointProxyPtr WeldJointProxy::clone()
    {
        b2WeldJoint* j = static_cast<b2WeldJoint*>(joint());

        if (!j) {
            return JointProxyPtr();
        }

        b2WeldJointDef jointDef;

        jointDef.bodyA = j->GetBodyA();
        jointDef.bodyB = j->GetBodyB();
        jointDef.collideConnected = j->GetCollideConnected();
        jointDef.localAnchorA = j->GetLocalAnchorA();
        jointDef.localAnchorB = j->GetLocalAnchorB();
        jointDef.referenceAngle = j->GetReferenceAngle();
        jointDef.frequencyHz = j->GetFrequency();
        jointDef.dampingRatio = j->GetDampingRatio();

        return static_cast<Scene*>(j->GetUserData())->addJoint(&jointDef, name());
    }

    b2Vec2 WeldJointProxy::localAnchorA() const
    {
        return joint_ ? joint_->GetLocalAnchorA() : b2Vec2_zero;
    }

    b2Vec2 WeldJointProxy::localAnchorB() const
    {
        return joint_ ? joint_->GetLocalAnchorB() : b2Vec2_zero;
    }

    float32 WeldJointProxy::referenceAngle() const
    {
        return joint_ ? joint_->GetReferenceAngle() : 0.0f;
    }

    float32 WeldJointProxy::frequency() const
    {
        return joint_ ? joint_->GetFrequency() : 0.0f;
    }

    void WeldJointProxy::setFrequency(float32 value)
    {
        if (joint_) {
            joint_->SetFrequency(value);
        }
    }

    float32 WeldJointProxy::dampingRatio() const
    {
        return joint_ ? joint_->GetDampingRatio() : 0.0f;
    }

    void WeldJointProxy::setDampingRatio(float32 value)
    {
        if (joint_) {
            joint_->SetDampingRatio(value);
        }
    }

    /*
     * FrictionJointProxy.
     */

    FrictionJointProxy::FrictionJointProxy(b2FrictionJoint* jointPtr,
                                           const std::string& name)
    : JointProxy(name),
      joint_(jointPtr)
    {
    }

    FrictionJointProxy::~FrictionJointProxy()
    {
    }

    JointProxyPtr FrictionJointProxy::clone()
    {
        assert(false);
        return JointProxyPtr();
    }

    b2Vec2 FrictionJointProxy::localAnchorA() const
    {
        return joint_ ? joint_->GetLocalAnchorA() : b2Vec2_zero;
    }

    b2Vec2 FrictionJointProxy::localAnchorB() const
    {
        return joint_ ? joint_->GetLocalAnchorB() : b2Vec2_zero;
    }

    float32 FrictionJointProxy::maxForce() const
    {
        return joint_ ? joint_->GetMaxForce() : 0.0f;
    }

    void FrictionJointProxy::setMaxForce(float32 value)
    {
        if (joint_) {
            joint_->SetMaxForce(value);
        }
    }

    float32 FrictionJointProxy::maxTorque() const
    {
        return joint_ ? joint_->GetMaxTorque() : 0.0f;
    }

    void FrictionJointProxy::setMaxTorque(float32 value)
    {
        if (joint_) {
            joint_->SetMaxTorque(value);
        }
    }

    /*
     * RopeJointProxy.
     */

    RopeJointProxy::RopeJointProxy(b2RopeJoint* jointPtr,
                                   const std::string& name)
    : JointProxy(name),
      joint_(jointPtr)
    {
    }

    RopeJointProxy::~RopeJointProxy()
    {
    }

    JointProxyPtr RopeJointProxy::clone()
    {
        b2RopeJoint* j = static_cast<b2RopeJoint*>(joint());

        if (!j) {
            return JointProxyPtr();
        }

        b2RopeJointDef jointDef;

        jointDef.bodyA = j->GetBodyA();
        jointDef.bodyB = j->GetBodyB();
        jointDef.collideConnected = j->GetCollideConnected();
        jointDef.localAnchorA = j->GetLocalAnchorA();
        jointDef.localAnchorB = j->GetLocalAnchorB();
        jointDef.maxLength = j->GetMaxLength();

        return static_cast<Scene*>(j->GetUserData())->addJoint(&jointDef, name());
    }

    b2Vec2 RopeJointProxy::localAnchorA() const
    {
        return joint_ ? joint_->GetLocalAnchorA() : b2Vec2_zero;
    }

    b2Vec2 RopeJointProxy::localAnchorB() const
    {
        return joint_ ? joint_->GetLocalAnchorB() : b2Vec2_zero;
    }

    float32 RopeJointProxy::maxLength() const
    {
        return joint_ ? joint_->GetMaxLength() : 0.0f;
    }

    void RopeJointProxy::setMaxLength(float32 value)
    {
        if (joint_) {
            joint_->SetMaxLength(value);
        }
    }

    b2LimitState RopeJointProxy::limitState() const
    {
        return joint_ ? joint_->GetLimitState() : e_inactiveLimit;
    }

    /*
     * GearJointProxy.
     */

    GearJointProxy::GearJointProxy(b2GearJoint* jointPtr,
                                   const JointProxyPtr& jointProxy1,
                                   const JointProxyPtr& jointProxy2,
                                   const std::string& name)
    : JointProxy(name),
      joint_(jointPtr),
      jointProxy1_(jointProxy1),
      jointProxy2_(jointProxy2)
    {
    }

    GearJointProxy::~GearJointProxy()
    {
    }

    JointProxyPtr GearJointProxy::clone()
    {
        assert(false);
        return JointProxyPtr();
    }

    float32 GearJointProxy::ratio() const
    {
        return joint_ ? joint_->GetRatio() : 0.0f;
    }

    void GearJointProxy::setRatio(float32 value)
    {
        if (joint_) {
            joint_->SetRatio(value);
        }
    }
}
