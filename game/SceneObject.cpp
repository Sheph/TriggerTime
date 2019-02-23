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

#include "SceneObject.h"
#include "Scene.h"
#include "RenderQuadComponent.h"
#include "LightComponent.h"
#include "PhysicsBodyComponent.h"
#include "Utils.h"
#include "Settings.h"
#include "Const.h"
#include <boost/make_shared.hpp>

namespace af
{
    static SInt32 nextCookie = 0;

    static const b2Vec2 zeroVector(0.0f, 0.0f);

    static void insertComponent(std::vector<ComponentPtr>& components,
                                const ComponentPtr& component)
    {
        for (std::vector<ComponentPtr>::iterator it = components.begin();
             it != components.end();
             ++it ) {
            if (*it == component) {
                return;
            }
        }

        components.push_back(component);
    }

    static bool eraseComponent(std::vector<ComponentPtr>& components,
                               const ComponentPtr& component)
    {
        for (std::vector<ComponentPtr>::iterator it = components.begin();
             it != components.end();
             ++it ) {
            if (*it == component) {
                components.erase(it);
                return true;
            }
        }

        return false;
    }

    SceneObject::SceneObject()
    : cookie_(nextCookie++),
      type_(SceneObjectTypeOther),
      material_(MaterialOther),
      body_(NULL),
      smoothRot_(0.0f),
      smoothRotAngle_(0.0f),
      life_(-1),
      maxLife_(-1),
      freezeRadius_(0.0f),
      blastDamageMultiplier_(1.0f),
      fireDamageMultiplier_(1.0f),
      bulletDamageMultiplier_(1.0f),
      collisionImpulseMultiplier_(1.0f),
      numCollisionComponents_(0),
      active_(true),
      propagateDamage_(true),
      ggMassOverride_(-1.0f),
      ggInertiaOverride_(-1.0f)
    {
        setFreezePhysics(true);
        setDetourable(true);
        bodyDef_.userData = this;
        resetSmooth();
    }

    SceneObject::~SceneObject()
    {
        assert(!scene());

        while (!components_.empty()) {
            ComponentPtr component = *components_.begin();

            removeComponent(component);
        }

        removeAllObjects();

        if (body_) {
            body_->GetWorld()->DestroyBody(body_);
        }
    }

    SceneObject* SceneObject::fromFixture(b2Fixture* fixture)
    {
        return fromBody(fixture->GetBody());
    }

    SceneObject* SceneObject::fromBody(b2Body* body)
    {
        return static_cast<SceneObject*>(body->GetUserData());
    }

    void SceneObject::addComponent(const ComponentPtr& component)
    {
        assert(!component->parent());

        insertComponent(components_, component);
        component->setParent(this);

        if (scene()) {
            scene()->registerComponent(component);
        }
    }

    void SceneObject::removeComponent(const ComponentPtr& component)
    {
        /*
         * Hold on to this component while
         * removing.
         */
        ComponentPtr tmp = component;

        if (eraseComponent(components_, tmp)) {
            if (scene()) {
                scene()->unregisterComponent(tmp);
            }

            tmp->setParent(NULL);
        }
    }

    BehaviorRoamComponentPtr SceneObject::roamBehavior()
    {
        return findOrCreateComponent<BehaviorRoamComponent>();
    }

    BehaviorSeekComponentPtr SceneObject::seekBehavior()
    {
        return findOrCreateComponent<BehaviorSeekComponent>();
    }

    BehaviorAvoidComponentPtr SceneObject::avoidBehavior()
    {
        return findOrCreateComponent<BehaviorAvoidComponent>();
    }

    BehaviorDetourComponentPtr SceneObject::detourBehavior()
    {
        return findOrCreateComponent<BehaviorDetourComponent>();
    }

    BehaviorInterceptComponentPtr SceneObject::interceptBehavior()
    {
        return findOrCreateComponent<BehaviorInterceptComponent>();
    }

    SceneObject* SceneObject::parentObject()
    {
        if (parent() != scene()) {
            return static_cast<SceneObject*>(parent());
        } else {
            return NULL;
        }
    }

