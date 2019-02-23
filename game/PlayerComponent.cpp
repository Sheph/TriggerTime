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

#include "PlayerComponent.h"
#include "ExplosionComponent.h"
#include "Settings.h"
#include "InputManager.h"
#include "Scene.h"
#include "SceneObject.h"
#include "SceneObjectFactory.h"
#include "CollisionDieComponent.h"
#include "WeaponBlasterComponent.h"
#include "WeaponShotgunComponent.h"
#include "WeaponGGComponent.h"
#include "WeaponRLauncherComponent.h"
#include "WeaponProxMineComponent.h"
#include "WeaponChainsawComponent.h"
#include "WeaponLGunComponent.h"
#include "WeaponPlasmaGunComponent.h"
#include "WeaponMachineGunComponent.h"
#include "WeaponEShieldComponent.h"
#include "PhysicsBodyComponent.h"
#include "Const.h"
#include "Utils.h"
#include "AssetManager.h"
#include "CameraComponent.h"
#include "SingleTweening.h"
#include <boost/make_shared.hpp>

namespace af
{
    PlayerComponent::PlayerComponent(const b2Vec2& weaponPos,
        float weaponAngle,
        float weaponFlashDistance,
        const b2Vec2& exhaustPos1,
        float exhaustAngle1,
        const b2Vec2& exhaustPos2,
        float exhaustAngle2,
        const RenderQuadComponentPtr& torsoRc,
        const AnimationComponentPtr& torsoAc,
        const RenderQuadComponentPtr& legsRc,
        const AnimationComponentPtr& legsAc,
        const LightPtr& flashlight)
    : PhasedComponent(phaseThink),
      weaponPos_(weaponPos),
      weaponAngle_(weaponAngle),
      weaponFlashDistance_(weaponFlashDistance),
      numHearts_(0),
      numGems_(0),
      prevAngVelocity_(0),
      prevHealth_(0),
      sndDeath_(audio.createSound("player_death.ogg")),
      sndSwitch_(audio.createSound("weapon_switch.ogg")),
      torsoRc_(torsoRc),
      torsoAc_(torsoAc),
      legsRc_(legsRc),
      legsAc_(legsAc),
      torsoOrigHeight_(torsoRc_->height()),
      torsoOrigPos_(torsoRc_->pos()),
      jetpack_(false),
      flashlight_(flashlight),
      earpiece_(false),
      standing_(false),
      recoilTweenTime_(0.0f),
      haveGun_(true),
      haveBackpack_(true),
      ghost_(false),
      stamina_(0.0f),
      maxStamina_(0.0f)
    {
        for (int i = 0; i < WeaponTypeMax + 1; ++i) {
            ammo_[i] = 0.0f;
        }

        exhaustPos_[0] = exhaustPos1;
        exhaustAngle_[0] = exhaustAngle1;
        exhaustPos_[1] = exhaustPos2;
        exhaustAngle_[1] = exhaustAngle2;

        primarySlot_ = WeaponComponent::getSlot(WeaponTypeBlaster);
        secondarySlot_ = WeaponComponent::getSlot(WeaponTypeGG);

        inventory_ = boost::make_shared<Inventory>();

        recoilTweening_ = boost::make_shared<SingleTweening>(0.1f, EaseOutQuad, 0.2f, 0.0f);
        recoilTweenTime_ = recoilTweening_->duration();
    }

    PlayerComponent::~PlayerComponent()
    {
    }

