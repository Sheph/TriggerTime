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

#include "GovernedTurretComponent.h"
#include "CameraComponent.h"
#include "Scene.h"
#include "Utils.h"
#include "Settings.h"

namespace af
{
    GovernedTurretComponent::GovernedTurretComponent()
    : PhasedComponent(phaseThink),
      sndMove_(audio.createSound("roboarm_move.ogg")),
      active_(false),
      turnSpeed_(0.0f),
      haveSound_(true)
    {
        sndMove_->setLoop(true);
    }

    GovernedTurretComponent::~GovernedTurretComponent()
    {
    }

    void GovernedTurretComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void GovernedTurretComponent::update(float dt)
    {
        if (!scene()->inputTurret()->active() || !active_) {
            weapon_->trigger(false);
            sndMove_->stop();
            return;
        }

        weapon_->trigger(scene()->inputTurret()->shootPressed());

        if (fabs(parent()->angularVelocity()) > deg2rad(2.0f)) {
            if (sndMove_->status() != AudioSource::Playing) {
                if (haveSound_) {
                    sndMove_->play();
                }
            }
        } else {
            sndMove_->stop();
        }

        b2Vec2 dir = b2Vec2_zero;
        b2Vec2 pos = b2Vec2_zero;
        float angularVelocity = 0.0f;

        if (scene()->inputTurret()->movePressed(dir)) {
            b2Vec2 parentDir = angle2vec(parent()->angle() + parent()->angularVelocity() / 6.0f, 1.0f);

            float angle = angleBetween(parentDir, dir);

            if (angle >= 0.0f) {
                angularVelocity = turnSpeed_;
            } else {
                angularVelocity = -turnSpeed_;
            }

            if (fabs(angularVelocity * settings.physics.fixedTimestep) > fabs(angle)) {
                angularVelocity = angle / settings.physics.fixedTimestep;
            }
        } else if (scene()->inputTurret()->moveToPressed(pos)) {
            pos = scene()->camera()->findComponent<CameraComponent>()->getWorldPoint(pos);

            b2Vec2 parentDir = angle2vec(parent()->angle() + parent()->angularVelocity() / 6.0f, 1.0f);

            float angle = angleBetween(parentDir, pos - parent()->pos());

            if (angle >= 0.0f) {
                angularVelocity = turnSpeed_;
            } else {
                angularVelocity = -turnSpeed_;
            }

            if (fabs(angularVelocity * settings.physics.fixedTimestep) > fabs(angle)) {
                angularVelocity = angle / settings.physics.fixedTimestep;
            }
        }

        parent()->applyTorque(parent()->inertia() * (angularVelocity - parent()->angularVelocity()), true);
    }

    void GovernedTurretComponent::script_setWeapon(const ComponentPtr& value)
    {
        /*
         * FIXME: WAR for luabind.
         * I made luabind handle almost all of shared_ptr aspects instead of
         * this one:
         * If there're functions f(DerivedPtr p), g(BasePtr p) and there's an
         * object that's managed by luabind via BasePtr then f cannot be
         * called, but g can. I'm really tiered of fucking around with luabind,
         * so just make a WAR here that takes BasePtr and does dynamic_cast.
         */

        WeaponComponentPtr tmp = boost::dynamic_pointer_cast<WeaponComponent>(value);
        if (tmp) {
            weapon_ = tmp;
        } else {
            throw std::runtime_error("script_setWeapon cast failed");
        }
    }

    void GovernedTurretComponent::onRegister()
    {
    }

    void GovernedTurretComponent::onUnregister()
    {
        sndMove_->stop();
    }
}
