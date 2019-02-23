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

#include "MechComponent.h"
#include "CameraComponent.h"
#include "Scene.h"
#include "Utils.h"
#include "Settings.h"
#include "Renderer.h"
#include "SceneObjectFactory.h"
#include "RenderHealthbarComponent.h"
#include "AssetManager.h"
#include "Const.h"

namespace af
{
    MechComponent::MechComponent(const SceneObjectPtr& torsoObj,
        const RenderQuadComponentPtr& torsoRc,
        const RenderQuadComponentPtr& legsRc,
        const AnimationComponentPtr& torsoAc,
        const AnimationComponentPtr& legsAc,
        const Points& melee2Points,
        float melee2Duration,
        const std::vector<LightPtr>& fireLights)
    : PhasedComponent(phaseThink),
      torsoObj_(torsoObj),
      torsoRc_(torsoRc),
      legsRc_(legsRc),
      torsoAc_(torsoAc),
      legsAc_(legsAc),
      melee2Points_(melee2Points),
      melee2Duration_(melee2Duration),
      fireLights_(fireLights),
      active_(false),
      state_(StateIdle),
      melee2Index_(0),
      t_(0.0f),
      sndWalk_(audio.createSound("mech_walk.ogg")),
      walk1Sounded_(false),
      sndMelee2_(audio.createSound("mech_melee2.ogg")),
      dying_(false),
      dead_(false)
    {
        fireTime_[0] = 0.0001f;
        fireTime_[1] = 0.5f;
        fireTime_[2] = 1.0f;
    }

    MechComponent::~MechComponent()
    {
    }