    void PlayerComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void PlayerComponent::update(float dt)
    {
        legsAc_->setAnimationFactor(AnimationWalk, 1.0f);

        recoilTweenTime_ += dt;
        if (!recoilTweening_->finished(recoilTweenTime_)) {
            float v = recoilTweening_->getValue(recoilTweenTime_);
            torsoRc_->setPos(torsoOrigPos_ + b2Vec2(-v, 0.0f));
        } else {
            torsoRc_->setPos(torsoOrigPos_);
        }

        if (parent()->life() < prevHealth_) {
            float value = (prevHealth_ - parent()->life()) / parent()->maxLife();
            if (value <= (0.3f)) {
                value *= (1.0f / 0.3f);
            } else {
                value = 1.0f;
            }
            scene()->damage()->setDamaged(value);
            flags_[FlagDamaged] = true;
        }

        prevHealth_ = parent()->life();

        if (parent()->life() <= 0) {
            SceneObjectPtr explosion = sceneObjectFactory.createExplosion1(zOrderExplosion);

            explosion->setTransform(parent()->getTransform());

            explosion->findComponent<ExplosionComponent>()->setBlast(parent()->shared_from_this(),
                settings.player.explosionImpulse, settings.player.explosionDamage,
                SceneObjectTypeEnemy);

            scene()->addObject(explosion);

            sndDeath_->play();

            scene()->damage()->setDamaged(1.0f);

            flags_[FlagLifeLost] = true;

            parent()->removeFromParent();

            return;
        }

        if (jetpack_ && !exhaust_[0]) {
            exhaust_[0] = assetManager.getParticleEffect("exhaust.p",
                exhaustPos_[0], exhaustAngle_[0]);

            exhaust_[0]->setZOrder(legsRc_->zOrder() - 1);

            exhaust_[0]->resetEmit();

            parent()->addComponent(exhaust_[0]);

            exhaust_[1] = assetManager.getParticleEffect("exhaust.p",
                exhaustPos_[1], exhaustAngle_[1]);

            exhaust_[1]->setZOrder(legsRc_->zOrder() - 1);

            exhaust_[1]->resetEmit();

            parent()->addComponent(exhaust_[1]);

            torsoRc_->setHeight(torsoOrigHeight_ * 1.15f);
        } else if (jetpack_ && exhaust_[0]) {
            if (exhaust_[0]->isAllowCompletion()) {
                exhaust_[0]->removeFromParent();

                exhaust_[0] = assetManager.getParticleEffect("exhaust.p",
                    exhaustPos_[0], exhaustAngle_[0]);

                exhaust_[0]->setZOrder(legsRc_->zOrder() - 1);

                exhaust_[0]->resetEmit();

                parent()->addComponent(exhaust_[0]);

                exhaust_[1] = assetManager.getParticleEffect("exhaust.p",
                    exhaustPos_[1], exhaustAngle_[1]);

                exhaust_[1]->setZOrder(legsRc_->zOrder() - 1);

                exhaust_[1]->resetEmit();

                parent()->addComponent(exhaust_[1]);

                torsoRc_->setHeight(torsoOrigHeight_ * 1.15f);
            }
        } else if (!jetpack_ && exhaust_[0]) {
            if (!exhaust_[0]->isAllowCompletion()) {
                exhaust_[0]->allowCompletion();
                exhaust_[1]->allowCompletion();
                torsoRc_->setHeight(torsoOrigHeight_);
            }
            if (exhaust_[0]->isComplete()) {
                exhaust_[0]->removeFromParent();
                exhaust_[0].reset();
                exhaust_[1]->removeFromParent();
                exhaust_[1].reset();
            }
        }

        int defAnimation;
        int idleAnimation;
        int walkAnimation;

        if (flashlight()) {
            defAnimation = AnimationHelmetDefault;
            idleAnimation = AnimationHelmetIdle;
            walkAnimation = AnimationHelmetWalk;
        } else if (!haveGun_) {
            defAnimation = AnimationNogunDefault;
            idleAnimation = AnimationNogunIdle;
            walkAnimation = AnimationNogunWalk;
        } else if (!haveBackpack_) {
            if (weapon() && (weapon()->weaponType() == WeaponTypeChainsaw)) {
                if (weapon()->triggerHeld()) {
                    defAnimation = AnimationNobackpackChainsawAttack;
                    idleAnimation = AnimationNobackpackChainsawAttack;
                    walkAnimation = AnimationNobackpackChainsawAttack;
                } else {
                    defAnimation = AnimationNobackpackChainsawDefault;
                    idleAnimation = AnimationNobackpackChainsawIdle;
                    walkAnimation = AnimationNobackpackChainsawWalk;
                }
            } else {
                defAnimation = AnimationNobackpackDefault;
                idleAnimation = AnimationNobackpackIdle;
                walkAnimation = AnimationNobackpackWalk;
            }
        } else {
            if (weapon() && (weapon()->weaponType() == WeaponTypeChainsaw)) {
                if (weapon()->triggerHeld()) {
                    defAnimation = AnimationChainsawAttack;
                    idleAnimation = AnimationChainsawAttack;
                    walkAnimation = AnimationChainsawAttack;
                } else {
                    defAnimation = AnimationChainsawDefault;
                    idleAnimation = AnimationChainsawIdle;
                    walkAnimation = AnimationChainsawWalk;
                }
            } else {
                defAnimation = AnimationDefault;
                idleAnimation = AnimationIdle;
                walkAnimation = AnimationWalk;
            }
        }

        if ((parent()->linearVelocity().LengthSquared() >= 2.0f) && !jetpack_ && !standing_) {
            legsRc_->setAngle(vec2angle(parent()->linearVelocity()) - (b2_pi / 2.0f));

            if (recoilTweening_->finished(recoilTweenTime_)) {
                if (torsoAc_->currentAnimation() != walkAnimation) {
                    torsoAc_->startAnimation(walkAnimation);
                }
            } else {
                if (torsoAc_->currentAnimation() != defAnimation) {
                    torsoAc_->startAnimation(defAnimation);
                }
            }

            if (legsAc_->currentAnimation() != AnimationWalk) {
                legsAc_->startAnimation(AnimationWalk);
            }
        } else {
            if (legsAc_->currentAnimation() != AnimationDefault) {
                legsAc_->startAnimation(AnimationDefault);
            }
            if (torsoAc_->currentAnimation() != idleAnimation) {
                torsoAc_->startAnimation(idleAnimation);
            }
        }

        if (scene()->cutscene()) {
            changeStamina(dt * settings.player.staminaFastRecoverySpeed);
            return;
        }

        assert(parent()->linearDamping() == 0.0f);
        assert(parent()->angularDamping() == 0.0f);

        if (scene()->inputPlayer()->ghostTriggered()) {
            if (!ghost_) {
                parent()->findComponent<PhysicsBodyComponent>()->disableAllFixtures();
                torsoRc_->setColor(Color(1.0f, 1.0f, 1.0f, 0.5f));
                legsRc_->setColor(Color(1.0f, 1.0f, 1.0f, 0.5f));
            } else {
                parent()->findComponent<PhysicsBodyComponent>()->restoreFilter();
                torsoRc_->setColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
                legsRc_->setColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
            }

            ghost_ = !ghost_;
        }

        b2Vec2 moveDir = b2Vec2_zero, lookDir = b2Vec2_zero;

        bool movePressed = scene()->inputPlayer()->movePressed(moveDir);
        bool primaryPressed = scene()->inputPlayer()->primaryPressed(lookDir);
        bool secondaryPressed = scene()->inputPlayer()->secondaryPressed(lookDir);
        bool lookRelative = false;
        bool lookPressed = scene()->inputPlayer()->lookPressed(lookDir, lookRelative);
        bool runPressed = scene()->inputPlayer()->runPressed();

        if (lookPressed && !lookRelative) {
            lookDir -= scene()->camera()->findComponent<CameraComponent>()->getGamePoint(parent()->pos());
        }

        if ((lookDir == b2Vec2_zero) && !(altWeapon() && altWeapon()->aiming())) {
            lookDir = moveDir;
        }

        if (movePressed) {
            float speed = settings.player.moveSpeed;

            if (runPressed && (!weapon() || !primaryPressed)) {
                if (stamina_ > 0.0f) {
                    speed = settings.player.runSpeed;
                    legsAc_->setAnimationFactor(AnimationWalk, speed / settings.player.moveSpeed);
                }
                changeStamina(-dt * settings.player.staminaLooseSpeed);
            } else {
                changeStamina(dt * settings.player.staminaRecoverySpeed);
            }

            moveDir *= ghost_ ? 80.0f : speed;

            if (altWeapon() && altWeapon()->charged() && !lookPressed) {
                lookDir = moveDir;
                moveDir = parent()->getDirection(moveDir.Length());
            }
        } else {
            changeStamina(dt * settings.player.staminaFastRecoverySpeed);
        }

        float tmpRotateSpeed;

        if (altWeapon() && altWeapon()->charged()) {
            tmpRotateSpeed = settings.player.chargedRotateSpeed;
        } else {
            tmpRotateSpeed = b2_maxRotation / settings.physics.fixedTimestep;
        }

        float angVelocity = 0.0f;

        if ((!secondaryPressed || !(altWeapon() && altWeapon()->charged())) && (lookDir != b2Vec2_zero)) {
            b2Vec2 dir = parent()->getDirection(1.0f);

            float angle = angleBetween(dir, lookDir);

            if (angle >= 0.0f) {
                angVelocity = tmpRotateSpeed;
            } else {
                angVelocity = -tmpRotateSpeed;
            }

            if (fabs(angVelocity * settings.physics.fixedTimestep) > fabs(angle)) {
                angVelocity = angle / settings.physics.fixedTimestep;
            }
        }

        setAngVelocity(angVelocity);

        /*
         * The idea here is to find 'excess' velocity, i.e. velocity
         * that was introduced by collisions/external forces and 'damp'
         * it manually, afterwards we can add 'damped excess' velocity
         * to our move velocity. In order for this to work we must
         * have object's linear damping set to 0.0f always.
         *
         * Regarding formula, see b2Island.cpp:Solve:
         * // Apply damping.
         * // ODE: dv/dt + c * v = 0
         */

        float linearDamping = jetpack_ ? settings.player.jetpackLinearDamping :
            settings.player.walkLinearDamping;

        b2Vec2 excessVelocity = (1.0f / (1.0f + dt * linearDamping)) * (parent()->linearVelocity() - moveDir);

        parent()->applyLinearImpulse(
            parent()->mass() * (excessVelocity + moveDir - parent()->linearVelocity()),
            parent()->worldCenter(), true);

        if (weapon()) {
            weapon()->trigger(primaryPressed);
        }
        if (altWeapon()) {
            altWeapon()->trigger(secondaryPressed);
        }

        if (scene()->inputPlayer()->suicidePressed()) {
            parent()->changeLife(-parent()->life());
        }
    }