    SceneObjectPtr SceneObject::script_parentObject()
    {
        SceneObject* obj = parentObject();
        return obj ? obj->shared_from_this() : SceneObjectPtr();
    }

    void SceneObject::removeFromParent()
    {
        if (parent()) {
            parent()->removeObject(shared_from_this());
        }
    }

    void SceneObject::removeFromParentRecursive()
    {
        SceneObject* obj = this;

        while (obj->parentObject()) {
            obj = obj->parentObject();
        }

        obj->removeFromParent();
    }

    SceneObjectType SceneObject::typeForBlast() const
    {
        /*
         * Stunned enemies should behave like garbage for blasts.
         */
        return ((type() == SceneObjectTypeEnemy) && stunned()) ?
            SceneObjectTypeGarbage : type();
    }

    void SceneObject::setBody(b2Body* value)
    {
        assert(value);
        assert(!body_);

        body_ = value;
        active_ = body_->IsActive();
    }

    void SceneObject::setBodyDef(const b2BodyDef& value)
    {
        bodyDef_ = value;
        bodyDef_.userData = this;
        resetSmooth();
    }

    b2Transform SceneObject::getTransform() const
    {
        if (body_) {
            return body_->GetTransform();
        } else {
            return b2Transform(bodyDef_.position, b2Rot(bodyDef_.angle));
        }
    }

    void SceneObject::setTransform(const b2Vec2& p, float32 a)
    {
        assert(p.IsValid());
        assert(b2IsValid(a));

        b2Vec2 tmpPos = smoothPos_ - pos();
        b2Vec2 tmpPrevPos = smoothPrevPos_ - pos();
        float tmpAng = smoothAngle_ - angle();
        float tmpPrevAng = smoothPrevAngle_ - angle();

        if (body_) {
            body_->SetTransform(p, a);
        } else {
            bodyDef_.position = p;
            bodyDef_.angle = a;
        }

        smoothPos_ = pos() + tmpPos;
        smoothPrevPos_ = pos() + tmpPrevPos;
        smoothAngle_ = angle() + tmpAng;
        smoothPrevAngle_ = angle() + tmpPrevAng;
    }

    void SceneObject::setTransform(const b2Transform& t)
    {
        setTransform(t.p, t.q.GetAngle());
    }

    void SceneObject::setTransformRecursive(const b2Vec2& pos, float32 angle)
    {
        setPosRecursive(pos);
        setAngleRecursive(angle);
    }

    void SceneObject::setTransformRecursive(const b2Transform& t)
    {
        setTransformRecursive(t.p, t.q.GetAngle());
    }

    const b2Vec2& SceneObject::pos() const
    {
        if (body_) {
            return body_->GetPosition();
        } else {
            return bodyDef_.position;
        }
    }

    void SceneObject::setPos(const b2Vec2& value)
    {
        assert(value.IsValid());

        b2Vec2 tmpPos = smoothPos_ - pos();
        b2Vec2 tmpPrevPos = smoothPrevPos_ - pos();

        if (body_) {
            body_->SetTransform(value, body_->GetAngle());
        } else {
            bodyDef_.position = value;
        }

        smoothPos_ = pos() + tmpPos;
        smoothPrevPos_ = pos() + tmpPrevPos;
    }

    void SceneObject::setPosRecursive(const b2Vec2& value)
    {
        b2Vec2 tmp = value - pos();
        setPos(value);
        for (std::set<SceneObjectPtr>::const_iterator it = objects().begin();
             it != objects().end();
             ++it ) {
            (*it)->setPosRecursive((*it)->pos() + tmp);
        }
    }

    void SceneObject::setPosSmoothed(const b2Vec2& value)
    {
        assert(value.IsValid());

        b2Vec2 tmpPos = smoothPos_ - pos();

        if (body_) {
            body_->SetTransform(value, body_->GetAngle());
        } else {
            bodyDef_.position = value;
        }

        smoothPos_ = pos() + tmpPos;
    }

    float SceneObject::angle() const
    {
        if (body_) {
            return body_->GetAngle();
        } else {
            return bodyDef_.angle;
        }
    }