    void MechComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void MechComponent::update(float dt)
    {
        static std::map<SceneObject*, b2Vec2> objs;

        static SceneObjectTypes directHitTypes = SceneObjectTypes(SceneObjectTypePlayer) |
            SceneObjectTypeEnemy | SceneObjectTypeAlly | SceneObjectTypeEnemyBuilding;

        if (dead_) {
            return;
        }

        if (dying_) {
            bool done = true;

            for (int i = 0; i < 3; ++i) {
                if (fireTime_[i] > 0.0f) {
                    done = false;

                    fireTime_[i] -= dt;

                    if (fireTime_[i] <= 0.0f) {
                        fireTime_[i] = 0.0f;

                        SceneObjectPtr explosion = sceneObjectFactory.createExplosion1(zOrderExplosion);
                        explosion->setPos(torsoObj_->getWorldPoint(fireLights_[i]->pos()));
                        scene()->addObject(explosion);

                        fireLights_[i]->setVisible(true);

                        ParticleEffectComponentPtr pec = assetManager.getParticleEffect("fire1.p",
                            b2Vec2_zero, 0.0f, false);

                        pec->setTransform(b2Transform(fireLights_[i]->pos(), b2Rot(0.0f)));

                        pec->setFixedAngle(true);

                        pec->setZOrder(zOrderEffects);

                        pec->resetEmit();

                        torsoObj_->addComponent(pec);
                    }
                }
            }

            if (done) {
                torsoAc_->setPaused(true);
                parent()->setFreezeRadius(10.0f);
                parent()->setFreezable(true);
                dead_ = true;
                return;
            }
        } else if (parent()->life() <= 0) {
            parent()->setFreezable(false);
            dying_ = true;

            RenderHealthbarComponentPtr hc = parent()->findComponent<RenderHealthbarComponent>();
            if (hc) {
                hc->removeFromParent();
            }

            legsAc_->setPaused(true);
        }

        if (!dying_) {
            if (parent()->linearVelocity().LengthSquared() >= 2.0f) {
                legsAc_->setPaused(false);
            } else {
                legsAc_->setPaused(true);
            }
        }

        bool active = scene()->inputMech()->active() && active_ && !dying_;

        bool primaryPressed = false;
        bool secondaryPressed = false;

        if (active) {
            b2Vec2 moveDir = b2Vec2_zero, lookDir = b2Vec2_zero;

            bool movePressed = scene()->inputMech()->movePressed(moveDir);
            primaryPressed = scene()->inputMech()->primaryPressed(lookDir);
            secondaryPressed = scene()->inputMech()->secondaryPressed(lookDir);

            bool relative = false;

            if (scene()->inputMech()->lookPressed(lookDir, relative) && !relative) {
                lookDir -= scene()->camera()->findComponent<CameraComponent>()->getGamePoint(parent()->pos());
            }

            if (lookDir == b2Vec2_zero) {
                lookDir = moveDir;
            }

            float angularVelocity = 0.0f;
            float linearVelocity = 0.0f;
            float torsoAngularVelocity = 0.0f;

            if (movePressed) {
                b2Vec2 parentDir = angle2vec(parent()->angle() + parent()->angularVelocity() / 6.0f, 1.0f);

                float angle = angleBetween(parentDir, moveDir);

                if (angle >= 0.0f) {
                    angularVelocity = settings.mech.legsTurnSpeed * 15;
                } else {
                    angularVelocity = -settings.mech.legsTurnSpeed * 15;
                }

                if (fabs(angularVelocity * settings.physics.fixedTimestep) > fabs(angle * 15.0f)) {
                    angularVelocity = angle * 15.0f / settings.physics.fixedTimestep;
                }

                linearVelocity = settings.mech.walkSpeed;

                walkSound();
            }

            if (lookDir != b2Vec2_zero) {
                b2Vec2 torsoDir = angle2vec(torsoObj_->angle() + torsoObj_->angularVelocity() / 6.0f, 1.0f);

                float angle = angleBetween(torsoDir, lookDir);

                if (angle >= 0.0f) {
                    torsoAngularVelocity = settings.mech.torsoTurnSpeed * 15;
                } else {
                    torsoAngularVelocity = -settings.mech.torsoTurnSpeed * 15;
                }

                if (fabs(torsoAngularVelocity * settings.physics.fixedTimestep) > fabs(angle * 15.0f)) {
                    torsoAngularVelocity = angle * 15.0f / settings.physics.fixedTimestep;
                }
            }

            parent()->applyTorque(parent()->inertia() *
                (angularVelocity - parent()->angularVelocity() + parent()->angularVelocityDamped()), true);
            if (parent()->linearVelocity().Length() < linearVelocity) {
                parent()->applyForceToCenter(parent()->mass() *
                    parent()->getDirection(linearVelocity * 4.0f + parent()->linearVelocityDamped()), true);
            }
            torsoObj_->applyTorque(torsoObj_->inertia() *
                (torsoAngularVelocity - torsoObj_->angularVelocity() + torsoObj_->angularVelocityDamped()), true);
        }

        t_ -= dt;

        switch (state_) {
        case StateIdle: {
            if (!active) {
                break;
            }
            if (primaryPressed) {
                state_ = StateMelee2;
                torsoAc_->startAnimation(AnimationMelee2);
                break;
            } else if (secondaryPressed) {
                state_ = StatePreGun;
                torsoAc_->startAnimation(AnimationPreGun);
                break;
            }
            break;
        }
        case StateMelee2: {
            if (torsoAc_->animationFrameIndex() >= torsoAc_->animationSpecialIndex(0)) {
                sndMelee2_->play();
                state_ = StateMelee2Hit;
                melee2Index_ = 0;
                t_ = melee2Duration_;
                break;
            }
            break;
        }
        case StateMelee2Hit: {
            if (melee2Index_ >= (melee2Points_.size() - 1)) {
                state_ = StateMelee2Final;
                break;
            }

            b2Vec2 tmp = calcMelee2Point();

            objs.clear();

            scene()->rayCast(torsoObj_->pos(), tmp, boost::bind(&MechComponent::rayCastCb, this,
                _1, _2, _3, _4, boost::ref(objs)));

            b2Vec2 dir = tmp - torsoObj_->pos();
            dir.Normalize();

            b2Vec2 skDir = dir.Skew();

            for (std::map<SceneObject*, b2Vec2>::const_iterator it = objs.begin();
                 it != objs.end(); ++it) {
                scene()->addObject(sceneObjectFactory.createMissileHit1(it->first->shared_from_this(), it->second));

                if (directHitTypes[it->first->type()]) {
                    it->first->applyLinearImpulse(settings.mech.meleeImpulse * dt * skDir, torsoObj_->pos() + b2Dot(it->first->pos() - torsoObj_->pos(), dir) * dir, true);
                } else {
                    it->first->applyLinearImpulse(settings.mech.meleeImpulse * dt * skDir, it->second, true);
                }

                it->first->changeLife(-settings.mech.meleeDamage * dt);
            }

            if (t_ <= 0.0f) {
                ++melee2Index_;
                t_ = melee2Duration_;
            }

            break;
        }
        case StateMelee2Final: {
            if (torsoAc_->animationFinished()) {
                state_ = StateIdle;
                torsoAc_->startAnimation(AnimationDefault);
                break;
            }
            break;
        }
        case StatePreGun: {
            if (!torsoAc_->animationFinished()) {
                break;
            }
            if (secondaryPressed) {
                state_ = StateGun;
                torsoAc_->startAnimation(AnimationGun);
                weapon_->trigger(true);
            } else {
                state_ = StatePostGun;
                torsoAc_->startAnimation(AnimationPostGun);
            }
            break;
        }
        case StateGun: {
            if (!active || !secondaryPressed) {
                state_ = StatePostGun;
                torsoAc_->startAnimation(AnimationPostGun);
                weapon_->trigger(false);
                break;
            }
            weapon_->trigger(true);
            break;
        }
        case StatePostGun: {
            if (!torsoAc_->animationFinished()) {
                break;
            }
            state_ = StateIdle;
            torsoAc_->startAnimation(AnimationDefault);
            break;
        }
        default:
            assert(false);
            break;
        }
    }

