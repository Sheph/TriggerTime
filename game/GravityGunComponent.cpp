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

#include "GravityGunComponent.h"
#include "PhysicsBodyComponent.h"
#include "StunnedComponent.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "Settings.h"
#include "Utils.h"
#include "Const.h"
#include "FireballComponent.h"
#include <boost/make_shared.hpp>

namespace af
{
    GravityGunComponent::GravityGunComponent(const b2Vec2& pos,
        const SceneObjectPtr& target, const b2Vec2& holdPos)
    : PhasedComponent(phaseThink),
      pos_(pos),
      target_(target),
      holdPos_(holdPos),
      maxLength_(0.0f),
      sndHold_(audio.createSound("gg_hold.ogg")),
      sndShoot_(audio.createSound("gg_shoot.ogg"))
    {
        light_ = boost::make_shared<LineLight>();
        light_->setDiffuse(true);
        light_->setColor(Color(0.5f, 0.5f, 1.0f, 1.0f));
        light_->setXray(true);
        light_->setDistance(2.0f);
        light_->setBothWays(true);
    }

    GravityGunComponent::~GravityGunComponent()
    {
    }

    void GravityGunComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void GravityGunComponent::update(float dt)
    {
        if (!dummy_) {
            return;
        }

        dummy_->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * (b2Mul(parent()->getTransform(), pos_) - dummy_->pos()));
        dummy_->setAngularVelocity((1.0f / settings.physics.fixedTimestep) * normalizeAngle(parent()->angle() - dummy_->angle()));

        b2Vec2 tmp = target_->pos() - dummy_->pos();
        float angle = vec2angle(tmp) - dummy_->angle();
        beam_->setAngle(angle);
        beam_->setLength(tmp.Length());

        light_->setAngle((b2_pi / 2.0f) + angle);
        light_->setPos(pos_ + angle2vec(angle, tmp.Length() / 2));
        light_->setLength(tmp.Length() / 2);

        if (!target_->scene() || target_->gravityGunQuietCancel()) {
            return;
        }

        if ((tmp.Length() < maxLength_) &&
            (tmp.Length() >= (holdPos_ - pos_).Length())) {
            maxLength_ = tmp.Length();
        }

        if (tmp.Length() > (maxLength_ + settings.player.ggBreakLength)) {
            releaseTarget(false);
        } else if (fabs(normalizeAngle(angle)) > settings.player.ggBreakAngle) {
            releaseTarget(false);
        }
    }

    void GravityGunComponent::releaseTarget(bool shoot)
    {
        if (!target_ || !target_->scene() || target_->gravityGunQuietCancel()) {
            return;
        }

        joint_->remove();
        joint_.reset();

        if (shoot) {
            b2Vec2 tmp = b2Mul(parent()->getTransform(), holdPos_) -
                b2Mul(parent()->getTransform(), pos_);
            tmp.Normalize();

            target_->applyLinearImpulse(target_->mass() *
                (settings.player.ggShootVelocity +
                 target_->linearVelocityDamped(settings.player.ggShootVelocity)) * tmp,
                target_->worldCenter(), true);
        } else {
            target_->setGravityGunDropped(true);
        }

        target_->setAwake(true);

        if (target_->type() == SceneObjectTypeEnemy) {
            target_->addComponent(boost::make_shared<StunnedComponent>(shoot ? 0.7f: 0.0f));
        } else {
            target_->setStunned(false);
        }

        dummy_->removeFromParent();
        target_.reset();
        beam_->removeFromParent();
        beam_.reset();
        beamAnimation_->removeFromParent();
        beamAnimation_.reset();

        lightC_->removeFromParent();
        lightC_.reset();
        light_.reset();

        sndHold_->stop();

        sndShoot_->play();
    }

    void GravityGunComponent::onRegister()
    {
        ComponentPtr sc = target_->findComponent<StunnedComponent>();
        if (sc) {
            /*
             * Stun is about to wear out, cancel that.
             */
            sc->removeFromParent();
        }

        target_->setStunned(true);

        if (target_->type() == SceneObjectTypeEnemy) {
            PhysicsBodyComponentPtr pc = target_->findComponent<PhysicsBodyComponent>();
            b2Filter filter = pc->getFilter();
            filter.categoryBits |= collisionBitGeneral;
            filter.maskBits |= collisionBitEnemy | collisionBitEnemyBuilding;
            pc->setFilter(filter);
        }

        dummy_ = sceneObjectFactory.createDummy();

        dummy_->setPos(b2Mul(parent()->getTransform(), pos_));
        dummy_->setAngle(parent()->angle());

        scene()->addObject(dummy_);

        joint_ = scene()->addMotorJoint(dummy_, target_,
            holdPos_ - pos_,
            target_->ggMass() * (settings.player.ggHoldVelocity + target_->linearVelocityDamped(settings.player.ggHoldVelocity)),
            target_->ggInertia() * (settings.player.ggHoldVelocity + target_->angularVelocityDamped(settings.player.ggHoldVelocity)),
            0.1f, true);

        beamAnimation_ = sceneObjectFactory.createLightningAnimation();

        beamAnimation_->startAnimation(AnimationDefault);

        beam_ = boost::make_shared<RenderBeamComponent>(pos_, 0.0f, 2.0f,
            beamAnimation_->drawable(), zOrderEffects - 1);

        b2Vec2 tmp = target_->pos() - dummy_->pos();
        float angle = vec2angle(tmp) - dummy_->angle();
        beam_->setAngle(angle);
        beam_->setLength(tmp.Length());

        maxLength_ = (std::max)(tmp.Length(), ((holdPos_ - pos_).Length()));

        light_->setAngle((b2_pi / 2.0f) + angle);
        light_->setPos(pos_ + angle2vec(angle, tmp.Length() / 2));
        light_->setLength(tmp.Length() / 2);

        lightC_ = boost::make_shared<LightComponent>();

        lightC_->attachLight(light_);

        parent()->addComponent(beam_);
        parent()->addComponent(beamAnimation_);
        parent()->addComponent(lightC_);

        sndHold_->setLoop(true);
        sndHold_->play();
    }

    void GravityGunComponent::onUnregister()
    {
        if (target_) {
            target_->setStunned(false);
            target_->setGravityGunDropped(true);
            if (target_->type() == SceneObjectTypeEnemy) {
                target_->findComponent<PhysicsBodyComponent>()->restoreFilter();
            }

            joint_.reset();
            dummy_->removeFromParent();
            target_.reset();
            beam_->removeFromParent();
            beam_.reset();
            beamAnimation_->removeFromParent();
            beamAnimation_.reset();

            lightC_->removeFromParent();
            lightC_.reset();
            light_.reset();

            sndHold_->stop();
        }
    }
}
