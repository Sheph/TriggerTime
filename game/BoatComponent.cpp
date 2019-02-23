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

#include "BoatComponent.h"
#include "PhysicsJointComponent.h"
#include "CameraComponent.h"
#include "Scene.h"
#include "Utils.h"
#include "Settings.h"

namespace af
{
    BoatComponent::BoatComponent(const SceneObjectPtr& gun,
        const std::string& gunJointName)
    : PhasedComponent(phaseThink),
      gun_(gun),
      gunJointName_(gunJointName),
      active_(false),
      sndIdle_(audio.createSound("boat_idle.ogg")),
      sndStart_(audio.createSound("boat_start.ogg")),
      sndStop_(audio.createSound("boat_stop.ogg")),
      sndMove_(audio.createSound("boat_move.ogg"))
    {
        sndIdle_->setLoop(true);
        sndMove_->setLoop(true);
    }

    BoatComponent::~BoatComponent()
    {
    }

    void BoatComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void BoatComponent::update(float dt)
    {
        if (active_) {
            scene()->setRespawnPoint(parent()->getSmoothTransform());
        }

        if (!scene()->inputBoat()->active() || !active_ || scene()->player()->dead()) {
            weapon_->trigger(false);
            if (active_) {
                if (sndMove_->status() == AudioSource::Playing) {
                    sndMove_->stop();
                    sndStop_->play();
                } else if ((sndIdle_->status() == AudioSource::Stopped) && (sndStop_->status() == AudioSource::Stopped)) {
                    sndStart_->stop();
                    sndIdle_->play();
                }
            }
            return;
        }

        if (prevPlayer_ != scene()->player()) {
            if (playerJoint_) {
                playerJoint_->remove();
            }
            prevPlayer_ = scene()->player();
            prevPlayer_->setTransform(parent()->getTransform());
            playerJoint_ = scene()->addWeldJoint(parent()->shared_from_this(), prevPlayer_, prevPlayer_->pos());
        }

        float angularVelocity = 0.0f;
        float linearVelocity = 0.0f;
        float k = 0.0f;

        bool forwardPressed = scene()->inputBoat()->forwardPressed();
        bool backPressed = scene()->inputBoat()->backPressed();
        float speedFactor = 1.0f;

        b2Vec2 dir = b2Vec2_zero;

        if (scene()->inputBoat()->movePressed(dir)) {
            b2Vec2 parentDir = angle2vec(parent()->angle() + parent()->angularVelocity() / 6.0f, 1.0f);

            float angle = angleBetween(parentDir, dir);

            forwardPressed = fabs(angle) <= b2_pi / 2.0f;
            backPressed = !forwardPressed;
            speedFactor = dir.Length();

            if (backPressed) {
                angle = -angle;
            }

            if (angle >= 0.0f) {
                angularVelocity = settings.boat.turnSpeed;
            } else {
                angularVelocity = -settings.boat.turnSpeed;
            }

            if (fabs(angularVelocity * settings.physics.fixedTimestep) > fabs(angle)) {
                angularVelocity = angle / settings.physics.fixedTimestep;
            }
        }

        if (scene()->inputBoat()->turnLeftPressed()) {
            angularVelocity = settings.boat.turnSpeed;
        } else if (scene()->inputBoat()->turnRightPressed()) {
            angularVelocity = -settings.boat.turnSpeed;
        }

        if (forwardPressed) {
            linearVelocity = settings.boat.forwardSpeed * speedFactor;
            k = 1.0f;
        } else if (backPressed) {
            linearVelocity = -settings.boat.backSpeed * speedFactor;
            k = -1.0f;
        }

        if (forwardPressed || backPressed) {
            if (sndIdle_->status() == AudioSource::Playing) {
                sndIdle_->stop();
                sndStart_->play();
            } else if ((sndMove_->status() == AudioSource::Stopped) && (sndStart_->status() == AudioSource::Stopped)) {
                sndStop_->stop();
                sndMove_->play();
            }
        } else {
            if (sndMove_->status() == AudioSource::Playing) {
                sndMove_->stop();
                sndStop_->play();
            } else if ((sndIdle_->status() == AudioSource::Stopped) && (sndStop_->status() == AudioSource::Stopped)) {
                sndStart_->stop();
                sndIdle_->play();
            }
        }

        parent()->applyTorque(parent()->inertia() * (angularVelocity - parent()->angularVelocity()), true);

        if (parent()->linearVelocity().Length() < linearVelocity * k) {
            parent()->applyForceToCenter(parent()->mass() * parent()->getDirection(linearVelocity), true);
        }

        dir = b2Vec2_zero;

        bool shootPressed = scene()->inputBoat()->shootPressed(dir);

        b2Vec2 pos = b2Vec2_zero;

        bool relative = false;

        if (scene()->inputBoat()->lookPressed(pos, relative)) {
            if (relative) {
                pos += gun_->pos();
            } else {
                pos = scene()->camera()->findComponent<CameraComponent>()->getWorldPoint(pos);
            }

            float angle = normalizeAngle(vec2angle(pos - gun_->pos()) - (joint_->lowerLimit() + parent()->angle()));

            float angleDt = 0.0f;

            if (angle >= 0.0f) {
                angleDt = settings.boat.gunTurnSpeed * settings.physics.fixedTimestep;
            } else {
                angleDt = -settings.boat.gunTurnSpeed * settings.physics.fixedTimestep;
            }

            if (fabs(angleDt) > fabs(angle)) {
                angleDt = angle;
            }

            joint_->setLimits(joint_->lowerLimit() + angleDt, joint_->lowerLimit() + angleDt);
        } else if (dir != b2Vec2_zero) {
            float angle = normalizeAngle(vec2angle(dir) - (joint_->lowerLimit() + parent()->angle()));

            float angleDt = 0.0f;

            if (angle >= 0.0f) {
                angleDt = settings.boat.gunTurnSpeed * settings.physics.fixedTimestep;
            } else {
                angleDt = -settings.boat.gunTurnSpeed * settings.physics.fixedTimestep;
            }

            if (fabs(angleDt) > fabs(angle)) {
                angleDt = angle;
            }

            joint_->setLimits(joint_->lowerLimit() + angleDt, joint_->lowerLimit() + angleDt);
        }

        weapon_->trigger(shootPressed);
    }

    void BoatComponent::setActive(bool value)
    {
        active_ = value;
        if (active_) {
            scene()->crosshair()->setRelativeObject(gun_);

            scene()->setRespawnPoint(parent()->getSmoothTransform());

            if (prevPlayer_ != scene()->player()) {
                if (playerJoint_) {
                    playerJoint_->remove();
                }
                prevPlayer_ = scene()->player();
                prevPlayer_->setTransform(parent()->getTransform());
                playerJoint_ = scene()->addWeldJoint(parent()->shared_from_this(), prevPlayer_, prevPlayer_->pos());
            }

            sndIdle_->play();
        } else {
            scene()->crosshair()->setRelativeObject(SceneObjectPtr());

            if (playerJoint_) {
                playerJoint_->remove();
                playerJoint_.reset();
            }
            prevPlayer_.reset();

            sndIdle_->stop();
            sndStart_->stop();
            sndStop_->stop();
            sndMove_->stop();
        }
    }

    void BoatComponent::onRegister()
    {
        joint_ = parent()->findComponent<PhysicsJointComponent>()->joint<RevoluteJointProxy>(gunJointName_);
    }

    void BoatComponent::onUnregister()
    {
        gun_.reset();
        prevPlayer_.reset();
        if (playerJoint_) {
            playerJoint_->remove();
            playerJoint_.reset();
        }
    }
}