    void PlayerComponent::onRegister()
    {
        for (int i = 0; i < WeaponSlotMax + 1; ++i) {
            if (weapons_[i]) {
                parent()->addComponent(weapons_[i]);
            }
        }

        prevHealth_ = parent()->life();

        /*
         * FIXME: player.rube has zOrder 60 for exhaust particle effect, so
         * make it 61 here. Need to find a way to handle these zOrders better...
         */
        parent()->addComponent(sceneObjectFactory.createInvulnerability(2.1f,
            settings.player.shieldDuration, 61));

        legsRc_->setAngle(parent()->angle() - (b2_pi / 2));
    }

    void PlayerComponent::onUnregister()
    {
        for (int i = 0; i < WeaponSlotMax + 1; ++i) {
            if (weapons_[i]) {
                weapons_[i]->removeFromParent();
            }
        }
    }

    void PlayerComponent::setAngVelocity(float angVelocity)
    {
        float excessAngVelocity = (1.0f / (1.0f + settings.physics.fixedTimestep * settings.player.angularDamping)) *
            (parent()->angularVelocity() - prevAngVelocity_);

        parent()->applyAngularImpulse(parent()->inertia() *
            (excessAngVelocity + angVelocity - parent()->angularVelocity()),
            true);

        prevAngVelocity_ = angVelocity;
    }