    void SceneObject::setAngle(float value)
    {
        assert(b2IsValid(value));

        float tmpAng = smoothAngle_ - angle();
        float tmpPrevAng = smoothPrevAngle_ - angle();

        if (body_) {
            body_->SetTransform(body_->GetPosition(), value);
        } else {
            bodyDef_.angle = value;
        }

        smoothAngle_ = angle() + tmpAng;
        smoothPrevAngle_ = angle() + tmpPrevAng;
    }

    void SceneObject::setAngleRecursive(float value)
    {
        float tmp = value - angle();
        setAngle(value);
        b2Rot rot(tmp);
        for (std::set<SceneObjectPtr>::const_iterator it = objects().begin();
             it != objects().end();
             ++it ) {
            (*it)->setPosRecursive(pos() + b2Mul(rot, (*it)->pos() - pos()));
            (*it)->setAngleRecursive((*it)->angle() + tmp);
        }
    }

    void SceneObject::setAngleSmoothed(float value)
    {
        float tmpAng = smoothAngle_ - angle();

        if (body_) {
            body_->SetTransform(body_->GetPosition(), value);
        } else {
            bodyDef_.angle = value;
        }

        smoothAngle_ = angle() + tmpAng;
    }

    const b2Vec2& SceneObject::worldCenter() const
    {
        if (body_) {
            return body_->GetWorldCenter();
        } else {
            return bodyDef_.position;
        }
    }

    const b2Vec2& SceneObject::localCenter() const
    {
        if (body_) {
            return body_->GetLocalCenter();
        } else {
            return zeroVector;
        }
    }

    float32 SceneObject::mass() const
    {
        if (body_) {
            return body_->GetMass();
        } else {
            return 0.0f;
        }
    }

    float32 SceneObject::inertia() const
    {
        if (body_) {
            return body_->GetInertia();
        } else {
            return 0.0f;
        }
    }

    const b2Vec2& SceneObject::linearVelocity() const
    {
        if (body_) {
            return body_->GetLinearVelocity();
        } else {
            return bodyDef_.linearVelocity;
        }
    }

    void SceneObject::setLinearVelocity(const b2Vec2& value)
    {
        assert(value.IsValid());

        if (body_) {
            body_->SetLinearVelocity(value);
        } else {
            bodyDef_.linearVelocity = value;
        }
    }

    void SceneObject::setLinearVelocityRecursive(const b2Vec2& value)
    {
        setLinearVelocity(value);
        for (std::set<SceneObjectPtr>::const_iterator it = objects().begin();
             it != objects().end();
             ++it ) {
            (*it)->setLinearVelocityRecursive(value);
        }
    }

    float SceneObject::angularVelocity() const
    {
        if (body_) {
            return body_->GetAngularVelocity();
        } else {
            return bodyDef_.angularVelocity;
        }
    }

    void SceneObject::setAngularVelocity(float value)
    {
        assert(b2IsValid(value));

        if (body_) {
            body_->SetAngularVelocity(value);
        } else {
            bodyDef_.angularVelocity = value;
        }
    }

    float SceneObject::linearDamping() const
    {
        if (body_) {
            return body_->GetLinearDamping();
        } else {
            return bodyDef_.linearDamping;
        }
    }

    void SceneObject::setLinearDamping(float value)
    {
        assert(b2IsValid(value));

        if (body_) {
            body_->SetLinearDamping(value);
        } else {
            bodyDef_.linearDamping = value;
        }
    }

    float SceneObject::angularDamping() const
    {
        if (body_) {
            return body_->GetAngularDamping();
        } else {
            return bodyDef_.angularDamping;
        }
    }

    void SceneObject::setAngularDamping(float value)
    {
        assert(b2IsValid(value));

        if (body_) {
            body_->SetAngularDamping(value);
        } else {
            bodyDef_.angularDamping = value;
        }
    }

    float SceneObject::linearVelocityDamped(float linearVel) const
    {
        return (linearDamping() /
            (1.0f + settings.physics.fixedTimestep * linearDamping())) * linearVel;
    }

