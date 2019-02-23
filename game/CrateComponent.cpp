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

#include "CrateComponent.h"
#include "SceneObjectFactory.h"
#include "FadeOutComponent.h"
#include "Scene.h"
#include "Const.h"
#include "Settings.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    CrateComponent::CrateComponent(int i, const float powerupProbabilities[PowerupTypeMax + 1], const float ammoProbabilities[WeaponTypeMax + 1])
    : PhasedComponent(phaseThink),
      i_(i)
    {
        for (int j = 0; j < PowerupTypeMax + 1; ++j) {
            powerupProbabilities_[j] = powerupProbabilities[j];
        }
        for (int j = 0; j < WeaponTypeMax + 1; ++j) {
            ammoProbabilities_[j] = ammoProbabilities[j];
        }
        sndBreak_[0] = audio.createSound("crate_break1.ogg");
        sndBreak_[1] = audio.createSound("crate_break2.ogg");
        sndBreak_[2] = audio.createSound("crate_break3.ogg");
    }

    CrateComponent::~CrateComponent()
    {
    }

    void CrateComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void CrateComponent::update(float dt)
    {
        if (parent()->life() <= 0) {
            bool debris = true;

            if (parent()->findComponent<AnimationComponent>()) {
                /*
                 * FIXME: Hack, if there's an animation created by script then
                 * don't leave debris.
                 */
                debris = false;
            }

            SceneObjectPtr gibbed = sceneObjectFactory.createCrateGibbed(i_);

            gibbed->setTransformRecursive(parent()->getTransform());

            b2Vec2 velDir = parent()->getDirection(10.0f);
            b2Rot rot(2.0f * b2_pi / (gibbed->objects().size() + 1));

            if (!debris || (gibbed->type() != SceneObjectTypeGarbage)) {
                FadeOutComponentPtr component =
                    boost::make_shared<FadeOutComponent>(0.5f, 1.0f);
                gibbed->addComponent(component);
            }

            gibbed->setLinearVelocity(parent()->linearVelocity());
            gibbed->setAngularVelocity(b2_pi * 0.5f * ((rand() % 9) - 4));

            for (std::set<SceneObjectPtr>::const_iterator it = gibbed->objects().begin();
                 it != gibbed->objects().end(); ++it) {
                velDir = b2Mul(rot, velDir);

                (*it)->setLinearVelocity(0.25f * parent()->linearVelocity() + velDir);
                (*it)->setAngularVelocity(b2_pi * 0.5f * ((rand() % 9) - 4));

                if (!debris || ((*it)->type() != SceneObjectTypeGarbage)) {
                    FadeOutComponentPtr component =
                        boost::make_shared<FadeOutComponent>(0.5f, 1.0f);

                    (*it)->addComponent(component);
                }
            }

            scene()->addObjectUnparent(gibbed);

            sndBreak_[rand() % 3]->play();

            SceneObjectPtr spawned;

            float val = getRandom(0.0f, 1.0f);
            float prob = 0.0f;

            for (int i = 0; i < PowerupTypeMax + 1; ++i) {
                prob += powerupProbabilities_[i];
                if (val < prob) {
                    PowerupType type = static_cast<PowerupType>(i);
                    switch (type) {
                    case PowerupTypeHealth:
                        spawned = sceneObjectFactory.createPowerupHealth(settings.powerupHealth.timeout,
                            settings.powerupHealth.amount);
                        break;
                    case PowerupTypeGem:
                        spawned = sceneObjectFactory.createPowerupGem(settings.powerupGem.timeout);
                        break;
                    case PowerupTypeAmmo:
                        spawned = spawnAmmo();
                        break;
                    default:
                        break;
                    }
                    break;
                }
            }

            if (spawned) {
                spawned->setPos(parent()->pos());

                scene()->addObject(spawned);
            } else {
                scene()->spawnPowerup(parent()->pos());
            }

            parent()->removeFromParent();
        }
    }

    void CrateComponent::onRegister()
    {
    }

    void CrateComponent::onUnregister()
    {
    }

    SceneObjectPtr CrateComponent::spawnAmmo()
    {
        float val = getRandom(0.0f, 1.0f);
        float prob = 0.0f;

        for (int i = 0; i < WeaponTypeMax + 1; ++i) {
            prob += ammoProbabilities_[i];
            if (val < prob) {
                return sceneObjectFactory.createPowerupAmmo(
                    static_cast<WeaponType>(i));
            }
        }

        return SceneObjectPtr();
    }
}