    bool PlayerComponent::giveWeapon(WeaponType weaponType)
    {
        if ((weaponType == WeaponTypeBlaster) && scene() && (scene()->upgradeCount(UpgradeIdSuperBlaster) > 0.0f)) {
            weaponType = WeaponTypeSuperBlaster;
        }

        WeaponSlot slot = WeaponComponent::getSlot(weaponType);

        if (weapons_[slot] && (weaponType == weapons_[slot]->weaponType())) {
            return false;
        }

        WeaponComponentPtr weapon;

        switch (weaponType) {
        case WeaponTypeGG: {
            weapon = boost::make_shared<WeaponGGComponent>(SceneObjectTypePlayerMissile);
            break;
        }
        case WeaponTypeBlaster: {
            WeaponBlasterComponentPtr blaster =
                boost::make_shared<WeaponBlasterComponent>(false, SceneObjectTypePlayerMissile);

            blaster->setDamage(settings.player.blaster.damage);
            blaster->setVelocity(settings.player.blaster.velocity);
            blaster->setTurns(1);
            blaster->setShotsPerTurn(1);
            blaster->setLoopDelay(settings.player.blaster.interval);
            blaster->setTweakPos(true);
            blaster->setTweakOffset(weaponPos_);
            blaster->setSpreadAngle(deg2rad(4.0f));

            weapon = blaster;
            break;
        }
        case WeaponTypeShotgun: {
            WeaponShotgunComponentPtr shotgun =
                boost::make_shared<WeaponShotgunComponent>(false, SceneObjectTypePlayerMissile);

            shotgun->setDamage(settings.player.shotgun.damage);
            shotgun->setVelocity(settings.player.shotgun.velocity);
            shotgun->setSpreadAngle(settings.player.shotgun.spreadAngle);
            shotgun->setNumPellets(settings.player.shotgun.numPellets);
            shotgun->setInterval(settings.player.shotgun.interval);
            shotgun->setTweakPos(true);
            shotgun->setTweakOffset(weaponPos_);

            weapon = shotgun;
            break;
        }
        case WeaponTypeSuperBlaster: {
            WeaponBlasterComponentPtr blaster =
                boost::make_shared<WeaponBlasterComponent>(true, SceneObjectTypePlayerMissile);

            blaster->setDamage(settings.player.superBlaster.damage);
            blaster->setVelocity(settings.player.superBlaster.velocity);
            blaster->setTurns(1);
            blaster->setShotsPerTurn(3);
            blaster->setLoopDelay(settings.player.superBlaster.interval);
            blaster->setTweakPos(true);
            blaster->setTweakOffset(weaponPos_);

            weapon = blaster;
            break;
        }
        case WeaponTypeRLauncher: {
            WeaponRLauncherComponentPtr rlauncher =
                boost::make_shared<WeaponRLauncherComponent>(SceneObjectTypePlayerMissile);

            rlauncher->setExplosionImpulse(settings.player.rlauncher.explosionImpulse);
            rlauncher->setExplosionDamage(settings.player.rlauncher.explosionDamage);
            rlauncher->setVelocity(settings.player.rlauncher.velocity);
            rlauncher->setInterval(settings.player.rlauncher.interval);
            rlauncher->setAmmo(&ammo_[weaponType]);
            rlauncher->setTweakOffset(weaponPos_);

            weapon = rlauncher;
            break;
        }
        case WeaponTypeProxMine: {
            WeaponProxMineComponentPtr proxmine =
                boost::make_shared<WeaponProxMineComponent>(SceneObjectTypePlayerMissile);

            proxmine->setArmTimeout(settings.player.proxmine.armTimeout);
            proxmine->setActivationRadius(settings.player.proxmine.activationRadius);
            proxmine->setActivationTimeout(settings.player.proxmine.activationTimeout);
            proxmine->setExplosionTimeout(settings.player.proxmine.explosionTimeout);
            proxmine->setExplosionImpulse(settings.player.proxmine.explosionImpulse);
            proxmine->setExplosionDamage(settings.player.proxmine.explosionDamage);
            proxmine->setInterval(settings.player.proxmine.interval);
            proxmine->setAmmo(&ammo_[weaponType]);

            weapon = proxmine;
            break;
        }
        case WeaponTypeChainsaw: {
            WeaponChainsawComponentPtr chainsaw =
                boost::make_shared<WeaponChainsawComponent>(SceneObjectTypePlayerMissile);

            chainsaw->setDamage(settings.player.chainsaw.damage);

            weapon = chainsaw;
            break;
        }
        case WeaponTypeMachineGun: {
            WeaponMachineGunComponentPtr mgun =
                boost::make_shared<WeaponMachineGunComponent>(SceneObjectTypePlayerMissile);

            mgun->setDamage(settings.player.machineGun.damage);
            mgun->setVelocity(settings.player.machineGun.velocity);
            mgun->setSpreadAngle(settings.player.machineGun.spreadAngle);
            mgun->setLoopDelay(settings.player.machineGun.interval);
            mgun->setTweakPos(true);
            mgun->setTweakOffset(weaponPos_);
            mgun->setAmmo(&ammo_[weaponType]);

            weapon = mgun;
            break;
        }
        case WeaponTypeLGun: {
            WeaponLGunComponentPtr lgun =
                boost::make_shared<WeaponLGunComponent>(SceneObjectTypePlayerMissile);

            lgun->setI(1);
            lgun->setLength(settings.player.lgun.length);
            lgun->setDamage(settings.player.lgun.damage);
            lgun->setImpulse(settings.player.lgun.impulse);
            lgun->setAmmo(&ammo_[WeaponTypePlasmaGun]);

            weapon = lgun;
            break;
        }
        case WeaponTypePlasmaGun: {
            WeaponPlasmaGunComponentPtr pgun =
                boost::make_shared<WeaponPlasmaGunComponent>(SceneObjectTypePlayerMissile);

            pgun->setDamage(settings.player.plasmaGun.damage);
            pgun->setVelocity(settings.player.plasmaGun.velocity);
            pgun->setSpreadAngle(settings.player.plasmaGun.spreadAngle);
            pgun->setNumShots(1);
            pgun->setInterval(settings.player.plasmaGun.interval);
            pgun->setAmmo(&ammo_[weaponType]);
            pgun->setTweakOffset(weaponPos_);

            weapon = pgun;
            break;
        }
        case WeaponTypeSuperShotgun: {
            WeaponShotgunComponentPtr shotgun =
                boost::make_shared<WeaponShotgunComponent>(true, SceneObjectTypePlayerMissile);

            shotgun->setDamage(settings.player.shotgun.damage);
            shotgun->setVelocity(settings.player.shotgun.velocity);
            shotgun->setSpreadAngle(settings.player.shotgun.spreadAngle);
            shotgun->setNumPellets(settings.player.shotgun.numPellets);
            shotgun->setInterval(settings.player.shotgun.interval);
            shotgun->setAmmo(&ammo_[WeaponTypeShotgun]);
            shotgun->setTweakPos(true);
            shotgun->setTweakOffset(weaponPos_);

            weapon = shotgun;
            break;
        }
        case WeaponTypeEShield: {
            WeaponEShieldComponentPtr eshield =
                boost::make_shared<WeaponEShieldComponent>(SceneObjectTypePlayerMissile);

            eshield->setAmmo(&ammo_[weaponType]);

            weapon = eshield;
            break;
        }
        default:
            assert(false);
            return false;
        }

        static const float gunOffset = 0.4f;

        if (weaponType == WeaponTypeChainsaw) {
            weapon->setPos(weaponPos_ + b2Vec2(0.0f, -0.15f) + angle2vec(weaponAngle_, 1.5f + gunOffset));
        } else if (weaponType == WeaponTypeRLauncher) {
            weapon->setPos(weaponPos_ + angle2vec(weaponAngle_, 1.0f + gunOffset + 0.5f));
        } else if (weaponType == WeaponTypePlasmaGun) {
            weapon->setPos(weaponPos_ + angle2vec(weaponAngle_, 1.0f + gunOffset));
        } else if (weaponType == WeaponTypeMachineGun) {
            weapon->setPos(weaponPos_ + angle2vec(weaponAngle_, 1.0f + gunOffset));
        } else if (weaponType == WeaponTypeBlaster) {
            weapon->setPos(weaponPos_ + angle2vec(weaponAngle_, 1.0f + gunOffset));
        } else if (weaponType == WeaponTypeSuperBlaster) {
            weapon->setPos(weaponPos_ + angle2vec(weaponAngle_, 1.0f + gunOffset));
        } else if ((weaponType == WeaponTypeShotgun) || (weaponType == WeaponTypeSuperShotgun)) {
            weapon->setPos(weaponPos_ + angle2vec(weaponAngle_, 0.8f + gunOffset));
            weapon->setFlashDistance(0.2f);
        } else {
            weapon->setPos(weaponPos_);
            weapon->setFlashDistance(weaponFlashDistance_ + gunOffset + 0.4f);
        }
        weapon->setAngle(weaponAngle_);

        if (scene() && weapons_[slot]) {
            weapons_[slot]->removeFromParent();
        }

        weapons_[slot] = weapon;

        if (scene()) {
            parent()->addComponent(weapons_[slot]);
        }

        if (weaponSlotPrimary(slot)) {
            if (!this->weapon()) {
                primarySlot_ = slot;
            }
        } else {
            if (!altWeapon()) {
                secondarySlot_ = slot;
            }
        }

        weapon->setTriggerCrosshair(true);

        return true;
    }