    void MechComponent::debugDraw()
    {
        if ((state_ != StateMelee2Hit) ||
            (melee2Index_ >= (melee2Points_.size() - 1))) {
            return;
        }

        b2Vec2 tmp = calcMelee2Point();

        renderer.setProgramColorLines(2.0f);

        RenderLineStrip rop = renderer.renderLineStrip();
        rop.addVertex(torsoObj_->pos().x, torsoObj_->pos().y);
        rop.addVertex(tmp.x, tmp.y);
        rop.addColors(Color(1.0f, 1.0f, 1.0f, 1.0f));
    }

    void MechComponent::setActive(bool value)
    {
        active_ = value;
        scene()->crosshair()->setRelativeObject(value ? torsoObj_ : SceneObjectPtr());
    }

    void MechComponent::onRegister()
    {
    }

    void MechComponent::onUnregister()
    {
        torsoObj_.reset();
    }

    b2Vec2 MechComponent::calcMelee2Point() const
    {
        b2Vec2 cur = torsoObj_->getWorldPoint(melee2Points_[melee2Index_]);
        b2Vec2 next = torsoObj_->getWorldPoint(melee2Points_[melee2Index_ + 1]);

        float a = melee2Duration_ - t_;
        if (a > melee2Duration_) {
            a = melee2Duration_;
        }

        a /= melee2Duration_;

        return a * next + (1.0f - a) * cur;
    }

    float MechComponent::rayCastCb(b2Fixture* fixture, const b2Vec2& point,
        const b2Vec2& normal, float fraction,
        std::map<SceneObject*, b2Vec2>& objs)
    {
        if (fixture->IsSensor() || (SceneObject::fromFixture(fixture) == parent())) {
            return -1;
        }

        if (parent()->type() == SceneObjectTypeEnemy) {
            static SceneObjectTypes hitTypes = SceneObjectTypes(SceneObjectTypeGizmo) |
                SceneObjectTypePlayer | SceneObjectTypeAlly | SceneObjectTypeRock;
            if (!hitTypes[SceneObject::fromFixture(fixture)->type()]) {
                return -1;
            }
        } else {
            static SceneObjectTypes hitTypes = SceneObjectTypes(SceneObjectTypeGizmo) |
                SceneObjectTypeEnemy | SceneObjectTypeEnemyBuilding | SceneObjectTypeRock;
            if (!hitTypes[SceneObject::fromFixture(fixture)->type()]) {
                return -1;
            }
        }

        objs[SceneObject::fromFixture(fixture)] = point;

        return 1;
    }

    void MechComponent::walkSound()
    {
        if (walk1Sounded_) {
            if (legsAc_->animationFrameIndex() >= legsAc_->animationSpecialIndex(1)) {
                walk1Sounded_ = false;
            } else {
                return;
            }
        } else {
            if ((legsAc_->animationFrameIndex() >= legsAc_->animationSpecialIndex(0)) && (legsAc_->animationFrameIndex() < legsAc_->animationSpecialIndex(1))) {
                walk1Sounded_ = true;
            } else {
                return;
            }
        }

        sndWalk_->play();
    }
}