    float SceneObject::linearVelocityDamped() const
    {
        return linearVelocityDamped(linearVelocity().Length());
    }

    float SceneObject::angularVelocityDamped(float angularVel) const
    {
        return (angularDamping() /
            (1.0f + settings.physics.fixedTimestep * angularDamping())) * angularVel;
    }

    float SceneObject::angularVelocityDamped() const
    {
        return angularVelocityDamped(angularVelocity());
    }

    void SceneObject::applyForce(const b2Vec2& force, const b2Vec2& point, bool wake)
    {
        assert(force.IsValid());
        assert(point.IsValid());

        if (body_) {
            body_->ApplyForce(force, point, wake);
        }
    }

    void SceneObject::applyForceToCenter(const b2Vec2& force, bool wake)
    {
        assert(force.IsValid());

        if (body_) {
            body_->ApplyForceToCenter(force, wake);
        }
    }

    void SceneObject::applyTorque(float32 torque, bool wake)
    {
        assert(b2IsValid(torque));

        if (body_) {
            body_->ApplyTorque(torque, wake);
        }
    }

    void SceneObject::applyLinearImpulse(const b2Vec2& impulse, const b2Vec2& point, bool wake)
    {
        assert(impulse.IsValid());
        assert(point.IsValid());

        if (body_) {
            body_->ApplyLinearImpulse(impulse, point, wake);
        }
    }

    void SceneObject::applyAngularImpulse(float32 impulse, bool wake)
    {
        assert(b2IsValid(impulse));

        if (body_) {
            body_->ApplyAngularImpulse(impulse, wake);
        }
    }

    bool SceneObject::bullet() const
    {
        if (body_) {
            return body_->IsBullet();
        } else {
            return bodyDef_.bullet;
        }
    }

    void SceneObject::setBullet(bool value)
    {
        if (body_) {
            body_->SetBullet(value);
        } else {
            bodyDef_.bullet = value;
        }
    }

    bool SceneObject::active() const
    {
        if (body_) {
            return active_;
        } else {
            return bodyDef_.active;
        }
    }

    void SceneObject::setActive(bool value)
    {
        if (body_) {
            active_ = value;
            if (!frozen()) {
                body_->SetActive(value);
            }
        } else {
            bodyDef_.active = value;
        }
    }

    void SceneObject::setActiveRecursive(bool value)
    {
        setActive(value);
        for (std::set<SceneObjectPtr>::const_iterator it = objects().begin();
             it != objects().end();
             ++it ) {
            (*it)->setActiveRecursive(value);
        }
    }

    bool SceneObject::sleepingAllowed() const
    {
        if (body_) {
            return body_->IsSleepingAllowed();
        } else {
            return bodyDef_.allowSleep;
        }
    }

    void SceneObject::setSleepingAllowed(bool value)
    {
        if (body_) {
            body_->SetSleepingAllowed(value);
        } else {
            bodyDef_.allowSleep = value;
        }
    }

    bool SceneObject::awake() const
    {
        if (body_) {
            return body_->IsAwake();
        } else {
            return bodyDef_.awake;
        }
    }

    void SceneObject::setAwake(bool value)
    {
        if (body_) {
            body_->SetAwake(value);
        } else {
            bodyDef_.awake = value;
        }
    }

    void SceneObject::resetMassData()
    {
        if (body_) {
            body_->ResetMassData();
        }
    }

    b2Vec2 SceneObject::getWorldPoint(const b2Vec2& localPoint) const
    {
        if (body_) {
            return body_->GetWorldPoint(localPoint);
        }

        b2Transform t(bodyDef_.position, b2Rot(bodyDef_.angle));

        return b2Mul(t, localPoint);
    }

    b2Vec2 SceneObject::getLocalPoint(const b2Vec2& worldPoint) const
    {
        if (body_) {
            return body_->GetLocalPoint(worldPoint);
        }

        b2Transform t(bodyDef_.position, b2Rot(bodyDef_.angle));

        return b2MulT(t, worldPoint);
    }