    void PlayerComponent::switchWeapon(WeaponSlot slot)
    {
        if (!parent() || parent()->dead()) {
            return;
        }

        if (!weapons_[slot]) {
            return;
        }

        if (weaponSlotPrimary(slot)) {
            if (primarySlot_ == slot) {
                return;
            }

            if (weapon()) {
                weapon()->trigger(false);
                weapon()->cancel();
            }

            primarySlot_ = slot;
        } else {
            if (secondarySlot_ == slot) {
                return;
            }

            if (altWeapon()) {
                altWeapon()->trigger(false);
                altWeapon()->cancel();
            }

            secondarySlot_ = slot;
        }

        sndSwitch_->play();
    }

    bool PlayerComponent::haveWeapon(WeaponType weaponType)
    {
        WeaponSlot slot = WeaponComponent::getSlot(weaponType);

        return (weapons_[slot] && (weaponType == weapons_[slot]->weaponType()));
    }

    float PlayerComponent::ammo(WeaponType weaponType)
    {
        return ammo_[ammoWeaponType(weaponType)];
    }

    void PlayerComponent::changeAmmo(WeaponType weaponType, float amount)
    {
        ammo_[ammoWeaponType(weaponType)] += amount;
    }

    void PlayerComponent::merge(const PlayerComponentPtr& other)
    {
        other->setNumHearts(numHearts() - 1);
        other->setNumGems(numGems());

        for (int i = 0; i < WeaponSlotMax + 1; ++i) {
            if (weapons_[i]) {
                other->giveWeapon(weapons_[i]->weaponType());
            }
        }

        for (int i = 0; i < WeaponTypeMax + 1; ++i) {
            other->ammo_[i] = ammo_[i];
        }

        other->primarySlot_ = primarySlot_;
        other->secondarySlot_ = secondarySlot_;

        /*
         * Share the inventory... really?
         */
        other->inventory_ = inventory_;

        other->setFlashlight(flashlight());
        other->setEarpiece(earpiece());
        other->setStanding(standing());
        other->setHaveGun(haveGun());
        other->setHaveBackpack(haveBackpack());

        other->flags_ = flags_;
    }

    void PlayerComponent::changeLifeNoDamage(float value)
    {
        parent()->setLife(parent()->life() + value);
        prevHealth_ = parent()->life();
    }

    void PlayerComponent::setStamina(float value)
    {
        if ((maxStamina_ >= 0) && (value > maxStamina_)) {
            stamina_ = maxStamina_;
        } else {
            stamina_ = value;
            if (stamina_ < 0.0f) {
                stamina_ = 0.0f;
            }
        }
    }

    void PlayerComponent::setMaxStamina(float value)
    {
        maxStamina_ = value;

        if ((maxStamina_ >= 0) && (stamina_ > maxStamina_)) {
            stamina_ = maxStamina_;
        }
    }

    float PlayerComponent::staminaPercent() const
    {
        if (parent()->dead()) {
            return 0.0f;
        }

        if (maxStamina_ < 0) {
            return 1.0f;
        }

        assert(stamina_ <= maxStamina_);

        return static_cast<float>(stamina_) / maxStamina_;
    }

    void PlayerComponent::changeStamina(float value)
    {
        setStamina(stamina_ + value);
    }
}
