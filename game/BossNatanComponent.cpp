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

#include "BossNatanComponent.h"
#include "Utils.h"
#include "Const.h"
#include "Settings.h"
#include "ExplosionComponent.h"
#include "Scene.h"
#include "SceneObjectFactory.h"
#include "CameraComponent.h"
#include "FadeOutComponent.h"
#include "DizzyComponent.h"
#include "PhysicsBodyComponent.h"
#include "RenderQuadComponent.h"
#include "CollisionCancelComponent.h"
#include "RenderHealthbarComponent.h"
#include "InvulnerabilityComponent.h"
#include "AssetManager.h"
#include "Renderer.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    static const int32 circleSegments = 30;
    static const float32 circleIncrement = 2.0f * b2_pi / circleSegments;

    BossNatanComponent::BossNatanComponent(const AnimationComponentPtr& ac,
        const ParticleEffectComponentPtr pec[2],
        const b2Vec2& meleePos, float meleeRadius)
    : TargetableComponent(phaseThink),
      ac_(ac),
      meleePos_(meleePos),
      meleeRadius_(meleeRadius),
      state_(StateIdle),
      t_(0.0f),
      rotateT_(0.0f),
      rotateDir_((rand() % 2) == 0),
      syringeT_(0.0f),
      dieRot_(0.0f),
      dieT_(0.0f)
    {
        pec_[0] = pec[0];
        pec_[1] = pec[1];

        sndTeleOut_ = audio.createSound("natan_tele_out.ogg");
        sndTeleIn_ = audio.createSound("natan_tele_in.ogg");
        sndShoot_[0] = audio.createSound("natan_shoot1.ogg");
        sndShoot_[1] = audio.createSound("natan_shoot2.ogg");
        sndRam_ = audio.createSound("natan_ram.ogg");
        sndPowerup_ = audio.createSound("natan_powerup.ogg");
        sndMelee_[0] = audio.createSound("natan_melee1.ogg");
        sndMelee_[1] = audio.createSound("natan_melee2.ogg");
        sndSyringePain_ = audio.createSound("natan_syringe_pain.ogg");
        sndFinishPowerup_ = audio.createSound("natan_finish_powerup.ogg");
    }

    BossNatanComponent::~BossNatanComponent()
    {
    }

    void BossNatanComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void BossNatanComponent::update(float dt)
    {
        if (!interceptBehavior_) {
            interceptBehavior_ = parent()->interceptBehavior();
        }

        if (!avoidBehavior_) {
            avoidBehavior_ = parent()->avoidBehavior();
        }

        if (!detourBehavior_) {
            detourBehavior_ = parent()->detourBehavior();
        }

        t_ -= dt;
        syringeT_ -= dt;

        updateAutoTarget(dt);

        if (ramPec_ && (!ramPec_->isAllowCompletion() || !ramPec_->isComplete())) {
            const ParticleEmmiter::Particles& particles = ramPec_->emmiters()[0]->particles();

            bool hit = false;

            for (size_t i = 0; i < particles.size(); ++i) {
                if (!particles[i].active || (particles[i].currentLife <= 400)) {
                    continue;
                }

                if (b2DistanceSquared(b2Mul(particles[i].imageStartXf, particles[i].imagePos), target()->pos()) < 4.8f * 4.8f) {
                    hit = true;
                    break;
                }
            }

            if (hit) {
                target()->changeLife(-settings.bossNatan.ramDamage * dt);
            }
        }

        bool inSight = false;
        bool detouring = false;

        if (target() && (state_ != StateIdle) && (state_ != StatePreRam) && (state_ != StateRam) &&
            (state_ != StatePostRam) && (state_ != StatePowerup2) && (state_ != StatePowerup3) &&
            (state_ != StateDie) && (state_ != StateDead)) {
            SceneObjectPtr targetObj;
            float vel;

            if (state_ == StatePowerup) {
                targetObj = powerupObj_;
                vel = 13.0f;
                if (b2DistanceSquared(parent()->pos(), targetObj->pos()) <= 5.0f * 5.0f) {
                    vel = 3.0f;
                }
            } else {
                targetObj = target();
                vel = 10.0f;
            }

            detouring = !detourBehavior_->finished();

            if (!detouring && (b2DistanceSquared(parent()->pos(), targetObj->pos()) > 8.0f * 8.0f)) {
                detourBehavior_->start();
            }

            if (detouring) {
                interceptBehavior_->setTarget(SceneObjectPtr());
            } else {
                interceptBehavior_->setTarget(targetObj);
            }

            b2Vec2 v = b2Vec2_zero;

            inSight = scene()->camera()->findComponent<CameraComponent>()->rectVisible(
                parent()->pos(), 1.0f, 1.0f);

            if (!detouring && inSight && (state_ != StatePowerup)) {
                rotateT_ -= dt;

                if (rotateT_ <= 0.0f) {
                    rotateDir_ = (rand() % 2) == 0;
                    rotateT_ = 1.0f;
                }

                float ang = b2_pi * dt / 2.0f;

                if (rotateDir_) {
                    ang = -ang;
                }

                v = target()->pos() - parent()->pos();
                v = b2Mul(b2Rot(ang), v) - v;
                v.Normalize();
            } else {
                rotateT_ = 0.0f;
            }

            if (parent()->linearVelocity().Length() < vel) {
                if (state_ != StateShoot) {
                    v += parent()->getDirection(1.0f);
                    v.Normalize();
                }

                parent()->applyForceToCenter(parent()->mass() *
                    (vel * 2.0f + parent()->linearVelocityDamped()) * v, true);
            }
        }

        switch (state_) {
        case StateIdle: {
            if (!target()) {
                break;
            }

            interceptBehavior_->reset();
            interceptBehavior_->setAngularVelocity(deg2rad(360.0f));
            interceptBehavior_->setTarget(target());
            interceptBehavior_->start();

            avoidBehavior_->reset();
            avoidBehavior_->setTypes(SceneObjectTypeEnemy);
            avoidBehavior_->setRadius(6.0f);
            avoidBehavior_->setLinearVelocity(10.0f);
            avoidBehavior_->setTimeout(0.05f);
            avoidBehavior_->setLoop(true);
            avoidBehavior_->start();

            detourBehavior_->reset();
            detourBehavior_->setAheadDistance(8.0f);
            detourBehavior_->setAngularVelocity(deg2rad(360.0f));
            detourBehavior_->setTimeout(0.05f);
            detourBehavior_->setRayDistance(1.0f);
            detourBehavior_->start();

            startWalk();
            break;
        }
        case StateWalk: {
            if (!target()) {
                interceptBehavior_->reset();
                avoidBehavior_->reset();
                detourBehavior_->reset();

                state_ = StateIdle;
                break;
            }

            if (checkPowerup()) {
                parent()->addComponent(sceneObjectFactory.createInvulnerability(5.5f,
                    -1.0f, zOrderEffects + 5));

                state_ = StatePowerup;
                sndPowerup_->play();
                break;
            }

            if (parent()->dead()) {
                interceptBehavior_->reset();
                avoidBehavior_->reset();
                detourBehavior_->reset();

                setupHealth(false);

                attackDone_[0] = false;
                ac_->startAnimation(AnimationDie);
                state_ = StateDie;
                t_ = 3.5f;
                dieT_ = 0.7f;
                break;
            }

            if (b2DistanceSquared(parent()->pos(), target()->pos()) <= 7.0f * 7.0f) {
                float angle = fabs(angleBetween(target()->pos() - parent()->pos(), parent()->getDirection(1.0f)));
                if (angle < deg2rad(45.0f)) {
                    if ((syringeT_ <= 0.0f) && ((rand() % 2) == 0)) {
                        ac_->startAnimation(AnimationSyringe);
                        state_ = StateSyringe;
                        attackDone_[0] = false;
                    } else {
                        ac_->startAnimation(AnimationMelee);
                        state_ = StateMelee;
                        attackDone_[0] = false;
                        attackDone_[1] = false;
                    }
                    break;
                }
            }
            if (inSight) {
                if (t_ <= 0.0f) {
                    int r = rand() % 7;

                    if ((r >= 3) && (r <= 5) &&
                        (detouring ||
                        (fabs(angleBetween(target()->pos() - parent()->pos(), parent()->getDirection(1.0f))) > deg2rad(25.0f)))) {
                        r = (((rand() % 3) == 0) ? 6 : 0);
                    }

                    if ((r >= 0) && (r <= 2)) {
                        sndShoot_[rand() % 2]->play();
                        ac_->startAnimation(AnimationPreShoot);
                        weaponIndex_ = rand() % 3;
                        shootNum_ = 1 + rand() % 3;
                        state_ = StatePreShoot;
                        t_ = 0.0f;
                    } else if ((r >= 3) && (r <= 5)) {
                        RenderQuadComponentPtr rc = parent()->findComponent<RenderQuadComponent>();

                        detourBehavior_->reset();
                        avoidBehavior_->reset();
                        interceptBehavior_->setTarget(target());

                        if (ramPec_) {
                            ramPec_->removeFromParent();
                        }
                        ramPec_ = assetManager.getParticleEffect("ram.p", rc->pos(), 0.0f);
                        ramPec_->setZOrder(rc->zOrder() - 2);
                        ramPec_->resetEmit();
                        parent()->addComponent(ramPec_);

                        attackDone_[0] = false;
                        ac_->startAnimation(AnimationRam);

                        state_ = StatePreRam;
                    } else {
                        startTeleport();
                    }
                    break;
                }
            } else if (t_ <= 0.0f) {
                if ((rand() % 3) == 0) {
                    startTeleport();
                } else {
                    startWalk();
                }
                break;
            }
            break;
        }
        case StateMelee: {
            if (!attackDone_[0] && (ac_->animationFrameIndex() >= ac_->animationSpecialIndex(0))) {
                if (target() && (b2DistanceSquared(parent()->getWorldPoint(meleePos_), target()->pos()) <= meleeRadius_ * meleeRadius_)) {
                    target()->changeLife(-settings.bossNatan.meleeDamage);
                }
                attackDone_[0] = true;
                sndMelee_[rand() % 2]->play();
            }

            if (!attackDone_[1] && (ac_->animationFrameIndex() >= ac_->animationSpecialIndex(1))) {
                if (target() && (b2DistanceSquared(parent()->getWorldPoint(meleePos_), target()->pos()) <= meleeRadius_ * meleeRadius_)) {
                    target()->changeLife(-settings.bossNatan.meleeDamage);
                }
                attackDone_[1] = true;
                sndMelee_[rand() % 2]->play();
            }

            if (!ac_->animationFinished()) {
                break;
            }

            ac_->startAnimation(AnimationDefault);
            startWalk();
            break;
        }
        case StateSyringe: {
            if (!attackDone_[0] && (ac_->animationFrameIndex() >= ac_->animationSpecialIndex(0))) {
                sndMelee_[rand() % 2]->play();
                if (target()->alive() && (b2DistanceSquared(parent()->getWorldPoint(meleePos_), target()->pos()) <= meleeRadius_ * meleeRadius_)) {
                    target()->changeLife(-settings.bossNatan.syringeDamage);
                    scene()->camera()->addComponent(boost::make_shared<DizzyComponent>(target(), 8.0f, 2.0f));
                    syringeT_ = 11.0f;
                    sndSyringePain_->play();
                }
                attackDone_[0] = true;
            }

            if (!ac_->animationFinished()) {
                break;
            }

            ac_->startAnimation(AnimationDefault);
            startWalk();
            break;
        }
        case StatePreTeleport: {
            float a = -t_ / 0.8f;
            if (a > 1.0f) {
                a = 1.0f;
            }
            float v = 1.0f * (1.0f - a) + 0.0f * a;
            parent()->findComponent<RenderQuadComponent>()->setColor(
                Color(1.0f, 1.0f, 1.0f, v));
            for (int i = 0; i < 2; ++i) {
                for (ParticleEffectComponent::Emmiters::const_iterator it = pec_[i]->emmiters().begin();
                    it != pec_[i]->emmiters().end(); ++it) {
                    (*it)->entry().transparency.setHigh(v);
                }
            }
            if (t_ <= -(0.8f + 1.0f)) {
                state_ = StateTeleport;
                t_ = 0.8f;
                float minDist = b2_maxFloat;
                b2Vec2 pos = parent()->pos();
                for (Points::const_iterator it = teleportDests_.begin();
                    it != teleportDests_.end(); ++it) {
                    float lenSq = b2DistanceSquared(*it, target()->pos());
                    if (lenSq < minDist) {
                        pos = *it;
                        minDist = lenSq;
                    }
                }
                parent()->setPos(pos);
                parent()->setAngle(vec2angle(target()->pos() - parent()->pos()));
                break;
            }
            break;
        }
        case StateTeleport: {
            if (t_ <= 0.0f) {
                t_ = 0.0f;
                state_ = StatePostTeleport;
                sndTeleIn_->play();
                break;
            }
            break;
        }
        case StatePostTeleport: {
            float a = -t_ / 0.8f;
            if (a > 1.0f) {
                a = 1.0f;
            }
            float v = 0.0f * (1.0f - a) + 1.0f * a;
            parent()->findComponent<RenderQuadComponent>()->setColor(
                Color(1.0f, 1.0f, 1.0f, v));
            for (int i = 0; i < 2; ++i) {
                for (ParticleEffectComponent::Emmiters::const_iterator it = pec_[i]->emmiters().begin();
                    it != pec_[i]->emmiters().end(); ++it) {
                    (*it)->entry().transparency.setHigh(v);
                }
            }
            if (a == 1.0f) {
                setupHealth(true);
                PhysicsBodyComponentPtr pc = parent()->findComponent<PhysicsBodyComponent>();
                pc->setFilterGroupIndex(0);
                pc->setFilterMaskBits(filterMaskBits_);
                parent()->setType(SceneObjectTypeEnemy);
                startWalk();
            }
            break;
        }
        case StatePreShoot: {
            if (ac_->animationFinished() && (t_ <= 0.0f)) {
                if ((b2DistanceSquared(parent()->pos(), target()->pos()) <= 7.0f * 7.0f) || checkPowerup() || parent()->dead()) {
                    ac_->startAnimation(AnimationPostShoot);
                    state_ = StatePostShoot;
                    break;
                }
                state_ = StateShoot;
                t_ = settings.bossNatan.shootDuration;
                weapon_[weaponIndex_]->trigger(true);
                ac_->startAnimation(AnimationShoot);
            }
            break;
        }
        case StateShoot: {
            if ((b2DistanceSquared(parent()->pos(), target()->pos()) <= 7.0f * 7.0f) || checkPowerup() || parent()->dead()) {
                weapon_[weaponIndex_]->trigger(false);
                ac_->startAnimation(AnimationPostShoot);
                state_ = StatePostShoot;
                break;
            }
            if (t_ <= 0.0f) {
                --shootNum_;
                weapon_[weaponIndex_]->trigger(false);
                if (shootNum_ <= 0) {
                    ac_->startAnimation(AnimationPostShoot);
                    state_ = StatePostShoot;
                } else {
                    state_ = StatePreShoot;
                    t_ = settings.bossNatan.shootDelay;
                    ac_->startAnimation(AnimationPreShoot2);
                }
                break;
            }
            break;
        }
        case StatePostShoot: {
            if (ac_->animationFinished()) {
                ac_->startAnimation(AnimationDefault);
                startWalk();
            }
            break;
        }
        case StatePreRam: {
            if (ac_->animationFrameIndex() >= ac_->animationSpecialIndex(0)) {
                SceneObjectTypes types;

                types.set(SceneObjectTypePlayer);

                CollisionCancelComponentPtr c = boost::make_shared<CollisionCancelComponent>();
                c->setFilter(types);
                c->setDamage(settings.bossNatan.ramHitDamage);
                c->setDamageSound(audio.createSound("player_hit.ogg"));
                c->setRoll(deg2rad(2.0f));
                parent()->addComponent(c);

                interceptBehavior_->reset();
                state_ = StateRam;
                t_ = 1.0f;
                sndRam_->play();
                break;
            }

            parent()->applyForceToCenter(parent()->mass() *
                -parent()->getDirection(25.0f * 2.0f + parent()->linearVelocityDamped()), true);

            break;
        }
        case StateRam: {
            if (!attackDone_[0] && (ac_->animationFrameIndex() >= ac_->animationSpecialIndex(1))) {
                ac_->setPaused(true);
                attackDone_[0] = true;
            }
            if (t_ <= 0.0f) {
                ac_->setPaused(false);
                state_ = StatePostRam;
                t_ = 0.9f;
                break;
            }
            parent()->applyForceToCenter(parent()->mass() *
                parent()->getDirection(25.0f * 4.0f + parent()->linearVelocityDamped()), true);
            break;
        }
        case StatePostRam: {
            if (t_ <= 0.0f) {
                parent()->findComponent<CollisionCancelComponent>()->removeFromParent();
                ramPec_->allowCompletion();
                ac_->startAnimation(AnimationDefault);
                state_ = StateIdle;
            }
            break;
        }
        case StatePowerup: {
            if (b2DistanceSquared(parent()->pos(), powerupObj_->pos()) <= 1.0f * 1.0f) {
                detourBehavior_->reset();
                avoidBehavior_->reset();
                interceptBehavior_->reset();
                parent()->setLinearVelocity(b2Vec2_zero);
                parent()->setAngularVelocity(0.0f);
                parent()->body()->SetType(b2_kinematicBody);
                state_ = StatePowerup2;
                break;
            }
            break;
        }
        case StatePowerup2: {
            break;
        }
        case StatePowerup3: {
            if (t_ <= 0.0f) {
                b2Rot rot(2.0f * b2_pi / settings.bossNatan.napalmNum);
                b2Vec2 dir = angle2vec(getRandom(0.0f, b2_pi * 2.0f), 4.0f);

                for (UInt32 i = 0; i < settings.bossNatan.napalmNum; ++i) {
                    SceneObjectPtr napalm = sceneObjectFactory.createNapalm2(settings.bossNatan.napalmDamage, i == 0);
                    napalm->setTransform(b2Mul(parent()->getTransform(), b2Transform(dir, b2Rot(vec2angle(dir)))));
                    napalm->setLinearVelocity(napalm->getDirection(35.0f));
                    scene()->addObject(napalm);
                    dir = b2Mul(rot, dir);
                }

                t_ = settings.bossNatan.napalmInterval;
                break;
            }
            break;
        }
        case StateDie: {
            if (t_ <= 0.0f) {
                if (!attackDone_[0] && (ac_->animationFrameIndex() < ac_->animationSpecialIndex(0))) {
                    attackDone_[0] = true;
                }
                if (attackDone_[0] && (ac_->animationFrameIndex() >= ac_->animationSpecialIndex(1))) {
                    scene()->stats()->incEnemiesKilled();

                    state_ = StateDead;
                    ac_->startAnimation(AnimationDead);
                    t_ = 1.0f;
                    break;
                }
            }

            dieT_ -= dt;

            parent()->applyTorque(parent()->inertia() * (deg2rad(360.0f) - parent()->angularVelocity() + parent()->angularVelocityDamped()), true);

            b2Vec2 v = b2Mul(dieRot_, b2Vec2(1.0f, 0.0f));

            if (parent()->linearVelocity().Length() < 15.0f) {
                parent()->applyForceToCenter(parent()->mass() *
                    (15.0f * 2.0f + parent()->linearVelocityDamped()) * v, true);
            }

            dieRot_ = b2Mul(dieRot_, b2Rot(deg2rad(180.0f) * dt));

            if (dieT_ <= 0.0f) {
                SceneObjectPtr explosion = sceneObjectFactory.createExplosion1(zOrderExplosion);

                explosion->setTransform(parent()->getTransform());

                scene()->addObject(explosion);

                SceneObjectPtr bs = sceneObjectFactory.createBloodStain2(getRandom(6.0f, 9.0f), zOrderBack);

                bs->setPos(parent()->pos());
                bs->setAngle(getRandom(0.0f, 2.0f * b2_pi));

                scene()->addObject(bs);

                dieT_ = 0.4f;
            }

            break;
        }
        case StateDead: {
            if ((t_ <= 0.0f) && (parent()->body()->GetType() != b2_kinematicBody)) {
                parent()->setLinearVelocity(b2Vec2_zero);
                parent()->setAngularVelocity(0.0f);
                parent()->body()->SetType(b2_kinematicBody);
                pec_[0]->removeFromParent();
                pec_[1]->removeFromParent();
            }
            break;
        }
        default:
            assert(false);
            break;
        }
    }

    void BossNatanComponent::debugDraw()
    {
        if (ramPec_ && (!ramPec_->isAllowCompletion() || !ramPec_->isComplete())) {
            const ParticleEmmiter::Particles& particles = ramPec_->emmiters()[0]->particles();

            for (size_t i = 0; i < particles.size(); ++i) {
                if (!particles[i].active || (particles[i].currentLife <= 400)) {
                    continue;
                }

                b2Vec2 center = b2Mul(particles[i].imageStartXf, particles[i].imagePos);
                float radius = 4.8f;

                float32 theta = 0.0f;
                b2Vec2 p0;

                renderer.setProgramColorLines(1.0f);

                RenderLineStrip rop = renderer.renderLineStrip();

                for (int32 i = 0; i < circleSegments; ++i) {
                    b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));

                    rop.addVertex(v.x, v.y);

                    if (i == 0) {
                        p0 = v;
                    }

                    theta += circleIncrement;
                }

                rop.addVertex(p0.x, p0.y);

                rop.addColors(Color(1.0f, 1.0f, 1.0f));
            }
        }
    }

    void BossNatanComponent::onRegister()
    {
        scene()->stats()->incEnemiesSpawned();

        parent()->body()->SetType(b2_kinematicBody);
        parent()->setInvulnerable(true);
    }

    void BossNatanComponent::onUnregister()
    {
        setTarget(SceneObjectPtr());
        powerupObj_.reset();
    }

    void BossNatanComponent::startWalk()
    {
        state_ = StateWalk;
        t_ = getRandom(settings.bossNatan.attackMinTimeout, settings.bossNatan.attackMaxTimeout);
    }

    void BossNatanComponent::startTeleport()
    {
        setupHealth(false);
        state_ = StatePreTeleport;
        PhysicsBodyComponentPtr pc = parent()->findComponent<PhysicsBodyComponent>();
        pc->setFilterGroupIndex(collisionGroupMissile);
        filterMaskBits_ = pc->filterMaskBits();
        pc->setFilterMaskBits(collisionBitGeneral);
        parent()->setType(SceneObjectTypeDeadbody);
        t_ = 0.0f;
        sndTeleOut_->play();
    }

    void BossNatanComponent::setupHealth(bool vulnerable)
    {
        RenderHealthbarComponentPtr hbc = parent()->findComponent<RenderHealthbarComponent>();

        if (vulnerable) {
            parent()->setInvulnerable(false);

            if (!hbc) {
                hbc = boost::make_shared<RenderHealthbarComponent>(b2Vec2(0.0f, 5.0f), 0, 8.0f, 1.2f, zOrderMarker + 1);

                hbc->setColor(Color(1.0f, 1.0f, 1.0f, 0.6f));

                parent()->addComponent(hbc);
            }
        } else {
            parent()->setInvulnerable(true);
            if (hbc) {
                hbc->removeFromParent();
            }
        }
    }

    bool BossNatanComponent::checkPowerup()
    {
        if (!powerupObj_) {
            return false;
        }

        if (parent()->lifePercent() >= (2.0f / 3.0f)) {
            return false;
        }

        return true;
    }

    void BossNatanComponent::setupInitial()
    {
        parent()->body()->SetType(b2_dynamicBody);
        setupHealth(true);
    }

    void BossNatanComponent::startNapalm()
    {
        state_ = StatePowerup3;
        t_ = 0.0f;
    }

    void BossNatanComponent::finishNapalm()
    {
        state_ = StatePowerup2;
    }

    void BossNatanComponent::finishPowerup()
    {
        InvulnerabilityComponentPtr c = parent()->findComponent<InvulnerabilityComponent>();
        c->setDuration(0.5f);
        parent()->body()->SetType(b2_dynamicBody);
        state_ = StateIdle;
        sndFinishPowerup_->play();
    }

    void BossNatanComponent::startPowerupAnimation()
    {
        ac_->startAnimation(AnimationPowerup);
    }

    void BossNatanComponent::finishPowerupAnimation()
    {
        ac_->startAnimation(AnimationDefault);
    }
}