    b2Vec2 SceneObject::getSmoothWorldPoint(const b2Vec2& localPoint) const
    {
        if (body_) {
            return b2Mul(getSmoothTransform(), localPoint);
        } else {
            return getWorldPoint(localPoint);
        }
    }

    b2Vec2 SceneObject::getSmoothLocalPoint(const b2Vec2& worldPoint) const
    {
        if (body_) {
            return b2MulT(getSmoothTransform(), worldPoint);
        } else {
            return getLocalPoint(worldPoint);
        }
    }

    b2Vec2 SceneObject::getDirection(float length) const
    {
        return b2Mul(getTransform().q, b2Vec2(length, 0.0f));
    }

    void SceneObject::resetSmooth()
    {
        if (body_) {
            smoothPos_ = smoothPrevPos_ = body_->GetPosition();
            smoothAngle_ = smoothPrevAngle_ = body_->GetAngle();
        } else {
            smoothPos_ = smoothPrevPos_ = bodyDef_.position;
            smoothAngle_ = smoothPrevAngle_ = bodyDef_.angle;
        }
    }

    void SceneObject::updateSmooth(float fixedTimestepAccumulatorRatio)
    {
        if (body_) {
            const float oneMinusRatio = 1.0f - fixedTimestepAccumulatorRatio;

            if (body_->GetPosition() != smoothPrevPos_) {
                smoothPos_ = fixedTimestepAccumulatorRatio * body_->GetPosition() +
                    oneMinusRatio * smoothPrevPos_;
            } else {
                smoothPos_ = body_->GetPosition();
            }

            if (body_->GetAngle() != smoothPrevAngle_) {
                smoothAngle_ = fixedTimestepAccumulatorRatio * body_->GetAngle() +
                    oneMinusRatio * smoothPrevAngle_;
            } else {
                smoothAngle_ = body_->GetAngle();
            }
        }
    }

    b2Transform SceneObject::getSmoothTransform() const
    {
        if (smoothAngle_ != smoothRotAngle_) {
            smoothRotAngle_ = smoothAngle_;
            smoothRot_.Set(smoothRotAngle_);
        }

        return b2Transform(smoothPos_, smoothRot_);
    }

    void SceneObject::setLife(float value)
    {
        if ((maxLife_ >= 0) && (value > maxLife_)) {
            life_ = maxLife_;
        } else {
            life_ = value;
        }
    }

    void SceneObject::changeLife(float value)
    {
        SceneObject* pobj;

        if (propagateDamage_ && (pobj = parentObject())) {
            pobj->changeLife(value);
            return;
        }

        if (dead() ||
            ((value < 0.0f) && (type_ == SceneObjectTypePlayer) && scene() && scene()->cutscene()) ||
            ((value < 0.0f) && flags_[FlagInvulnerable])) {
            return;
        }

        setLife(life_ + value);
    }

    void SceneObject::changeLife2(SceneObject* missile, float value)
    {
        changeLife2(missile->type(), value);
    }

    void SceneObject::changeLife2(SceneObjectType missileType, float value)
    {
        if (missileType == SceneObjectTypeNeutralMissile) {
            changeLife(value);
        } else if (missileType == SceneObjectTypeEnemyMissile) {
            if (((type_ != SceneObjectTypeEnemy) && (type_ != SceneObjectTypeEnemyBuilding)) || stunned()) {
                changeLife(value);
            }
        } else {
            if ((type_ != SceneObjectTypePlayer) && (type_ != SceneObjectTypeAlly)) {
                changeLife(value);
            }
        }
    }

    void SceneObject::setMaxLife(float value)
    {
        maxLife_ = value;

        if ((maxLife_ >= 0) && (life_ > maxLife_)) {
            life_ = maxLife_;
        }
    }

    float SceneObject::lifePercent() const
    {
        if (dead()) {
            return 0.0f;
        }

        if (maxLife_ < 0) {
            return 1.0f;
        }

        assert(life_ <= maxLife_);

        return static_cast<float>(life_) / maxLife_;
    }

    SceneObject* SceneObject::damageReceiver()
    {
        SceneObject* pobj = this;

        while (pobj->propagateDamage_ && pobj->parentObject()) {
            pobj = pobj->parentObject();
        }

        return pobj;
    }

