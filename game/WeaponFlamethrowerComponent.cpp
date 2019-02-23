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

#include "WeaponFlamethrowerComponent.h"
#include "SceneObject.h"
#include "Scene.h"
#include "AssetManager.h"
#include "Const.h"
#include "Renderer.h"
#include "Utils.h"
#include "SingleTweening.h"
#include "GameShell.h"
#include <boost/make_shared.hpp>

namespace af
{
    WeaponFlamethrowerComponent::WeaponFlamethrowerComponent(SceneObjectType objectType)
    : WeaponComponent(objectType, WeaponTypeFlamethrower),
      stopping_(false),
      damageAngle_(deg2rad(10.0f)),
      numRays_(4),
      damage_(0.0f),
      snd_(audio.createSound("flamethrower.ogg")),
      sndFrozen_(false),
      captured_(false),
      haveColors_(false)
    {
        scaleTweening_ = boost::make_shared<SingleTweening>(1.0f, EaseLinear, 0.0f, 1.0f);
        scaleTweenTime_ = 1.0f;
    }

    WeaponFlamethrowerComponent::~WeaponFlamethrowerComponent()
    {
    }

    void WeaponFlamethrowerComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void WeaponFlamethrowerComponent::update(float dt)
    {
        static std::map<SceneObject*, float> objs;

        if (stopping_) {
            if (pec_->isComplete()) {
                pec_->removeFromParent();
                pec_.reset();
                stopping_ = false;
            }
        }

        scaleTweenTime_ += dt;

        if (pec_) {
            applyScale();
        }

        if (light_) {
            if (pec_) {
                light_->setVisible(true);
                light_->setPos(pos());
                light_->setAngle(angle());

                b2Vec2 p1 = b2Mul(parent()->getTransform(), pos());
                b2Vec2 p2;

                if (stopping_ && pec_->emmiters()[0]->getStartPos(p2)) {
                    light_->setNearDistance((p2 - p1).Length());
                } else {
                    light_->setNearDistance(0.0f);
                }

                if (pec_->emmiters()[0]->getFinalPos(p2)) {
                    light_->setFarDistance((p2 - p1).Length());
                } else {
                    light_->setFarDistance(light_->nearDistance());
                }

                if (light_->farDistance() > 0.0f) {
                    light_->setNearOffset(light_->nearDistance() / light_->farDistance());
                }
            } else {
                light_->setVisible(false);
                light_->setFarDistance(0.0f);
            }
        }

        if (!triggerHeld()) {
            if (pec_ && !stopping_) {
                pec_->allowCompletion();
                stopping_ = true;
                snd_->setLoop(false);
            }

            return;
        }

        if (stopping_) {
            return;
        }

        if (!pec_) {
            pec_ = assetManager.getParticleEffect("flamethrower.p", pos(), angle());

            applyColors();

            applyScale();

            pec_->setZOrder(zOrderEffects);

            pec_->resetEmit();

            parent()->addComponent(pec_);

            if (haveSound()) {
                snd_->setLoop(true);
                snd_->play();
            }
        }

        b2Vec2 p2;

        if (!pec_->emmiters()[0]->getFinalPos(p2)) {
            return;
        }

        b2Transform xf = b2Mul(parent()->getTransform(),
            b2Transform(pos(), b2Rot(angle())));

        b2Vec2 p1 = b2Mul(xf, b2Vec2_zero);

        b2Rot rotR(-damageAngle_ / numRays_);
        b2Rot rotL(damageAngle_ / numRays_);

        b2Vec2 dirR = (p2 - p1);
        b2Vec2 dirL = dirR;

        objs.clear();

        for (UInt32 i = 0; i < numRays_ / 2; ++i) {
            dirR = b2Mul(rotR, dirR);
            dirL = b2Mul(rotL, dirL);

            scene()->rayCast(p1, p1 + dirL, boost::bind(&WeaponFlamethrowerComponent::rayCastCb, this,
                _1, _2, _3, _4, boost::ref(objs)));
            scene()->rayCast(p1, p1 + dirR, boost::bind(&WeaponFlamethrowerComponent::rayCastCb, this,
                _1, _2, _3, _4, boost::ref(objs)));
        }

        for (std::map<SceneObject*, float>::const_iterator it = objs.begin();
             it != objs.end(); ++it) {
            static SceneObjectTypes tmpTypes = SceneObjectTypes(SceneObjectTypeEnemy) |
                SceneObjectTypeEnemyBuilding;

            SceneObject* dr = it->first->damageReceiver();

            bool alive = dr->alive();

            it->first->changeLife(-damage_ * (1.0f - it->second) * dt * it->first->fireDamageMultiplier());

            if (alive && !dr->alive() && tmpTypes[dr->type()]) {
                gameShell->incAchievementProgress(AchievementRoasted, 1);
            }
        }
    }

    void WeaponFlamethrowerComponent::debugDraw()
    {
        if (!pec_ || !triggerHeld() || stopping_) {
            return;
        }

        b2Vec2 p2;

        if (!pec_->emmiters()[0]->getFinalPos(p2)) {
            return;
        }

        b2Transform xf = b2Mul(parent()->getTransform(),
            b2Transform(pos(), b2Rot(angle())));

        b2Vec2 p1 = b2Mul(xf, b2Vec2_zero);

        b2Rot rotR(-damageAngle_ / numRays_);
        b2Rot rotL(damageAngle_ / numRays_);

        b2Vec2 dirR = (p2 - p1);
        b2Vec2 dirL = dirR;

        renderer.setProgramColorLines(2.0f);

        for (UInt32 i = 0; i < numRays_ / 2; ++i) {
            dirR = b2Mul(rotR, dirR);
            dirL = b2Mul(rotL, dirL);

            RenderLineStrip rop = renderer.renderLineStrip();
            rop.addVertex(p1.x, p1.y);
            rop.addVertex((p1 + dirL).x, (p1 + dirL).y);
            rop.addColors(Color(1.0f, 1.0f, 1.0f, 1.0f));

            rop = renderer.renderLineStrip();
            rop.addVertex(p1.x, p1.y);
            rop.addVertex((p1 + dirR).x, (p1 + dirR).y);
            rop.addColors(Color(1.0f, 1.0f, 1.0f, 1.0f));
        }
    }

