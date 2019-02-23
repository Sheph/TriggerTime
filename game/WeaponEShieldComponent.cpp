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

#include "WeaponEShieldComponent.h"
#include "SceneObject.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "Settings.h"
#include "InputManager.h"
#include "Utils.h"
#include "FadeOutComponent.h"
#include <boost/make_shared.hpp>

namespace af
{
    WeaponEShieldComponent::WeaponEShieldComponent(SceneObjectType objectType)
    : WeaponComponent(objectType, WeaponTypeEShield),
      released_(true),
      sndUp_(audio.createSound("eshield_up.ogg")),
      sndDown_(audio.createSound("eshield_down.ogg")),
      distance_(0.0f)
    {
    }

    WeaponEShieldComponent::~WeaponEShieldComponent()
    {
    }

    void WeaponEShieldComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void WeaponEShieldComponent::update(float dt)
    {
        if (eshield_ && !eshield_->parent()) {
            eshield_.reset();
        }

        if (canceled() && eshield_ && !eshield_->findComponent<FadeOutComponent>()) {
            FadeOutComponentPtr component =
                boost::make_shared<FadeOutComponent>(0.5f);

            eshield_->addComponent(component);
            released_ = true;
        }

        if (eshield_) {
            if (eshield_->findComponent<FadeOutComponent>()) {
                changeAmmo(1.0f * dt, true);
            } else {
                if (!changeAmmo(1.0f * dt, true)) {
                    if (haveSound()) {
                        sndDown_->play();
                    }

                    FadeOutComponentPtr component =
                        boost::make_shared<FadeOutComponent>(0.5f);

                    eshield_->addComponent(component);
                }
            }

            float ang = parent()->angle() + angle();
            eshield_->setLinearVelocity((1.0f / settings.physics.fixedTimestep) * (parent()->getWorldPoint(pos()) - eshield_->pos() + angle2vec(ang, distance_)));
            eshield_->setAngularVelocity((1.0f / settings.physics.fixedTimestep) * normalizeAngle(ang - eshield_->angle()));
        }

        if (triggerHeld()) {
            if (released_) {
                if (eshield_) {
                    if (!eshield_->findComponent<FadeOutComponent>()) {
                        if (haveSound()) {
                            sndDown_->play();
                        }

                        FadeOutComponentPtr component =
                            boost::make_shared<FadeOutComponent>(0.5f);

                        eshield_->addComponent(component);
                    }
                } else {
                    if (changeAmmo(1.0f * dt)) {
                        eshield_ = sceneObjectFactory.createEShield(objectType());
                        eshield_->setTransform(b2Mul(parent()->getTransform(), b2Transform(pos() + angle2vec(angle(), distance_), b2Rot(angle()))));
                        scene()->addObject(eshield_);
                        if (haveSound()) {
                            sndUp_->play();
                        }
                    }
                }
                released_ = false;
            }
        } else {
            released_ = true;
        }
    }

    void WeaponEShieldComponent::reload()
    {
    }

    void WeaponEShieldComponent::onRegister()
    {
    }

    void WeaponEShieldComponent::onUnregister()
    {
        if (eshield_) {
            eshield_->removeFromParent();
            eshield_.reset();
        }
    }
}
