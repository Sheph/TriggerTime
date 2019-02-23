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

#include "SawerComponent.h"
#include "Scene.h"
#include "Utils.h"
#include "Const.h"
#include "Settings.h"
#include "SceneObjectFactory.h"
#include "CameraComponent.h"
#include "RenderQuadComponent.h"
#include "SingleTweening.h"
#include "PhysicsJointComponent.h"
#include "PhysicsBodyComponent.h"
#include "FadeOutComponent.h"
#include "BuzzSawComponent.h"
#include "StunnedComponent.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    SawerComponent::SawerComponent(const RenderTentacleComponentPtr& tc,
        const SceneObjectPtr& head)
    : TargetableComponent(phaseThink),
      tc_(tc),
      head_(head),
      state_(StateIdle),
      t_(0.0f),
      sawFreezeRadius_(0.0f)
    {
    }

    SawerComponent::~SawerComponent()
    {
    }

    void SawerComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void SawerComponent::update(float dt)
    {
        if (!interceptBehavior_) {
            interceptBehavior_ = tc_->objects()[tc_->objects().size() - 1]->interceptBehavior();
        }

        t_ -= dt;

        updateAutoTarget(dt);

        switch (state_) {
        case StateIdle: {
            if (target()) {
                interceptBehavior_->reset();
                interceptBehavior_->setTarget(target());
                interceptBehavior_->setAngularVelocity(settings.sawer.turnSpeed * (tc_->objects().size() + 1));
                interceptBehavior_->start();

                state_ = StateWalk;

                t_ = getRandom(0.0f, settings.sawer.deployDelay);
            }
            break;
        }
        case StateWalk: {
            if (!target()) {
                interceptBehavior_->reset();

                state_ = StateIdle;

                break;
            }

            interceptBehavior_->setTarget(target());

            if (!inflate_ && (t_ <= 0.0f)) {
                if (scene()->camera()->findComponent<CameraComponent>()->rectVisible(head_->pos(), 1.0f, 1.0f)) {
                    inflate_ = boost::make_shared<TentacleInflateComponent>(20, 1.6f, settings.sawer.deploySpeed,
                        head_->findComponent<RenderQuadComponent>());
                    parent()->addComponent(inflate_);
                } else {
                    t_ = getRandom(0.0f, settings.sawer.deployDelay);
                }
            }

            if (inflate_ && inflate_->done() && !saw_) {
                saw_ = sceneObjectFactory.createSawerSaw();

                sawFreezeRadius_ = saw_->freezeRadius();
                if (parent()->freezable()) {
                    saw_->setFreezeRadius(parent()->freezeRadius());
                }

                saw_->setTransform(head_->getTransform());
                scene()->addObject(saw_);
                RevoluteJointProxyPtr joint = scene()->addRevoluteJoint(head_, saw_, b2Vec2_zero, b2Vec2_zero);
                joint->setMaxMotorTorque(100000.0f);
                joint->setMotorSpeed(-settings.sawer.sawTurnSpeed);
                joint->enableMotor(true);
                sawJoint_ = joint;
            }

            if (inflate_ && !inflate_->scene()) {
                saw_->setGravityGunAware(true);
                state_ = StateSaw;
                t_ = getRandom(settings.sawer.attackMaxTimeout, settings.sawer.attackMaxTimeout * 2.0f);
                break;
            }

            break;
        }
        case StateSaw: {
            if (saw_->stunned()) {
                state_ = StateSawStunned;
                t_ = settings.sawer.sawPickupDelay;
                break;
            }
            if (t_ <= 0.0f) {
                state_ = StateThrow1;
                t_ = settings.sawer.throwForce1Duration;
                break;
            }
            break;
        }
        case StateSawStunned: {
            StunnedComponentPtr sc = saw_->findComponent<StunnedComponent>();
            if (!saw_->stunned() || sc) {
                if (sc) {
                    sc->removeFromParent();
                    saw_->setStunned(false);
                    saw_->findComponent<PhysicsBodyComponent>()->restoreFilter();
                }
                state_ = StateSaw;
                t_ = getRandom(settings.sawer.attackMinTimeout, settings.sawer.attackMinTimeout * 2.0f);
                break;
            }
            if (t_ <= 0.0f) {
                throwSaw(false);
                break;
            }
            break;
        }
        case StateThrow1: {
            if (saw_->stunned()) {
                state_ = StateSawStunned;
                t_ = settings.sawer.sawPickupDelay;
                break;
            }
            if (t_ <= 0.0f) {
                state_ = StateThrow2;
                t_ = settings.sawer.throwForce2Duration;
                break;
            }
            head_->applyForceToCenter(settings.sawer.throwForce1 * (tc_->objects().size() + 1) * head_->getDirection(1.0f), true);
            break;
        }
        case StateThrow2: {
            if (saw_->stunned()) {
                state_ = StateSawStunned;
                t_ = settings.sawer.sawPickupDelay;
                break;
            }
            if (t_ <= 0.0f) {
                throwSaw(true);
                break;
            }
            b2Vec2 tmp = target()->pos() - head_->pos();
            tmp.Normalize();

            head_->applyForceToCenter(settings.sawer.throwForce2 * (tc_->objects().size() + 1) * tmp, true);
            break;
        }
        default:
            assert(false);
            break;
        }
    }

    void SawerComponent::onFreeze()
    {
        if (sawJoint_) {
            sawJoint_->enableMotor(false);
            saw_->setAngularVelocity(0.0f);
            saw_->setAngularDamping(3.0f);
        }
    }

    void SawerComponent::onThaw()
    {
        if (sawJoint_) {
            saw_->setAngularDamping(0.0f);
            saw_->setAngularVelocity(-settings.sawer.sawTurnSpeed);
            sawJoint_->enableMotor(true);
        }
    }

    void SawerComponent::onRegister()
    {
        float density = tc_->objects()[0]->findComponent<PhysicsBodyComponent>()->density();

        for (size_t i = 0; i < tc_->objects().size(); ++i) {
            float a = static_cast<float>(i) / (tc_->objects().size() - 1);
            tc_->objects()[i]->findComponent<PhysicsBodyComponent>()->setDensity(density * a + 5.0f * density * (1.0f - a));
            tc_->objects()[i]->resetMassData();
        }

        for (size_t i = 1; i < tc_->objects().size(); ++i) {
            scene()->addWeldJointEx(tc_->objects()[i - 1],
                tc_->objects()[i],
                0.5f * (tc_->objects()[i - 1]->pos() + tc_->objects()[i]->pos()),
                6.0f, 0.7f);
        }
        scene()->addWeldJoint(tc_->objects().back(),
            head_,
            0.5f * (tc_->objects().back()->pos() + head_->pos()));
    }

    void SawerComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
        head_.reset();
        if (saw_) {
            saw_->removeFromParent();
            saw_.reset();
        }
    }

    void SawerComponent::throwSaw(bool enemy)
    {
        b2Vec2 tmp = target()->pos() - head_->pos();
        tmp.Normalize();

        sawJoint_->remove();
        sawJoint_.reset();
        inflate_.reset();

        saw_->setFreezeRadius(sawFreezeRadius_);

        if (saw_->frozen()) {
            saw_->removeFromParent();
        } else {
            saw_->setGGMassOverride(saw_->mass() * 4.0f);
            saw_->setGGInertiaOverride(saw_->inertia() * 4.0f);

            saw_->setLinearDamping(0.0f);
            saw_->setLinearVelocity(settings.sawer.sawWalkSpeed * tmp);
            saw_->setAngularVelocity(-settings.sawer.sawTurnSpeed);

            saw_->findComponent<CollisionComponent>()->removeFromParent();

            CollisionBuzzSawMissileComponentPtr cbc = boost::make_shared<CollisionBuzzSawMissileComponent>();

            cbc->setDamage(settings.sawer.sawDamage);

            saw_->addComponent(cbc);

            PhysicsBodyComponentPtr pc = saw_->findComponent<PhysicsBodyComponent>();

            b2Filter filter = pc->getFilter();
            filter.categoryBits = collisionBitGeneral;

            if (enemy) {
                saw_->setType(SceneObjectTypeEnemyMissile);
            } else {
                saw_->setLinearDamping(3.0f);
                cbc->setDamage(settings.sawer.sawFastDamage);

                saw_->setType(SceneObjectTypePlayerMissile);

                filter.maskBits |= (collisionBitEnemy | collisionBitEnemyBuilding);
                filter.maskBits &= ~(collisionBitPlayer | collisionBitAlly);
            }

            pc->setFilterOverride(filter);
            pc->enableAllFixtures();

            BuzzSawComponentPtr c = boost::make_shared<BuzzSawComponent>(cbc, settings.sawer.sawFastDamage);

            saw_->addComponent(c);

            saw_->setCollisionFilter(getBuzzSawFilter());

            saw_->setMaxLife(settings.sawer.sawLife);
            saw_->setLife(settings.sawer.sawLife);

            saw_->setActive(false);
            saw_->setActive(true);
        }

        saw_.reset();
        state_ = StateWalk;
        t_ = getRandom(0.0f, settings.sawer.deployDelay);
    }
}
