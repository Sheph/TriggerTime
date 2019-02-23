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

#include "SpiderComponent.h"
#include "Scene.h"
#include "Utils.h"
#include "Const.h"
#include "Renderer.h"
#include "Settings.h"
#include "SceneObjectFactory.h"
#include "SingleTweening.h"
#include "PhysicsBodyComponent.h"
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

namespace af
{
    static const float jointFrequency = 10.0f;
    static const float jointDampingRatio = 1.0f;

    SpiderComponent::SpiderComponent(const SceneObjectPtr legs[8],
        const b2Vec2 legTips[8],
        const b2Vec2& bodyTip,
        const AnimationComponentPtr& ac)
    : TargetableComponent(phaseThink),
      bodyTip_(bodyTip),
      ac_(ac),
      state_(StateStart),
      attackTime_(0.0f),
      cornered_(false),
      downSlow_(false),
      legVelocity_(settings.spider.legVelocity),
      canRotate_(true),
      tweenTime_(0.0f),
      trapped_(false),
      sndBite_(audio.createSound("spider_bite.ogg")),
      sndAngry_(audio.createSound("spider_angry.ogg"))
    {
        legPair_[0] = std::make_pair(0, 4);
        legPair_[1] = std::make_pair(1, 5);
        legPair_[2] = std::make_pair(2, 6);
        legPair_[3] = std::make_pair(3, 7);
        legPairIndex_ = 0;

        for (int i = 0; i < 8; ++i) {
            leg_[i] = legs[i];
            legTip_[i] = legTips[i];
        }
    }

    SpiderComponent::~SpiderComponent()
    {
    }