    void WeaponFlamethrowerComponent::onFreeze()
    {
        if (snd_->status() == AudioSource::Playing) {
            snd_->stop();
            sndFrozen_ = true;
        }
    }

    void WeaponFlamethrowerComponent::onThaw()
    {
        if (sndFrozen_) {
            snd_->play();
            sndFrozen_ = false;
        }
    }

    void WeaponFlamethrowerComponent::setHaveLight(bool value)
    {
        if (!(!!light_ ^ value)) {
            return;
        }

        if (!value) {
            lightC_->removeFromParent();
            lightC_.reset();
            light_.reset();

            return;
        }

        light_ = boost::make_shared<ConeLight>();
        light_->setVisible(false);
        light_->setColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
        light_->setDiffuse(false);
        light_->setConeAngle(damageAngle_ - deg2rad(3.0f));
        light_->setXray(true);
        light_->setIntensity(1.1f);

        lightC_ = boost::make_shared<LightComponent>();

        lightC_->attachLight(light_);

        parent()->addComponent(lightC_);
    }

    void WeaponFlamethrowerComponent::setScale(float value, float duration)
    {
        float tmp = scaleTweening_->getValue(scaleTweenTime_);

        if (duration <= 0.0f) {
            scaleTweening_ = boost::make_shared<SingleTweening>(1.0f, EaseLinear, tmp, value);
            scaleTweenTime_ = 1.0f;
        } else {
            scaleTweening_ = boost::make_shared<SingleTweening>(duration, EaseLinear, tmp, value);
            scaleTweenTime_ = 0.0f;
        }
    }

    void WeaponFlamethrowerComponent::setColors(const Color& color1, const Color& color2)
    {
        colors_ = std::make_pair(color1, color2);
        haveColors_ = true;

        if (pec_) {
            applyColors();
        }
    }

    void WeaponFlamethrowerComponent::reload()
    {
    }

    void WeaponFlamethrowerComponent::onRegister()
    {
    }

    void WeaponFlamethrowerComponent::onUnregister()
    {
        if (pec_) {
            pec_->removeFromParent();
            pec_.reset();
        }
        snd_->setLoop(false);

        if (light_) {
            lightC_->removeFromParent();
            lightC_.reset();
            light_.reset();
        }
    }

    float WeaponFlamethrowerComponent::rayCastCb(b2Fixture* fixture, const b2Vec2& point,
        const b2Vec2& normal, float fraction,
        std::map<SceneObject*, float>& objs)
    {
        if (fixture->IsSensor() || (SceneObject::fromFixture(fixture) == parent())) {
            return -1;
        }

        if (objectType() == SceneObjectTypeEnemyMissile) {
            static SceneObjectTypes hitTypes = SceneObjectTypes(SceneObjectTypeGizmo) |
                SceneObjectTypePlayer | SceneObjectTypeAlly | SceneObjectTypeGarbage;
            if (!hitTypes[SceneObject::fromFixture(fixture)->type()]) {
                return -1;
            }
        } else if (objectType() == SceneObjectTypeNeutralMissile) {
            static SceneObjectTypes hitTypes = SceneObjectTypes(SceneObjectTypeGizmo) |
                SceneObjectTypePlayer | SceneObjectTypeAlly |
                SceneObjectTypeEnemy | SceneObjectTypeEnemyBuilding | SceneObjectTypeGarbage;
            if (!hitTypes[SceneObject::fromFixture(fixture)->type()]) {
                return -1;
            }
        } else {
            static SceneObjectTypes hitTypes = SceneObjectTypes(SceneObjectTypeGizmo) |
                SceneObjectTypeEnemy | SceneObjectTypeEnemyBuilding | SceneObjectTypeGarbage;
            if (!hitTypes[SceneObject::fromFixture(fixture)->type()]) {
                return -1;
            }
        }

        objs[SceneObject::fromFixture(fixture)] = fraction;

        return 1;
    }

    void WeaponFlamethrowerComponent::applyScale()
    {
        if (!captured_) {
            capturedValues_[0] = pec_->emmiters()[0]->entry().velocity.highMin();
            capturedValues_[1] = pec_->emmiters()[0]->entry().emission.highMin();
            captured_ = true;
        }

        float tmp = scaleTweening_->getValue(scaleTweenTime_);

        pec_->emmiters()[0]->entry().velocity.setHigh(capturedValues_[0] * tmp);
        pec_->emmiters()[0]->entry().emission.setHigh(capturedValues_[1] * tmp);
    }

    void WeaponFlamethrowerComponent::applyColors()
    {
        if (!haveColors_) {
            return;
        }

        std::vector<float> tmp;

        tmp.push_back(0.0f);
        tmp.push_back(1.0f);

        pec_->emmiters()[0]->entry().tint.setTimeline(tmp);

        tmp.clear();

        tmp.push_back(colors_.first.rgba[0]);
        tmp.push_back(colors_.first.rgba[1]);
        tmp.push_back(colors_.first.rgba[2]);
        tmp.push_back(colors_.second.rgba[0]);
        tmp.push_back(colors_.second.rgba[1]);
        tmp.push_back(colors_.second.rgba[2]);

        pec_->emmiters()[0]->entry().tint.setColors(tmp);
    }
}
