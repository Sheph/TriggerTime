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

#include "WeaponRopeComponent.h"
#include "SceneObject.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "Settings.h"
#include "FadeOutComponent.h"
#include "PhysicsRopeComponent.h"
#include <boost/make_shared.hpp>

namespace af
{
    WeaponRopeComponent::WeaponRopeComponent(SceneObjectType objectType)
    : WeaponComponent(objectType, WeaponTypeRope),
      length_(10.0f),
      launchSpeed_(10.0f),
      launchDistance_(10.0f),
      pullSpeed_(10.0f),
      pullMaxForce_(1000.0f),
      interval_(0.0f),
      t_(0.0f),
      snd_(audio.createSound("rope_fire.ogg"))
    {
    }

    WeaponRopeComponent::~WeaponRopeComponent()
    {
    }

    void WeaponRopeComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void WeaponRopeComponent::update(float dt)
    {
        if (puller_) {
            PhysicsRopeComponentPtr rc = rope_->findComponent<PhysicsRopeComponent>();
            if ((rc->segmentWidth() * (rc->objects().size() - 1)) <= length_) {
                puller_->setPaused(true);
            }
        } else if (launcher_) {
            PhysicsRopeComponentPtr rc = rope_->findComponent<PhysicsRopeComponent>();
            if (launcher_->launched()) {
                launcher_->removeFromParent();
                launcher_.reset();

                if (rc->hit()) {
                    puller_ = boost::make_shared<RopePullComponent>(rope_,
                        pos(), pullSpeed_, pullMaxForce_);
                    parent()->addComponent(puller_);
                } else {
                    rc->removeFromParent();
                    rope_->addComponent(boost::make_shared<FadeOutComponent>(1.0f));
                    for (std::set<SceneObjectPtr>::const_iterator it = rope_->objects().begin();
                        it != rope_->objects().end(); ++it) {
                        (*it)->addComponent(boost::make_shared<FadeOutComponent>(1.0f));
                    }

                    rope_.reset();
                    t_ = interval_;
                }
            } else if (rc->hit()) {
                launcher_->cut(length_);
            } else {
                rope_->setLinearVelocity(launchSpeed_ * launchDir_);
            }
        } else {
            t_ -= dt;

            if ((t_ > 0.0f) || !triggerHeld()) {
                return;
            }

            b2Transform xf = b2Mul(parent()->getTransform(),
                b2Transform(pos(), b2Rot(0.0f)));

            rope_ = sceneObjectFactory.createRope(launchDistance_);

            rope_->setTransformRecursive(xf);

            setupShot(rope_);

            launchDir_ = parent()->getDirection(1.0f);

            for (std::set<SceneObjectPtr>::const_iterator it =
                rope_->objects().begin(); it != rope_->objects().end();
                ++it) {
                setupShot(*it);
            }

            scene()->addObject(rope_);

            launcher_ = boost::make_shared<RopeLaunchComponent>(rope_,
                pos(), launchSpeed_);

            parent()->addComponent(launcher_);

            snd_->play();
        }
    }

    void WeaponRopeComponent::reload()
    {
    }

    bool WeaponRopeComponent::hit() const
    {
        if (!rope_) {
            return false;
        }
        PhysicsRopeComponentPtr rc = rope_->findComponent<PhysicsRopeComponent>();
        if (!rc) {
            return false;
        }
        return rc->hit();
    }

    void WeaponRopeComponent::tear()
    {
        if (!puller_) {
            return;
        }
        PhysicsRopeComponentPtr rc = rope_->findComponent<PhysicsRopeComponent>();
        puller_->removeFromParent();
        puller_.reset();
        rc->removeFromParent();
        rope_->addComponent(boost::make_shared<FadeOutComponent>(1.0f));
        for (std::set<SceneObjectPtr>::const_iterator it = rope_->objects().begin();
            it != rope_->objects().end(); ++it) {
            (*it)->addComponent(boost::make_shared<FadeOutComponent>(1.0f));
        }

        rope_.reset();
        t_ = interval_;
    }

    void WeaponRopeComponent::onRegister()
    {
    }

    void WeaponRopeComponent::onUnregister()
    {
        if (launcher_) {
            launcher_->removeFromParent();
        }
        if (puller_) {
            puller_->removeFromParent();
        }
        if (rope_) {
            rope_->removeFromParent();
        }
        rope_.reset();
        launcher_.reset();
        puller_.reset();
    }
}