    void SpiderComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void SpiderComponent::update(float dt)
    {
        updateAutoTarget(dt);

        attackTime_ -= dt;

        switch (state_) {
        case StateStart:
            for (int i = 0; i < 4; ++i) {
                legMove(0, 4.0f);
            }
            for (int i = 1; i < 8; ++i) {
                legMove(i, 0.0f);
            }
            state_ = StateDown;
            break;
        case StateDown: {
            bool res = true;

            for (int i = 0; i < 8; ++i) {
                res &= legDown(i, dt);
            }

            if (!res) {
                break;
            }

            if (trapped_) {
                state_ = ((rand() % 2) == 0) ? StateTrapped1 : StateTrapped2;
                break;
            }

            if (!target() || ((target()->maxLife() > 0.0f) && target()->dead())) {
                if (legPairIndex_ != 0) {
                    state_ = StateUp;
                }
                break;
            }

            b2Vec2 dir = target()->pos() - parent()->pos();

            if ((legPairIndex_ == 0) &&
                (fabs(angleBetween(dir, legDir_[0])) <= (b2_pi / 2.0f)) &&
                (dir.Length() <= 12.0f) &&
                (b2Cross(target()->pos() - legPos_[0], legPos_[1] - legPos_[0]) >= 0.0f)) {
                if ((attackTime_ <= 0.0f) && !patrol()) {
                    float len = (target()->pos() - parent()->getWorldPoint(bodyTip_)).Length();

                    if (len < 0.1f) {
                        len = 0.1f;
                    }

                    bodyJoint_ = scene()->addDistanceJoint(dummyObj_, parent()->shared_from_this(),
                        dummyObj_->getLocalPoint(target()->pos()), bodyTip_, len,
                        jointFrequency, jointDampingRatio, false);

                    state_ = StateAttack;

                    ac_->startAnimation(AnimationAttack);

                    attackTime_ = 0.5f;
                }
            } else if ((fabs(angleBetween(dir, -legDir_[0])) < (b2_pi / 3.0f)) &&
                (dir.Length() >= 12.0f) && !patrol() && canRotate_) {
                state_ = StateRotate;
            } else {
                state_ = StateUp;
            }

            break;
        }
        case StateUp: {
            state_ = StateDown;

            if ((legPairIndex_ == 0) && (!target() || ((target()->maxLife() > 0.0f) && target()->dead()))) {
                break;
            }

            b2AABB aabb;

            b2Vec2 tmp = parent()->pos() + parent()->getDirection(7.0f);

            aabb.lowerBound = tmp - b2Vec2(5.0f, 5.0f);
            aabb.upperBound = tmp + b2Vec2(5.0f, 5.0f);

            downSlow_ = false;

            scene()->queryAABB(aabb,
                boost::bind(&SpiderComponent::queryAABBCb, this, _1,
                    boost::ref(downSlow_)));

            bool res = legMove(legPair_[legPairIndex_].first, 4.0f);
            res &= legMove(legPair_[legPairIndex_].second, 4.0f);
            legPairIndex_ = (legPairIndex_ + 1) % 4;

            if (!res) {
                state_ = StateRotate;
            }

            break;
        }
        case StateRotate: {
            for (int i = 0; i < 8; ++i) {
                if (legJoint_[i]) {
                    legJoint_[i]->remove();
                    legJoint_[i].reset();
                }
            }

            b2Swap(legDir_[0], legDir_[3]);
            b2Swap(legDir_[1], legDir_[2]);

            for (int i = 0; i < 4; ++i) {
                legDir_[i] = -legDir_[i];
            }

            b2Swap(legPos_[0], legPos_[7]);
            b2Swap(legPos_[1], legPos_[6]);

            b2Swap(legPos_[2], legPos_[5]);
            b2Swap(legPos_[3], legPos_[4]);

            parent()->setAngleRecursive(parent()->angle() + b2_pi);

            for (int i = 0; i < 8; ++i) {
                bool res = legMove(i, 4.0f);
                (void)res;
                assert(res);
            }

            legPairIndex_ = 0;

            state_ = StateDown;

            break;
        }
        case StateAttack: {
            float len = bodyJoint_->length();

            len -= dt * settings.spider.attackVelocity;

            if (len <= 0.1f) {
                len = 0.1f;
            }

            bodyJoint_->setLength(len);

            if (((bodyJoint_->getAnchorA() - bodyJoint_->getAnchorB()).Length() <= 0.5f) || (attackTime_ <= 0.0f)) {
                sndBite_->play();

                bodyJoint_->remove();

                state_ = StateDown;

                attackTime_ = cornered_ ? 0.2f : settings.spider.attackTimeout;

                if ((target()->pos() - parent()->getWorldPoint(bodyTip_)).Length() <= 2.0f) {
                    target()->changeLife(cornered_ ? -target()->life() : -settings.spider.attackDamage);
                }
            }

            break;
        }
        case StateAngry: {
            float value = tweening_->getValue(tweenTime_);

            parent()->applyAngularImpulse(parent()->inertia() * 7.0f * (value + angryStartAngle_ - parent()->angle()), true);

            tweenTime_ += dt;
            if (tweenTime_ >= 2.0f) {
                tweening_.reset();
                state_ = StateDown;
            }

            break;
        }
        case StateTrapped1: {
            if (attackTime_ <= 0.0f) {
                attackTime_ = 0.25f;
                state_ = StateTrapped2;
                break;
            }
            parent()->applyForceToCenter(parent()->getDirection(1000.0f), true);
            break;
        }
        case StateTrapped2: {
            if (attackTime_ <= 0.0f) {
                attackTime_ = 0.25f;
                state_ = StateTrapped1;
                break;
            }
            parent()->applyForceToCenter(parent()->getDirection(-500.0f), true);
            break;
        }
        default:
            assert(0);
            break;
        }
    }

    void SpiderComponent::debugDraw()
    {
        static const Color colors[] = {
            Color(1.0f, 1.0f, 1.0f),
            Color(1.0f, 0.0f, 0.0f),
            Color(0.0f, 1.0f, 0.0f),
            Color(0.0f, 0.0f, 1.0f)
        };

        renderer.setProgramColorLines(2.0f);

        for (int i = 0; i < 8; i += 2) {
            RenderLineStrip rop = renderer.renderLineStrip();
            rop.addVertex(legPos_[i].x, legPos_[i].y);
            rop.addVertex(legPos_[i + 1].x, legPos_[i + 1].y);
            rop.addColors(colors[i / 2]);
        }
    }

    void SpiderComponent::rotate()
    {
        state_ = StateRotate;
    }

