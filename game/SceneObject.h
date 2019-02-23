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

#ifndef _SCENEOBJECT_H_
#define _SCENEOBJECT_H_

#include "SceneObjectManager.h"
#include "Component.h"
#include "CollisionFilter.h"
#include "BehaviorRoamComponent.h"
#include "BehaviorSeekComponent.h"
#include "BehaviorAvoidComponent.h"
#include "BehaviorDetourComponent.h"
#include "BehaviorInterceptComponent.h"
#include <Box2D/Box2D.h>
#include <boost/enable_shared_from_this.hpp>
#include <vector>

namespace af
{
    class SceneObject : public boost::enable_shared_from_this<SceneObject>,
                        public SceneObjectManager
    {
    public:
        SceneObject();
        ~SceneObject();

        static SceneObject* fromFixture(b2Fixture* fixture);

        static SceneObject* fromBody(b2Body* body);

        inline SInt32 cookie() const { return cookie_; }

        inline const std::string& name() const { return name_; }
        inline void setName(const std::string& value) { name_ = value; }

        void setCollisionFilter(const CollisionFilterPtr& value) { collisionFilter_ = value; }
        inline const CollisionFilterPtr& collisionFilter() const { return collisionFilter_; }

        void addComponent(const ComponentPtr& component);

        void removeComponent(const ComponentPtr& component);

        inline const std::vector<ComponentPtr>& components() const { return components_; }

        template <class T>
        inline boost::shared_ptr<T> findComponent() const
        {
            for (std::vector<ComponentPtr>::const_iterator it = components_.begin();
                 it != components_.end();
                 ++it ) {
                const boost::shared_ptr<T>& component =
                    boost::dynamic_pointer_cast<T>(*it);
                if (component) {
                    return component;
                }
            }
            return boost::shared_ptr<T>();
        }

        template <class T>
        boost::shared_ptr<T> findComponentByName(const std::string& name) const
        {
            for (std::vector<ComponentPtr>::const_iterator it = components_.begin();
                 it != components_.end();
                 ++it ) {
                const boost::shared_ptr<T>& component =
                    boost::dynamic_pointer_cast<T>(*it);
                if (component && (component->name() == name)) {
                    return component;
                }
            }

            return boost::shared_ptr<T>();
        }

        template <class T>
        std::vector< boost::shared_ptr<T> > findComponents(const std::string& name) const
        {
            std::vector< boost::shared_ptr<T> > res;

            for (std::vector<ComponentPtr>::const_iterator it = components_.begin();
                 it != components_.end();
                 ++it ) {
                const boost::shared_ptr<T>& component =
                    boost::dynamic_pointer_cast<T>(*it);
                if (component && (component->name() == name)) {
                    res.push_back(component);
                }
            }

            return res;
        }

        template <class T>
        std::vector< boost::shared_ptr<T> > findComponents() const
        {
            std::vector< boost::shared_ptr<T> > res;

            for (std::vector<ComponentPtr>::const_iterator it = components_.begin();
                 it != components_.end();
                 ++it ) {
                const boost::shared_ptr<T>& component =
                    boost::dynamic_pointer_cast<T>(*it);
                if (component) {
                    res.push_back(component);
                }
            }

            return res;
        }

        /*
         * Behaviors.
         * @{
         */

        BehaviorRoamComponentPtr roamBehavior();
        BehaviorSeekComponentPtr seekBehavior();
        BehaviorAvoidComponentPtr avoidBehavior();
        BehaviorDetourComponentPtr detourBehavior();
        BehaviorInterceptComponentPtr interceptBehavior();

        /*
         * @}
         */

        SceneObject* parentObject();

        SceneObjectPtr script_parentObject();

        void removeFromParent();

        void removeFromParentRecursive();

        inline SceneObjectType type() const { return type_; }
        void setType(SceneObjectType value) { type_ = value; }

        /*
         * FIXME: can we do this nicer?
         */
        SceneObjectType typeForBlast() const;

        inline Material material() const { return material_; }
        void setMaterial(Material value) { material_ = value; }

        inline b2Body* body() { return body_; }
        inline const b2Body* body() const { return body_; }
        void setBody(b2Body* value);

        const b2BodyDef& bodyDef() const { return bodyDef_; }
        void setBodyDef(const b2BodyDef& value);

        b2Transform getTransform() const;
        void setTransform(const b2Vec2& pos, float32 angle);
        void setTransform(const b2Transform& t);

        void setTransformRecursive(const b2Vec2& pos, float32 angle);

        void setTransformRecursive(const b2Transform& t);

        const b2Vec2& pos() const;
        void setPos(const b2Vec2& value);
        void setPosRecursive(const b2Vec2& value);

        void setPosSmoothed(const b2Vec2& value);

        float angle() const;
        void setAngle(float value);
        void setAngleRecursive(float value);

        void setAngleSmoothed(float value);

        const b2Vec2& worldCenter() const;
        const b2Vec2& localCenter() const;

        float32 mass() const;
        float32 inertia() const;

        const b2Vec2& linearVelocity() const;
        void setLinearVelocity(const b2Vec2& value);
        void setLinearVelocityRecursive(const b2Vec2& value);

        float angularVelocity() const;
        void setAngularVelocity(float value);

        float linearDamping() const;
        void setLinearDamping(float value);

        float angularDamping() const;
        void setAngularDamping(float value);

        float linearVelocityDamped(float linearVel) const;
        float linearVelocityDamped() const;

        float angularVelocityDamped(float angularVel) const;
        float angularVelocityDamped() const;

        void applyForce(const b2Vec2& force, const b2Vec2& point, bool wake);
        void applyForceToCenter(const b2Vec2& force, bool wake);
        void applyTorque(float32 torque, bool wake);
        void applyLinearImpulse(const b2Vec2& impulse, const b2Vec2& point, bool wake);
        void applyAngularImpulse(float32 impulse, bool wake);

        bool bullet() const;
        void setBullet(bool value);

        bool active() const;
        void setActive(bool value);
        void setActiveRecursive(bool value);

        bool sleepingAllowed() const;
        void setSleepingAllowed(bool value);

        bool awake() const;
        void setAwake(bool value);

        void resetMassData();

        b2Vec2 getWorldPoint(const b2Vec2& localPoint) const;

        b2Vec2 getLocalPoint(const b2Vec2& worldPoint) const;

        b2Vec2 getSmoothWorldPoint(const b2Vec2& localPoint) const;

        b2Vec2 getSmoothLocalPoint(const b2Vec2& worldPoint) const;

        b2Vec2 getDirection(float length) const;

        void resetSmooth();
        void updateSmooth(float fixedTimestepAccumulatorRatio);

        b2Transform getSmoothTransform() const;

        inline const b2Vec2& smoothPos() const { return smoothPos_; }

        inline float smoothAngle() const { return smoothAngle_; }

        inline float life() const { return life_; }
        void setLife(float value);

        void changeLife(float value);

        void changeLife2(SceneObject* missile, float value);

        void changeLife2(SceneObjectType missileType, float value);

        inline float maxLife() const { return maxLife_; }
        void setMaxLife(float value);

        inline bool alive() const { return life_ > 0; }
        inline bool dead() const { return !alive(); }

        float lifePercent() const;

        SceneObject* damageReceiver();

        inline bool stunned() const { return flags_[FlagStunned]; }
        void setStunned(bool value);

        inline bool gravityGunAware() const { return flags_[FlagGravityGunAware]; }
        inline void setGravityGunAware(bool value) { flags_[FlagGravityGunAware] = value; }

        inline bool invulnerable() const { return flags_[FlagInvulnerable]; }
        inline void setInvulnerable(bool value) { flags_[FlagInvulnerable] = value; }

        inline bool freezable() const { return flags_[FlagFreezable]; }
        inline void setFreezable(bool value) { flags_[FlagFreezable] = value; }

        inline float freezeRadius() const { return freezeRadius_; }
        inline void setFreezeRadius(float value) { freezeRadius_ = value; }

        inline bool freezePhysics() const { return flags_[FlagFreezePhysics]; }
        inline void setFreezePhysics(bool value) { flags_[FlagFreezePhysics] = value; }

        inline bool frozen() const { return flags_[FlagFrozen]; }

        inline float blastDamageMultiplier() const { return blastDamageMultiplier_; }
        inline void setBlastDamageMultiplier(float value) { blastDamageMultiplier_ = value; }

        inline float fireDamageMultiplier() const { return fireDamageMultiplier_; }
        inline void setFireDamageMultiplier(float value) { fireDamageMultiplier_ = value; }

        inline float bulletDamageMultiplier() const { return bulletDamageMultiplier_; }
        inline void setBulletDamageMultiplier(float value) { bulletDamageMultiplier_ = value; }

        inline float collisionImpulseMultiplier() const { return collisionImpulseMultiplier_; }
        inline void setCollisionImpulseMultiplier(float value) { collisionImpulseMultiplier_ = value; }

        inline bool glassy() const { return flags_[FlagGlassy]; }
        inline void setGlassy(bool value) { flags_[FlagGlassy] = value; }

        inline bool propagateDamage() const { return propagateDamage_; }
        inline void setPropagateDamage(bool value) { propagateDamage_ = value; }

        inline bool detourable() const { return flags_[FlagDetourable]; }
        inline void setDetourable(bool value) { flags_[FlagDetourable] = value; }

        inline bool activeDeadbody() const { return flags_[FlagActiveDeadbody]; }
        inline void setActiveDeadbody(bool value) { flags_[FlagActiveDeadbody] = value; }

        inline bool deadbodyAware() const { return flags_[FlagDeadbodyAware]; }
        inline void setDeadbodyAware(bool value) { flags_[FlagDeadbodyAware] = value; }

        inline bool gravityGunDropped() const { return flags_[FlagGravityGunDropped]; }
        inline void setGravityGunDropped(bool value) { flags_[FlagGravityGunDropped] = value; }

        inline bool gravityGunQuietCancel() const { return flags_[FlagGravityGunQuietCancel]; }
        inline void setGravityGunQuietCancel(bool value) { flags_[FlagGravityGunQuietCancel] = value; }

        void becomeDeadbody();

        inline float ggMassOverride() const { return ggMassOverride_; }
        inline void setGGMassOverride(float value) { ggMassOverride_ = value; }

        inline float ggInertiaOverride() const { return ggInertiaOverride_; }
        inline void setGGInertiaOverride(float value) { ggInertiaOverride_ = value; }

        inline float ggMass() const { return (ggMassOverride_ >= 0.0f) ? ggMassOverride_ : mass(); }
        inline float ggInertia() const { return (ggInertiaOverride_ >= 0.0f) ? ggInertiaOverride_ : inertia(); }

        bool collidesWith(b2Fixture* fixture);

        /*
         * Goes over all RenderComponent's and sets
         * them visible/invisible.
         */
        bool visible() const;
        void setVisible(bool value);
        void setVisibleRecursive(bool value);

        /*
         * Goes over all RenderComponent's and sets
         * color.
         */
        Color color() const;
        void setColor(const Color& value);
        void setColorRecursive(const Color& value);

        float alpha() const;
        void setAlpha(float value);
        void setAlphaRecursive(float value);

        b2BodyType script_bodyType() const;
        void script_setBodyType(b2BodyType value);

        inline void changeNumCollisionComponents(int value) { numCollisionComponents_ += value; }
        inline bool haveCollisionComponents() const { return numCollisionComponents_ > 0; }

        void script_changePosSmoothed(float x, float y);

        /*
         * Internal, do not call.
         * @{
         */
        void freeze();
        void thaw();

        inline bool actualActive() const { return active_; }

        /*
         * @}
         */

    private:
        enum Flag
        {
            FlagStunned = 0,
            FlagGravityGunAware,
            FlagInvulnerable,
            FlagFreezable,
            FlagFrozen,
            FlagGlassy,
            FlagFreezePhysics,
            FlagDetourable,
            FlagActiveDeadbody,
            FlagDeadbodyAware,
            FlagGravityGunDropped,
            FlagGravityGunQuietCancel,
        };

        static const int FlagMax = FlagGravityGunQuietCancel;

        typedef EnumSet<Flag, FlagMax + 1> Flags;

        template <class T>
        boost::shared_ptr<T> findOrCreateComponent();

        SInt32 cookie_;

        SceneObjectType type_;

        Material material_;

        std::string name_;

        b2BodyDef bodyDef_;
        b2Body* body_;

        b2Vec2 smoothPrevPos_;
        b2Vec2 smoothPos_;

        float smoothPrevAngle_;
        float smoothAngle_;

        mutable b2Rot smoothRot_;
        mutable float smoothRotAngle_;

        float life_;

        float maxLife_;

        float freezeRadius_;

        float blastDamageMultiplier_;
        float fireDamageMultiplier_;
        float bulletDamageMultiplier_;
        float collisionImpulseMultiplier_;

        std::vector<ComponentPtr> components_;

        CollisionFilterPtr collisionFilter_;

        int numCollisionComponents_;

        Flags flags_;

        bool active_;

        bool propagateDamage_;

        float ggMassOverride_;
        float ggInertiaOverride_;
    };
}

#endif