    void SceneObject::setStunned(bool value)
    {
        flags_[FlagStunned] = value;
    }

    void SceneObject::becomeDeadbody()
    {
        static CollisionFilterPtr deadbodyFilter;

        if (!deadbodyFilter) {
            deadbodyFilter = boost::make_shared<CollisionDeadbodyFilter>();
        }

        type_ = SceneObjectTypeDeadbody;
        setCollisionFilter(deadbodyFilter);

        PhysicsBodyComponentPtr pc = findComponent<PhysicsBodyComponent>();
        if (pc) {
            pc->refilter();
        }
    }

    bool SceneObject::collidesWith(b2Fixture* fixture)
    {
        assert(body_);

        b2ContactFilter* filter = body_->GetWorld()->GetContactManager().m_contactFilter;

        for (b2Fixture* f = body_->GetFixtureList(); f; f = f->GetNext()) {
            if (f->IsSensor()) {
                continue;
            }

            if (filter->ShouldCollide(f, fixture)) {
                return true;
            }
        }

        return false;
    }

    bool SceneObject::visible() const
    {
        for (std::vector<ComponentPtr>::const_iterator it = components_.begin();
             it != components_.end();
             ++it ) {
            const RenderComponentPtr& rc =
                boost::dynamic_pointer_cast<RenderComponent>(*it);
            if (rc && rc->visible()) {
                return true;
            }
            const LightComponentPtr& lc =
                boost::dynamic_pointer_cast<LightComponent>(*it);
            if (lc) {
                for (std::vector<LightPtr>::const_iterator jt = lc->lights().begin();
                     jt != lc->lights().end();
                     ++jt ) {
                    if ((*jt)->visible()) {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    void SceneObject::setVisible(bool value)
    {
        for (std::vector<ComponentPtr>::const_iterator it = components_.begin();
             it != components_.end();
             ++it ) {
            const RenderComponentPtr& rc =
                boost::dynamic_pointer_cast<RenderComponent>(*it);
            if (rc) {
                rc->setVisible(value);
            }
            const LightComponentPtr& lc =
                boost::dynamic_pointer_cast<LightComponent>(*it);
            if (lc) {
                for (std::vector<LightPtr>::const_iterator jt = lc->lights().begin();
                     jt != lc->lights().end();
                     ++jt ) {
                    (*jt)->setVisible(value);
                }
            }
        }
    }

    void SceneObject::setVisibleRecursive(bool value)
    {
        setVisible(value);
        for (std::set<SceneObjectPtr>::const_iterator it = objects().begin();
             it != objects().end();
             ++it ) {
            (*it)->setVisibleRecursive(value);
        }
    }

    Color SceneObject::color() const
    {
        for (std::vector<ComponentPtr>::const_iterator it = components_.begin();
             it != components_.end();
             ++it ) {
            const RenderComponentPtr& rc =
                boost::dynamic_pointer_cast<RenderComponent>(*it);
            if (rc) {
                return rc->color();
            }
            const LightComponentPtr& lc =
                boost::dynamic_pointer_cast<LightComponent>(*it);
            if (lc) {
                for (std::vector<LightPtr>::const_iterator jt = lc->lights().begin();
                     jt != lc->lights().end();
                     ++jt ) {
                    return (*jt)->color();
                }
            }
        }

        return Color(1.0f, 1.0f, 1.0f, 1.0f);
    }

    void SceneObject::setColor(const Color& value)
    {
        for (std::vector<ComponentPtr>::const_iterator it = components_.begin();
             it != components_.end();
             ++it ) {
            const RenderComponentPtr& rc =
                boost::dynamic_pointer_cast<RenderComponent>(*it);
            if (rc) {
                rc->setColor(value);
            }
            const LightComponentPtr& lc =
                boost::dynamic_pointer_cast<LightComponent>(*it);
            if (lc) {
                for (std::vector<LightPtr>::const_iterator jt = lc->lights().begin();
                     jt != lc->lights().end();
                     ++jt ) {
                    (*jt)->setColor(value);
                }
            }
        }
    }

    void SceneObject::setColorRecursive(const Color& value)
    {
        setColor(value);
        for (std::set<SceneObjectPtr>::const_iterator it = objects().begin();
             it != objects().end();
             ++it ) {
            (*it)->setColorRecursive(value);
        }
    }

    float SceneObject::alpha() const
    {
        for (std::vector<ComponentPtr>::const_iterator it = components_.begin();
             it != components_.end();
             ++it ) {
            const RenderComponentPtr& rc =
                boost::dynamic_pointer_cast<RenderComponent>(*it);
            if (rc) {
                return rc->color().rgba[3];
            }
            const LightComponentPtr& lc =
                boost::dynamic_pointer_cast<LightComponent>(*it);
            if (lc) {
                for (std::vector<LightPtr>::const_iterator jt = lc->lights().begin();
                     jt != lc->lights().end();
                     ++jt ) {
                    return (*jt)->color().rgba[3];
                }
            }
        }

        return 1.0f;
    }

    void SceneObject::setAlpha(float value)
    {
        for (std::vector<ComponentPtr>::const_iterator it = components_.begin();
             it != components_.end();
             ++it ) {
            const RenderComponentPtr& rc =
                boost::dynamic_pointer_cast<RenderComponent>(*it);
            if (rc) {
                Color c = rc->color();
                c.rgba[3] = value;
                rc->setColor(c);
            }
            const LightComponentPtr& lc =
                boost::dynamic_pointer_cast<LightComponent>(*it);
            if (lc) {
                for (std::vector<LightPtr>::const_iterator jt = lc->lights().begin();
                     jt != lc->lights().end();
                     ++jt ) {
                    Color c = (*jt)->color();
                    c.rgba[3] = value;
                    (*jt)->setColor(c);
                }
            }
        }
    }

    void SceneObject::setAlphaRecursive(float value)
    {
        setAlpha(value);
        for (std::set<SceneObjectPtr>::const_iterator it = objects().begin();
             it != objects().end();
             ++it ) {
            (*it)->setAlphaRecursive(value);
        }
    }

    b2BodyType SceneObject::script_bodyType() const
    {
        if (body_) {
            return body_->GetType();
        } else {
            return bodyDef_.type;
        }
    }

    void SceneObject::script_setBodyType(b2BodyType value)
    {
        if (body_) {
            body_->SetType(value);
        } else {
            bodyDef_.type = value;
        }
    }

    void SceneObject::script_changePosSmoothed(float x, float y)
    {
        setPosSmoothed(pos() + b2Vec2(x, y));
    }

    void SceneObject::freeze()
    {
        assert(!flags_[FlagFrozen]);

        flags_[FlagFrozen] = true;

        std::set<SceneObjectPtr> tmpObjs = objects();
        std::vector<ComponentPtr> tmpComponents = components();

        for (std::set<SceneObjectPtr>::iterator it = tmpObjs.begin();
             it != tmpObjs.end();
             ++it) {
            (*it)->freeze();
        }

        for (std::vector<ComponentPtr>::const_iterator it = tmpComponents.begin();
             it != tmpComponents.end();
             ++it) {
            scene()->freezeComponent(*it);
        }
    }

    void SceneObject::thaw()
    {
        assert(flags_[FlagFrozen]);

        flags_[FlagFrozen] = false;

        std::set<SceneObjectPtr> tmpObjs = objects();
        std::vector<ComponentPtr> tmpComponents = components();

        for (std::set<SceneObjectPtr>::iterator it = tmpObjs.begin();
             it != tmpObjs.end();
             ++it) {
            (*it)->thaw();
        }

        for (std::vector<ComponentPtr>::const_iterator it = tmpComponents.begin();
             it != tmpComponents.end();
             ++it) {
            scene()->thawComponent(*it);
        }
    }

    template <class T>
    boost::shared_ptr<T> SceneObject::findOrCreateComponent()
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

        boost::shared_ptr<T> component = boost::make_shared<T>();

        addComponent(component);

        return component;
    }
}