    void SpiderComponent::angry()
    {
        sndAngry_->play();
        state_ = StateAngry;

        angryStartAngle_ = parent()->angle();

        tweening_ = boost::make_shared<SingleTweening>(4.0f,
            VibrateFactor / 4.0f, EaseOutVibrate,
            0.0f, deg2rad(60.0f));
        tweenTime_ = 0.0f;
    }

    void SpiderComponent::trapped()
    {
        trapped_ = true;
    }

    void SpiderComponent::onRegister()
    {
        dummyObj_ = sceneObjectFactory.createDummy();
        scene()->addObject(dummyObj_);

        legPos_[0] = leg_[4]->getWorldPoint(b2Vec2(-4.0f, 0.0f));
        legPos_[1] = leg_[5]->getWorldPoint(b2Vec2(-4.0f, 0.0f));
        legDir_[0] = (legPos_[1] - legPos_[0]).Skew();
        legDir_[0].Normalize();
    }

    void SpiderComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
        for (int i = 0; i < 8; ++i) {
            leg_[i].reset();
        }

        dummyObj_->removeFromParent();
        dummyObj_.reset();
    }

    bool SpiderComponent::legMove(int index, float distance)
    {
        if (index == 0) {
            static const float seekAngle = deg2rad(5);
            static const float rayLen = 12.0f;

            b2Vec2 dir = b2Mul(b2Rot(b2_pi / 4.0f), legDir_[index / 2]);

            b2Rot rot(-seekAngle);

            b2Vec2 p1 = 0.5f * (legPos_[index] + legPos_[index + 1]);

            b2Vec2 finalDir = b2Vec2_zero;

            float targetAngle = 4.0f * b2_pi;

            if (target()) {
                for (float a = seekAngle; a < ((b2_pi / 2.0f) - seekAngle); a += seekAngle) {
                    dir = b2Mul(rot, dir);

                    b2Vec2 p2 = p1 + rayLen * dir;
                    bool found = false;

                    scene()->rayCast(p1, p2,
                        boost::bind(&SpiderComponent::rayCastCb, this, _1, _2, _3, _4,
                            boost::ref(found), boost::ref(p2)));

                    float angle = fabs(angleBetween(target()->pos() - p1, dir));

                    if (!found && (angle < targetAngle)) {
                        targetAngle = angle;
                        finalDir = dir;
                    }
                }
            }

            cornered_ = false;

            if (finalDir == b2Vec2_zero) {
                b2Vec2 p2 = p1 + rayLen * legDir_[index / 2];

                bool found = false;

                scene()->rayCast(p1, p2,
                    boost::bind(&SpiderComponent::rayCastCb, this, _1, _2, _3, _4,
                        boost::ref(found), boost::ref(p2)));

                float len = (p2 - p1).Length();

                if (len >= 0.4f) {
                    distance = b2Min(len - 0.1f, distance);

                    cornered_ = true;
                } else {
                    return false;
                }
            }

            for (int i = 3; i > 0; --i) {
                legPos_[i * 2 + 0] = legPos_[(i - 1) * 2 + 0];
                legPos_[i * 2 + 1] = legPos_[(i - 1) * 2 + 1];
                legDir_[i] = legDir_[i - 1];
            }

            legPos_[6] = 0.5f * (legPos_[4] + legPos_[6]);
            legPos_[7] = 0.5f * (legPos_[5] + legPos_[7]);
            legDir_[3] = 0.5f * (legDir_[2] + legDir_[3]);

            if (finalDir == b2Vec2_zero) {
                p1 += distance * legDir_[index / 2];

                findLegPos(p1, legDir_[index / 2].Skew(), legPos_[index], legPos_[index + 1]);
            } else {
                finalDir.Normalize();

                p1 += distance * finalDir;

                legDir_[index / 2] = findBestDir(p1, finalDir, legPos_[index], legPos_[index + 1]);
            }
        }

        float len = (leg_[index]->getWorldPoint(legTip_[index]) - legPos_[index]).Length();

        if (len < 0.1f) {
            len = 0.1f;
        }

        if (legJoint_[index]) {
            legJoint_[index]->remove();
        }

        legJoint_[index] = scene()->addDistanceJoint(dummyObj_, leg_[index],
            dummyObj_->getLocalPoint(legPos_[index]), legTip_[index], len,
            jointFrequency, jointDampingRatio, false);

        return true;
    }

    bool SpiderComponent::legDown(int index, float dt)
    {
        float len = legJoint_[index]->length();

        len -= dt * (downSlow_ ? (0.5f * legVelocity_) : legVelocity_);

        if (len <= 0.1f) {
            legJoint_[index]->setLength(0.1f);
            return true;
        }

        legJoint_[index]->setLength(len);

        return false;
    }

    float SpiderComponent::rayCastCb(b2Fixture* fixture, const b2Vec2& point,
        const b2Vec2& normal, float fraction, bool& found, b2Vec2& finalPoint)
    {
        if (fixture->IsSensor()) {
            return -1;
        }

        if ((SceneObject::fromFixture(fixture)->type() != SceneObjectTypeRock) &&
            (SceneObject::fromFixture(fixture)->type() != SceneObjectTypeTerrain) &&
            (SceneObject::fromFixture(fixture)->type() != SceneObjectTypeBlocker)) {
            return -1;
        }

        if ((SceneObject::fromFixture(fixture)->type() == SceneObjectTypeRock) &&
            (SceneObject::fromFixture(fixture)->bodyDef().type == b2_kinematicBody)) {
            /*
             * Hack: don't raycast against trains.
             */
            return -1;
        }

        PhysicsBodyComponentPtr pc = SceneObject::fromFixture(fixture)->findComponent<PhysicsBodyComponent>();
        if (pc && ((pc->filterCategoryBits() & collisionBitRock) != 0)) {
            /*
             * Hack: don't raycast against rocks.
             */
            return -1;
        }

        found = true;
        finalPoint = point;

        return fraction;
    }

    b2Vec2 SpiderComponent::findBestDir(const b2Vec2& p1, const b2Vec2& origDir,
        b2Vec2& bp1, b2Vec2& bp2)
    {
        static const float seekAngle = deg2rad(2);

        b2Vec2 bDir;

        b2Rot rot(-seekAngle);

        b2Vec2 dir = b2Mul(b2Rot(b2_pi / 4.0f), origDir);

        float minLen = b2_maxFloat;

        for (float a = seekAngle; a < (b2_pi / 2.0f - seekAngle); a += seekAngle) {
            dir = b2Mul(rot, dir);

            b2Vec2 p2, p3;

            float len = findLegPos(p1, dir.Skew(), p2, p3);

            if (len < minLen) {
                minLen = len;
                bp1 = p2;
                bp2 = p3;
                bDir = dir;
            }
        }

        b2Vec2 p2, p3;

        float len = findLegPos(p1, origDir.Skew(), p2, p3);

        if (len <= (minLen + 0.3f)) {
            bp1 = p2;
            bp2 = p3;
            bDir = origDir;
        }

        return bDir;
    }

    float SpiderComponent::findLegPos(const b2Vec2& p1, const b2Vec2& dir,
        b2Vec2& bp1, b2Vec2& bp2)
    {
        static const float rayLen = 10.0f;

        bp1 = p1 + 1.0f * rayLen * dir;

        bool found;

        scene()->rayCast(p1, bp1,
            boost::bind(&SpiderComponent::rayCastCb, this, _1, _2, _3, _4,
                boost::ref(found), boost::ref(bp1)));

        bp2 = p1 - 1.0f * rayLen * dir;

        scene()->rayCast(p1, bp2,
            boost::bind(&SpiderComponent::rayCastCb, this, _1, _2, _3, _4,
                boost::ref(found), boost::ref(bp2)));

        return (bp2 - bp1).Length();
    }

    bool SpiderComponent::queryAABBCb(b2Fixture* fixture, bool& found)
    {
        SceneObject* obj = SceneObject::fromFixture(fixture);

        if ((obj != parent()) && obj->findComponent<SpiderComponent>()) {
            found = true;
            return false;
        }

        return true;
    }
}
