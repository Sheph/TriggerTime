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

#include "Scene.h"
#include "Settings.h"
#include "SceneObject.h"
#include "PhasedComponentManager.h"
#include "PhysicsComponentManager.h"
#include "CollisionComponentManager.h"
#include "RenderComponentManager.h"
#include "UIComponentManager.h"
#include "PhasedComponent.h"
#include "CameraComponent.h"
#include "PhysicsComponent.h"
#include "CollisionComponent.h"
#include "RenderComponent.h"
#include "UIComponent.h"
#include "PauseMenuComponent.h"
#include "GameOverComponent.h"
#include "SceneObjectFactory.h"
#include "Renderer.h"
#include "Logger.h"
#include "InputManager.h"
#include "DebugDraw.h"
#include "KnobComponent.h"
#include "PlayerComponent.h"
#include "AssetManager.h"
#include "Const.h"
#include "InputControllerTS.h"
#include "InputControllerPC.h"
#include "UserData.h"
#include "GameShell.h"
#include <Rocket/Core/ElementDocument.h>
#include <boost/make_shared.hpp>
#include <cmath>

namespace af
{
    namespace
    {
        class RegisterComponentVisitor : public ComponentVisitor
        {
        public:
            RegisterComponentVisitor()
            {
            }

            ~RegisterComponentVisitor()
            {
            }

            virtual void visitPhasedComponent(const PhasedComponentPtr& component)
            {
                phasedComponentManager->addComponent(component);
            }

            virtual void visitPhysicsComponent(const PhysicsComponentPtr& component)
            {
                physicsComponentManager->addComponent(component);
            }

            virtual void visitCollisionComponent(const CollisionComponentPtr& component)
            {
                collisionComponentManager->addComponent(component);
            }

            virtual void visitRenderComponent(const RenderComponentPtr& component)
            {
                renderComponentManager->addComponent(component);
            }

            virtual void visitUIComponent(const UIComponentPtr& component)
            {
                uiComponentManager->addComponent(component);
            }

            boost::shared_ptr<PhasedComponentManager> phasedComponentManager;
            boost::shared_ptr<CollisionComponentManager> collisionComponentManager;
            boost::shared_ptr<PhysicsComponentManager> physicsComponentManager;
            boost::shared_ptr<RenderComponentManager> renderComponentManager;
            boost::shared_ptr<UIComponentManager> uiComponentManager;
        };

        typedef std::map<b2Joint*, JointProxyPtr> JointMap;

        class DestructionListener : public b2DestructionListener
        {
        public:
            DestructionListener()
            {
            }

            ~DestructionListener()
            {
            }

            virtual void SayGoodbye(b2Joint* joint)
            {
                JointMap::iterator it = jointMap.find(joint);

                assert(it != jointMap.end());

                if (it != jointMap.end()) {
                    it->second->setInvalid();
                    jointMap.erase(it);
                }
            }

            virtual void SayGoodbye(b2Fixture* fixture)
            {
            }

            JointMap jointMap;
        };

        class RayCastCallback : public b2RayCastCallback
        {
        public:
            RayCastCallback(const Scene::RayCastFn& fn)
            : fn_(fn)
            {
            }

            ~RayCastCallback()
            {
            }

            virtual float32 ReportFixture(b2Fixture* fixture,
                const b2Vec2& point, const b2Vec2& normal, float32 fraction)
            {
                const b2Filter& filter = fixture->GetFilterData();

                if ((filter.categoryBits == 0) &&
                    (filter.maskBits == 0) &&
                    (filter.groupIndex == 0)) {
                    /*
                     * "disabled" fixture, pretend it isn't there.
                     */
                    return -1;
                }

                return fn_(fixture, point, normal, fraction);
            }

        private:
            const Scene::RayCastFn& fn_;
        };

        class QueryAABBCallback : public b2QueryCallback
        {
        public:
            QueryAABBCallback(const Scene::QueryAABBFn& fn)
            : fn_(fn)
            {
            }

            ~QueryAABBCallback()
            {
            }

            virtual bool ReportFixture(b2Fixture* fixture)
            {
                const b2Filter& filter = fixture->GetFilterData();

                if ((filter.categoryBits == 0) &&
                    (filter.maskBits == 0) &&
                    (filter.groupIndex == 0)) {
                    /*
                     * "disabled" fixture, pretend it isn't there.
                     */
                    return true;
                }

                return fn_(fixture);
            }

        private:
            const Scene::QueryAABBFn& fn_;
        };

        class QueryPointCallback : public b2QueryCallback
        {
        public:
            QueryPointCallback(const b2Vec2& point, const Scene::QueryPointFn& fn)
            : point_(point),
              fn_(fn)
            {
            }

            ~QueryPointCallback()
            {
            }

            virtual bool ReportFixture(b2Fixture* fixture)
            {
                const b2Filter& filter = fixture->GetFilterData();

                if ((filter.categoryBits == 0) &&
                    (filter.maskBits == 0) &&
                    (filter.groupIndex == 0)) {
                    /*
                     * "disabled" fixture, pretend it isn't there.
                     */
                    return true;
                }

                if (fixture->TestPoint(point_)) {
                    return fn_(fixture);
                } else {
                    return true;
                }
            }

        private:
            b2Vec2 point_;
            const Scene::QueryPointFn& fn_;
        };
    }

    typedef std::map<SInt32, Scene::TimerFn> TimerMap;

    class Scene::Impl
    {
    public:
        Impl()
        : fixedTimestepAccumulator_(0.0f),
          fixedTimestepAccumulatorRatio_(0.0f),
          nextTimerCookie_(1)
        {
            if (settings.physics.debugShape) {
                debugDraw_.AppendFlags(b2Draw::e_shapeBit);
            }

            if (settings.physics.debugJoint) {
                debugDraw_.AppendFlags(b2Draw::e_jointBit);
            }

            if (settings.physics.debugAABB) {
                debugDraw_.AppendFlags(b2Draw::e_aabbBit);
            }

            if (settings.physics.debugPair) {
                debugDraw_.AppendFlags(b2Draw::e_pairBit);
            }

            if (settings.physics.debugCenterOfMass) {
                debugDraw_.AppendFlags(b2Draw::e_centerOfMassBit);
            }

            v_.phasedComponentManager = boost::make_shared<PhasedComponentManager>();
            v_.collisionComponentManager = boost::make_shared<CollisionComponentManager>();
            v_.physicsComponentManager =
                boost::make_shared<PhysicsComponentManager>(v_.collisionComponentManager, &dl_, &debugDraw_);
            v_.renderComponentManager = boost::make_shared<RenderComponentManager>();
            v_.uiComponentManager = boost::make_shared<UIComponentManager>();

            timerIt_ = timers_.end();
        }

        ~Impl()
        {
        }

        void resetSmoothStates()
        {
            for (b2Body* body = v_.physicsComponentManager->world().GetBodyList();
                 body != NULL;
                 body = body->GetNext()) {
                SceneObject* obj = SceneObject::fromBody(body);

                obj->resetSmooth();
            }
        }

        void smoothStates()
        {
            for (b2Body* body = v_.physicsComponentManager->world().GetBodyList();
                 body != NULL;
                 body = body->GetNext()) {
                if (body->GetType() == b2_staticBody) {
                    continue;
                }

                SceneObject* obj = SceneObject::fromBody(body);

                obj->updateSmooth(fixedTimestepAccumulatorRatio_);
            }
        }

        float fixedTimestepAccumulator_;
        float fixedTimestepAccumulatorRatio_;

        DestructionListener dl_;
        DebugDraw debugDraw_;
        RegisterComponentVisitor v_;
        TimerMap timers_;
        SInt32 nextTimerCookie_;
        TimerMap::const_iterator timerIt_;
    };

    Scene::Scene(float gameWidth, float gameHeight,
        const std::string& scriptPath, const std::string& assetPath)
    : impl_(new Impl()),
      gameWidth_(gameWidth),
      gameHeight_(gameHeight),
      inputMode_(InputModeMenu),
      combatTargetsQueried_(false),
      playable_(false),
      respawnPoint_(b2Vec2(0.0f, 0.0f), b2Rot(0.0f)),
      paused_(false),
      cutscene_(false),
      quit_(false),
      cameraMoveStarted_(false),
      scriptPath_(scriptPath),
      assetPath_(assetPath),
      gameover_(false),
      gameoverTime_(0.0f),
      firstUpdate_(true),
      stats_(boost::make_shared<Statistics>())
    {
        for (int i = 0; i <= UpgradeIdMax; ++i) {
            upgrades_[i] = 0.0f;
        }

        inputActive_[0] = true;
        inputActive_[1] = true;
        inputActive_[2] = false;
        inputActive_[3] = false;
        inputActive_[4] = false;
        inputActive_[5] = false;

        setScene(this);

        impl_->v_.phasedComponentManager->setScene(this);
        impl_->v_.collisionComponentManager->setScene(this);
        impl_->v_.physicsComponentManager->setScene(this);
        impl_->v_.renderComponentManager->setScene(this);
        impl_->v_.uiComponentManager->setScene(this);

        camera_ = boost::make_shared<SceneObject>();

        camera_->addComponent(boost::make_shared<CameraComponent>(gameWidth, gameHeight));

        addObject(camera_);

        dummy_ = boost::make_shared<SceneObject>();

        lrComponent_ = boost::make_shared<LRComponent>("menu", 2);
        lightsComponent_ = boost::make_shared<RenderLightComponent>(zOrderLight);
        damageComponent_ = boost::make_shared<RenderDamageComponent>(zOrderTop);
        letterBox_ = boost::make_shared<LetterBoxComponent>(settings.letterBox.distance, settings.letterBox.duration, -3);
        crosshair_ = boost::make_shared<CrosshairComponent>(-4);

        dummy_->addComponent(lrComponent_);
        dummy_->addComponent(lightsComponent_);
        dummy_->addComponent(damageComponent_);
        dummy_->addComponent(letterBox_);
        dummy_->addComponent(crosshair_);

        addObject(dummy_);
    }

    Scene::~Scene()
    {
        assert(impl_->dl_.jointMap.empty());

        delete impl_;
    }

    void Scene::prepare()
    {
        if (settings.touchScreen.enabled) {
            input_ = boost::make_shared<InputControllerTS>(this, lrComponent_->context());
        } else {
            input_ = boost::make_shared<InputControllerPC>(this, lrComponent_->context());
        }

        inputMenuUI_ = input_->menuUI();
        inputGameUI_ = input_->gameUI();
        inputPlayer_ = input_->player();
        inputCutscene_ = input_->cutscene();
        inputRoboArm_ = input_->roboArm();
        inputBoat_ = input_->boat();
        inputMech_ = input_->mech();
        inputTurret_ = input_->turret();

        inputMenuUI_->setActive(true);

        if (!playable_) {
            return;
        }

        purchaseUpgrades();

        healthBar_ = boost::make_shared<SceneObject>();

        healthBar_->setPos(b2Vec2(settings.healthBar.xOffset, gameHeight_ - settings.healthBar.height - settings.healthBar.yOffset));

        healthBarComponent_ = boost::make_shared<HealthBarComponent>(b2Vec2(0.0f, 0.0f), settings.healthBar.width, settings.healthBar.height);

        healthBar_->addComponent(healthBarComponent_);

        addObject(healthBar_);

        staminaBar_ = boost::make_shared<SceneObject>();

        staminaBar_->setPos(b2Vec2(settings.staminaBar.xOffset, gameHeight_ - settings.staminaBar.height - settings.staminaBar.yOffset));

        staminaBarComponent_ = boost::make_shared<StaminaBarComponent>(b2Vec2(0.0f, 0.0f), settings.staminaBar.width, settings.staminaBar.height);

        staminaBar_->addComponent(staminaBarComponent_);

        addObject(staminaBar_);

        indicators_ = boost::make_shared<SceneObject>();

        indicators_->setPos(b2Vec2(settings.healthBar.xOffset + settings.healthBar.width + 2.0f,
            gameHeight_ - settings.healthBar.height / 2.0f - settings.healthBar.yOffset));

        heartsIndicator_ =
            boost::make_shared<NumericIndicatorComponent>(b2Vec2_zero,
                assetManager.getDrawable("common1/healthbar_heart.png"));

        heartsIndicator_->setCharSize(settings.healthBar.height);
        heartsIndicator_->setColor(Color(1.0f, 0.0f, 0.0f));

        indicators_->addComponent(heartsIndicator_);

        gemsIndicator_ =
            boost::make_shared<NumericIndicatorComponent>(b2Vec2(3.25f, 0.0f),
                assetManager.getDrawable("common2/powerup_gem_spin0.png"));

        gemsIndicator_->setCharSize(settings.healthBar.height);
        gemsIndicator_->setColor(settings.powerupGem.color);

        gemsIndicator_->setAmount(userData.numGems());

        indicators_->addComponent(gemsIndicator_);

        addObject(indicators_);

        inventoryDisplay_ =
            boost::make_shared<InventoryComponent>(
                b2Vec2(gameWidth_ - settings.healthBar.xOffset, gameHeight_ - settings.healthBar.yOffset),
                3.0f);

        dummy_->addComponent(inventoryDisplay_);

        upgradeDisplay_ =
            boost::make_shared<UpgradeDisplayComponent>(
                b2Vec2(gameWidth_ - settings.healthBar.xOffset, settings.healthBar.yOffset),
                2.0f);

        dummy_->addComponent(upgradeDisplay_);

        player_ = createPlayer();

        UInt32 extraHearts = static_cast<UInt32>(upgradeCount(UpgradeIdExtraLife));

        player_->findComponent<PlayerComponent>()->setNumHearts(
            settings.player.numHearts + extraHearts);

        heartsIndicator_->setAmount(settings.player.numHearts + extraHearts);

        inventoryDisplay_->setInventory(player_->findComponent<PlayerComponent>()->inventory());

        player_->setTransform(respawnPoint_);

        addObject(player_);

        camera_->findComponent<CameraComponent>()->setTarget(player_);

        if (!settings.hud.enabled) {
            healthBarComponent_->setVisible(false);
            staminaBarComponent_->setVisible(false);
            heartsIndicator_->setVisible(false);
            gemsIndicator_->setVisible(false);
            inventoryDisplay_->setVisible(false);
            upgradeDisplay_->setVisible(false);
        }
    }

    void Scene::cleanup()
    {
        combatAllies_.clear();
        combatTargets_.clear();

        input_.reset();

        removeAllObjects();

        instances_.clear();
        healthBar_.reset();
        staminaBar_.reset();
        indicators_.reset();
        player_.reset();
        camera_.reset();
        dummy_.reset();
        lightsComponent_.reset();
        damageComponent_.reset();
        healthBarComponent_.reset();
        staminaBarComponent_.reset();
        heartsIndicator_.reset();
        gemsIndicator_.reset();
        inventoryDisplay_.reset();
        upgradeDisplay_.reset();
        letterBox_.reset();
        crosshair_.reset();
        lrComponent_.reset();

        impl_->timers_.clear();

        impl_->v_.phasedComponentManager->cleanup();
        impl_->v_.collisionComponentManager->cleanup();
        impl_->v_.physicsComponentManager->cleanup();
        impl_->v_.renderComponentManager->cleanup();
        impl_->v_.uiComponentManager->cleanup();
    }

    void Scene::registerComponent(const ComponentPtr& component)
    {
        component->accept(impl_->v_);
    }

    void Scene::unregisterComponent(const ComponentPtr& component)
    {
        if (component->manager()) {
            component->manager()->removeComponent(component);
        }
    }

    void Scene::freezeComponent(const ComponentPtr& component)
    {
        if (component->manager()) {
            component->manager()->freezeComponent(component);
        }
    }

    void Scene::thawComponent(const ComponentPtr& component)
    {
        if (component->manager()) {
            component->manager()->thawComponent(component);
        }
    }

    void Scene::update(float dt)
    {
        InputMode newInputMode;

        if (!paused_) {
            if (playable_) {
                newInputMode = cutscene_ ? InputModeCutscene : InputModeGame;
            } else {
                newInputMode = cutscene_ ? InputModeCutscene : InputModeMenu;
            }
        } else {
            newInputMode = InputModeMenu;
        }

        if (inputMode_ != newInputMode) {
            switch (newInputMode) {
            case InputModeMenu:
                if (inputMode_ == InputModeGame) {
                    inputActive_[0] = inputGameUI_->active();
                    inputActive_[1] = inputPlayer_->active();
                    inputActive_[2] = inputRoboArm_->active();
                    inputActive_[3] = inputBoat_->active();
                    inputActive_[4] = inputMech_->active();
                    inputActive_[5] = inputTurret_->active();
                }
                inputMenuUI_->setActive(true);
                break;
            case InputModeGame:
                inputMenuUI_->setActive(false);
                inputGameUI_->setActive(inputActive_[0]);
                inputPlayer_->setActive(inputActive_[1]);
                inputRoboArm_->setActive(inputActive_[2]);
                inputBoat_->setActive(inputActive_[3]);
                inputMech_->setActive(inputActive_[4]);
                inputTurret_->setActive(inputActive_[5]);
                inputCutscene_->setActive(false);
                break;
            case InputModeCutscene:
                if (inputMode_ == InputModeGame) {
                    inputActive_[0] = true;
                    inputActive_[1] = inputPlayer_->active();
                    inputActive_[2] = inputRoboArm_->active();
                    inputActive_[3] = inputBoat_->active();
                    inputActive_[4] = inputMech_->active();
                    inputActive_[5] = inputTurret_->active();
                }
                inputMenuUI_->setActive(false);
                inputGameUI_->setActive(false);
                inputPlayer_->setActive(false);
                inputRoboArm_->setActive(false);
                inputBoat_->setActive(false);
                inputMech_->setActive(false);
                inputTurret_->setActive(false);
                inputCutscene_->setActive(true);
                break;
            default:
                assert(0);
                break;
            }
        }

        inputMode_ = newInputMode;

        input_->update(dt);

        if (input_->slowmoPressed()) {
            dt /= settings.physics.slowmoFactor;
        }

        if (!paused_ && input_->pausePressed()) {
            LOG4CPLUS_INFO(logger(), "game paused");

            setPaused(true);

            Rocket::Core::ElementDocument* doc = lrComponent_->context()->LoadDocument("ui/pause.rml");
            doc->Show();
            doc->RemoveReference();
        }

        if (gameover_ && !paused_) {
            gameoverTime_ -= dt;
            if (gameoverTime_ <= 0.0f) {
                gameover_ = false;

                LOG4CPLUS_INFO(logger(), "game over");

                setPaused(true);

                Rocket::Core::ElementDocument* doc = lrComponent_->context()->LoadDocument("ui/gameover.rml");
                doc->RemoveReference();
            }
        }

        CameraComponentPtr cc = camera_->findComponent<CameraComponent>();

        if (!paused_) {
            stats_->incPlayTime(dt);

            impl_->fixedTimestepAccumulator_ += dt;

            UInt32 numSteps = static_cast<UInt32>(std::floor(impl_->fixedTimestepAccumulator_ / settings.physics.fixedTimestep));

            if (numSteps > 0) {
                impl_->fixedTimestepAccumulator_ -= settings.physics.fixedTimestep * numSteps;
            }

            impl_->fixedTimestepAccumulatorRatio_ = impl_->fixedTimestepAccumulator_ / settings.physics.fixedTimestep;

            UInt32 numStepsClamped = (std::min)(numSteps, settings.physics.maxSteps);

            for (UInt32 i = 0; i < numStepsClamped; ++i) {
                impl_->resetSmoothStates();

                if (!impl_->timers_.empty()) {
                    SInt32 lastCookie = impl_->timers_.rbegin()->first;
                    for (impl_->timerIt_ = impl_->timers_.begin();
                        impl_->timerIt_ != impl_->timers_.end();) {
                        if (impl_->timerIt_->first > lastCookie) {
                            break;
                        }
                        const Scene::TimerFn& fn = impl_->timerIt_->second;
                        ++impl_->timerIt_;
                        fn(settings.physics.fixedTimestep);
                    }
                    impl_->timerIt_ = impl_->timers_.end();
                }

                if (playable_) {
                    if (player_->dead() && !player_->scene() && !(cameraMoveStarted_ && cc->isMoving()) && !gameover_) {
                        if (player_->findComponent<PlayerComponent>()->numHearts() == 0) {
                            gameover_ = true;
                            gameoverTime_ = settings.gameover.timeout;
                        } else {
                            bool trackingPlayer = (cc->target() == player_) || (cc->moveToTarget() == player_);

                            if (trackingPlayer &&
                                (cc->parent()->pos() != respawnPoint_.p)) {
                                cc->moveTo(respawnPoint_.p, EaseInOutQuad, 1.0f);
                                cameraMoveStarted_ = true;
                            } else {
                                PlayerComponentPtr oldPc = player_->findComponent<PlayerComponent>();

                                player_ = createPlayer();

                                PlayerComponentPtr pc = player_->findComponent<PlayerComponent>();

                                oldPc->merge(pc);

                                heartsIndicator_->setAmount(pc->numHearts());

                                inventoryDisplay_->setInventory(pc->inventory());

                                player_->setTransform(respawnPoint_);

                                addObject(player_);

                                if (trackingPlayer || cameraMoveStarted_) {
                                    cc->setTarget(player_);
                                }

                                cameraMoveStarted_ = false;
                            }
                        }
                    }

                    healthBarComponent_->setPercent(player_->lifePercent());

                    PlayerComponentPtr pc = player_->findComponent<PlayerComponent>();

                    staminaBarComponent_->setPercent(pc->staminaPercent());

                    gemsIndicator_->setAmount(userData.numGems() + pc->numGems());
                }

                impl_->v_.collisionComponentManager->step();
                impl_->v_.physicsComponentManager->update(settings.physics.fixedTimestep);
                impl_->v_.collisionComponentManager->step();

                impl_->v_.collisionComponentManager->update(settings.physics.fixedTimestep);

                impl_->v_.phasedComponentManager->update(settings.physics.fixedTimestep);
            }

            impl_->smoothStates();

            impl_->v_.phasedComponentManager->preRender(dt);

            if (numStepsClamped > 0) {
                freezeThawObjects(cc->getTrueAABB());
            }

            updateCombat();

            impl_->v_.uiComponentManager->update(dt);

            /*
             * FIXME: Or not?
             * The reason we call render component manager update _after_ ui component manager
             * update is because ui component manager update can actually invoke
             * scripts (e.g. in-game dialog "ok" action) and, thus, modify
             * scene, i.e. remove/add render components. However, render component manager assumes
             * that scene is not modified in between its "update" and "render" calls.
             * Having render component manager update after ui component manager update should be
             * ok though since render component manager update is a part of render itself, i.e. it
             * just culls the scene. However, ui component manager update is more than render preparations,
             * it can run custom logic...
             */
            impl_->v_.renderComponentManager->update(dt);
        } else {
            impl_->v_.uiComponentManager->update(dt);
        }

        b2AABB cameraOOBB = cc->getOOBB();

        b2Vec2 size = cameraOOBB.upperBound - cameraOOBB.lowerBound;

        renderer.lookAt(cameraOOBB.GetCenter(), size.x, size.y, cc->getActualAngle());

        impl_->v_.renderComponentManager->render();

        if (input_->physicsDebugPressed()) {
            impl_->v_.physicsComponentManager->world().DrawDebugData();
        }

        if (input_->gameDebugPressed()) {
            impl_->v_.physicsComponentManager->debugDraw();
            impl_->v_.collisionComponentManager->debugDraw();
            impl_->v_.phasedComponentManager->debugDraw();
            impl_->v_.renderComponentManager->debugDraw();
        }

        renderer.lookAt(b2Vec2(gameWidth_ / 2, gameHeight_ / 2), gameWidth_, gameHeight_, 0.0f);

        impl_->v_.uiComponentManager->render();

        if (input_->gameDebugPressed()) {
            impl_->v_.uiComponentManager->debugDraw();
        }

        inputManager.processed();

        renderer.swap();

        firstUpdate_ = false;
    }

    JointProxyPtr Scene::addJoint(const b2JointDef* jointDef,
                                  const std::string& name)
    {
        b2Joint* joint = impl_->v_.physicsComponentManager->world().CreateJoint(jointDef);

        joint->SetUserData(this);

        JointProxyPtr jointProxy;

        switch (jointDef->type) {
        case e_revoluteJoint:
            jointProxy = boost::make_shared<RevoluteJointProxy>(
                static_cast<b2RevoluteJoint*>(joint), name);
            break;
        case e_prismaticJoint:
            jointProxy = boost::make_shared<PrismaticJointProxy>(
                static_cast<b2PrismaticJoint*>(joint), name);
            break;
        case e_distanceJoint:
            jointProxy = boost::make_shared<DistanceJointProxy>(
                static_cast<b2DistanceJoint*>(joint), name);
            break;
        case e_pulleyJoint:
            jointProxy = boost::make_shared<PulleyJointProxy>(
                static_cast<b2PulleyJoint*>(joint), name);
            break;
        case e_wheelJoint:
            jointProxy = boost::make_shared<WheelJointProxy>(
                static_cast<b2WheelJoint*>(joint), name);
            break;
        case e_motorJoint:
            jointProxy = boost::make_shared<MotorJointProxy>(
                static_cast<b2MotorJoint*>(joint), name);
            break;
        case e_weldJoint:
            jointProxy = boost::make_shared<WeldJointProxy>(
                static_cast<b2WeldJoint*>(joint), name);
            break;
        case e_frictionJoint:
            jointProxy = boost::make_shared<FrictionJointProxy>(
                static_cast<b2FrictionJoint*>(joint), name);
            break;
        case e_ropeJoint:
            jointProxy = boost::make_shared<RopeJointProxy>(
                static_cast<b2RopeJoint*>(joint), name);
            break;
        case e_gearJoint: {
            JointMap::const_iterator it1 = impl_->dl_.jointMap.find(static_cast<const b2GearJointDef*>(jointDef)->joint1);
            JointMap::const_iterator it2 = impl_->dl_.jointMap.find(static_cast<const b2GearJointDef*>(jointDef)->joint2);

            if ((it1 == impl_->dl_.jointMap.end()) ||
                (it2 == impl_->dl_.jointMap.end())) {
                assert(false);
                LOG4CPLUS_ERROR(logger(), "Gear joint's child joints don't exist");
                impl_->v_.physicsComponentManager->world().DestroyJoint(joint);
                return jointProxy;
            }

            jointProxy = boost::make_shared<GearJointProxy>(
                static_cast<b2GearJoint*>(joint), it1->second, it2->second, name);
            break;
        }
        case e_unknownJoint:
        case e_mouseJoint:
        default:
            LOG4CPLUS_ERROR(logger(), "Bad joint type: " << jointDef->type);
            impl_->v_.physicsComponentManager->world().DestroyJoint(joint);
            return jointProxy;
        }

        impl_->dl_.jointMap[joint] = jointProxy;

        return jointProxy;
    }

    JointProxyPtr Scene::addGearJoint(const SceneObjectPtr& objectA,
                                      const SceneObjectPtr& objectB,
                                      const JointProxyPtr& joint1,
                                      const JointProxyPtr& joint2,
                                      float ratio,
                                      bool collideConnected)
    {
        b2GearJointDef jointDef;

        jointDef.bodyA = objectA->body();
        jointDef.bodyB = objectB->body();
        jointDef.collideConnected = collideConnected;
        jointDef.joint1 = joint1->joint();
        jointDef.joint2 = joint2->joint();
        jointDef.ratio = ratio;

        return addJoint(&jointDef);
    }

    WeldJointProxyPtr Scene::addWeldJoint(const SceneObjectPtr& objectA,
                                      const SceneObjectPtr& objectB,
                                      const b2Vec2& worldAnchor,
                                      bool collideConnected)
    {
        b2WeldJointDef jointDef;

        jointDef.Initialize(objectA->body(), objectB->body(), worldAnchor);
        jointDef.collideConnected = collideConnected;

        return boost::static_pointer_cast<WeldJointProxy>(addJoint(&jointDef));
    }

    JointProxyPtr Scene::addWeldJointEx(const SceneObjectPtr& objectA,
                                        const SceneObjectPtr& objectB,
                                        const b2Vec2& worldAnchor,
                                        float frequency,
                                        float dampingRatio,
                                        bool collideConnected)
    {
        b2WeldJointDef jointDef;

        jointDef.Initialize(objectA->body(), objectB->body(), worldAnchor);
        jointDef.collideConnected = collideConnected;
        jointDef.frequencyHz = frequency;
        jointDef.dampingRatio = dampingRatio;

        return addJoint(&jointDef);
    }

    RevoluteJointProxyPtr Scene::addRevoluteJoint(const SceneObjectPtr& objectA,
                                          const SceneObjectPtr& objectB,
                                          const b2Vec2& localAnchorA,
                                          const b2Vec2& localAnchorB,
                                          bool collideConnected)
    {
        b2RevoluteJointDef jointDef;

        jointDef.bodyA = objectA->body();
        jointDef.bodyB = objectB->body();
        jointDef.localAnchorA = localAnchorA;
        jointDef.localAnchorB = localAnchorB;
        jointDef.referenceAngle = jointDef.bodyB->GetAngle() - jointDef.bodyA->GetAngle();
        jointDef.collideConnected = collideConnected;

        return boost::static_pointer_cast<RevoluteJointProxy>(addJoint(&jointDef));
    }

    RopeJointProxyPtr Scene::addRopeJoint(const SceneObjectPtr& objectA,
                                          const SceneObjectPtr& objectB,
                                          const b2Vec2& localAnchorA,
                                          const b2Vec2& localAnchorB,
                                          float maxLength,
                                          bool collideConnected)
    {
        b2RopeJointDef jointDef;

        jointDef.bodyA = objectA->body();
        jointDef.bodyB = objectB->body();
        jointDef.localAnchorA = localAnchorA;
        jointDef.localAnchorB = localAnchorB;
        jointDef.maxLength = maxLength;
        jointDef.collideConnected = collideConnected;

        return boost::static_pointer_cast<RopeJointProxy>(addJoint(&jointDef));
    }

    PrismaticJointProxyPtr Scene::addPrismaticJoint(const SceneObjectPtr& objectA,
                                           const SceneObjectPtr& objectB,
                                           const b2Vec2& localAnchorA,
                                           const b2Vec2& localAnchorB,
                                           const b2Vec2& localAxis,
                                           bool collideConnected)
    {
        b2PrismaticJointDef jointDef;

        jointDef.bodyA = objectA->body();
        jointDef.bodyB = objectB->body();
        jointDef.localAnchorA = localAnchorA;
        jointDef.localAnchorB = localAnchorB;
        jointDef.localAxisA = localAxis;
        jointDef.collideConnected = collideConnected;
        jointDef.referenceAngle = jointDef.bodyB->GetAngle() - jointDef.bodyA->GetAngle();

        return boost::static_pointer_cast<PrismaticJointProxy>(addJoint(&jointDef));
    }

    PrismaticJointProxyPtr Scene::addPrismaticJoint(const SceneObjectPtr& objectA,
                                           const SceneObjectPtr& objectB,
                                           const b2Vec2& localAnchorA,
                                           const b2Vec2& localAnchorB,
                                           float lowerLimit, float upperLimit,
                                           const b2Vec2& localAxis,
                                           bool collideConnected)
    {
        b2PrismaticJointDef jointDef;

        jointDef.bodyA = objectA->body();
        jointDef.bodyB = objectB->body();
        jointDef.localAnchorA = localAnchorA;
        jointDef.localAnchorB = localAnchorB;
        jointDef.enableLimit = true;
        jointDef.lowerTranslation = lowerLimit;
        jointDef.upperTranslation = upperLimit;
        jointDef.localAxisA = localAxis;
        jointDef.collideConnected = collideConnected;
        jointDef.referenceAngle = jointDef.bodyB->GetAngle() - jointDef.bodyA->GetAngle();

        return boost::static_pointer_cast<PrismaticJointProxy>(addJoint(&jointDef));
    }

    PrismaticJointProxyPtr Scene::addPrismaticJoint(const SceneObjectPtr& objectA,
                                           const SceneObjectPtr& objectB,
                                           const b2Vec2& localAnchorA,
                                           const b2Vec2& localAnchorB,
                                           float lowerLimit, float upperLimit,
                                           const b2Vec2& localAxis,
                                           float referenceAngle,
                                           bool collideConnected)
    {
        b2PrismaticJointDef jointDef;

        jointDef.bodyA = objectA->body();
        jointDef.bodyB = objectB->body();
        jointDef.localAnchorA = localAnchorA;
        jointDef.localAnchorB = localAnchorB;
        jointDef.enableLimit = true;
        jointDef.lowerTranslation = lowerLimit;
        jointDef.upperTranslation = upperLimit;
        jointDef.localAxisA = localAxis;
        jointDef.collideConnected = collideConnected;
        jointDef.referenceAngle = referenceAngle;

        return boost::static_pointer_cast<PrismaticJointProxy>(addJoint(&jointDef));
    }

    MotorJointProxyPtr Scene::addMotorJoint(const SceneObjectPtr& objectA,
                                            const SceneObjectPtr& objectB,
                                            const b2Vec2& linearOffset,
                                            float maxForce,
                                            float maxTorque,
                                            float correctionFactor,
                                            bool collideConnected)
    {
        b2MotorJointDef jointDef;

        jointDef.bodyA = objectA->body();
        jointDef.bodyB = objectB->body();
        jointDef.linearOffset = linearOffset;
        jointDef.angularOffset = jointDef.bodyB->GetAngle() - jointDef.bodyA->GetAngle();
        jointDef.maxForce = maxForce;
        jointDef.maxTorque = maxTorque;
        jointDef.correctionFactor = correctionFactor;
        jointDef.collideConnected = collideConnected;

        return boost::static_pointer_cast<MotorJointProxy>(addJoint(&jointDef));
    }

    FrictionJointProxyPtr Scene::addFrictionJoint(const SceneObjectPtr& objectA,
                                                  const SceneObjectPtr& objectB,
                                                  const b2Vec2& localAnchorA,
                                                  const b2Vec2& localAnchorB,
                                                  float maxForce,
                                                  float maxTorque,
                                                  bool collideConnected)
    {
        b2FrictionJointDef jointDef;

        jointDef.bodyA = objectA->body();
        jointDef.bodyB = objectB->body();
        jointDef.localAnchorA = localAnchorA;
        jointDef.localAnchorB = localAnchorB;
        jointDef.maxForce = maxForce;
        jointDef.maxTorque = maxTorque;
        jointDef.collideConnected = collideConnected;

        return boost::static_pointer_cast<FrictionJointProxy>(addJoint(&jointDef));
    }

    DistanceJointProxyPtr Scene::addDistanceJoint(const SceneObjectPtr& objectA,
                                                  const SceneObjectPtr& objectB,
                                                  const b2Vec2& localAnchorA,
                                                  const b2Vec2& localAnchorB,
                                                  float length,
                                                  float frequency,
                                                  float dampingRatio,
                                                  bool collideConnected)
    {
        b2DistanceJointDef jointDef;

        jointDef.bodyA = objectA->body();
        jointDef.bodyB = objectB->body();
        jointDef.localAnchorA = localAnchorA;
        jointDef.localAnchorB = localAnchorB;
        jointDef.length = length;
        jointDef.frequencyHz = frequency;
        jointDef.dampingRatio = dampingRatio;
        jointDef.collideConnected = collideConnected;

        return boost::static_pointer_cast<DistanceJointProxy>(addJoint(&jointDef));
    }

    void Scene::removeJoint(const JointProxyPtr& joint)
    {
        /*
         * Hold on to this joint while
         * removing.
         */
        JointProxyPtr tmp = joint;

        if (!tmp->valid()) {
            return;
        }

        impl_->v_.physicsComponentManager->world().DestroyJoint(tmp->joint());
        impl_->dl_.jointMap.erase(tmp->joint());
        tmp->setInvalid();
    }

    std::vector<JointProxyPtr> Scene::getJoints(const std::string& name) const
    {
        std::vector<JointProxyPtr> res;

        for (JointMap::const_iterator it = impl_->dl_.jointMap.begin();
             it != impl_->dl_.jointMap.end();
             ++it ) {
            if (it->second->name() == name) {
                res.push_back(it->second);
            }
        }

        return res;
    }

    SInt32 Scene::addTimer(const TimerFn& fn)
    {
        impl_->timers_[impl_->nextTimerCookie_] = fn;

        return impl_->nextTimerCookie_++;
    }

    void Scene::removeTimer(SInt32 cookie)
    {
        TimerMap::iterator it = impl_->timers_.find(cookie);

        if (it == impl_->timers_.end()) {
            LOG4CPLUS_WARN(logger(), "removeTimer(" << cookie << "), " << cookie << " doesn't exist");
            return;
        }

        if (impl_->timerIt_ == it) {
            ++impl_->timerIt_;
        }

        impl_->timers_.erase(it);
    }

    void Scene::addInstances(const Instances& instances)
    {
        instances_.insert(instances.begin(), instances.end());
    }

    void Scene::rayCast(const b2Vec2& p1, const b2Vec2& p2, const RayCastFn& fn)
    {
        RayCastCallback cb(fn);
        impl_->v_.physicsComponentManager->world().RayCast(&cb, p1, p2);
    }

    void Scene::queryAABB(const b2AABB& aabb, const QueryAABBFn& fn)
    {
        QueryAABBCallback cb(fn);
        impl_->v_.physicsComponentManager->world().QueryAABB(&cb, aabb);
    }

    void Scene::queryPoint(const b2Vec2& point, const QueryPointFn& fn)
    {
        b2AABB aabb;
        b2Vec2 d(0.001f, 0.001f);
        aabb.lowerBound = point - d;
        aabb.upperBound = point + d;

        QueryPointCallback cb(point, fn);
        impl_->v_.physicsComponentManager->world().QueryAABB(&cb, aabb);
    }

    void Scene::setRespawnPoint(const b2Transform& value)
    {
        playable_ = true;
        respawnPoint_ = value;
    }

    void Scene::setCutscene(bool value)
    {
        if (player_) {
            PlayerComponentPtr pc = player_->findComponent<PlayerComponent>();
            if (!cutscene_ && value) {
                pc->setAngVelocity(0.0f);
                player_->setAngularDamping(settings.player.angularDamping);
            } else if (cutscene_ && !value) {
                player_->setLinearDamping(0.0f);
                player_->setAngularDamping(0.0f);
            }
        }
        cutscene_ = value;
        letterBox_->setActive(cutscene_, firstUpdate_);
        if (playable_ && settings.hud.enabled) {
            healthBarComponent_->setVisible(!value);
            staminaBarComponent_->setVisible(!value);
            heartsIndicator_->setVisible(!value);
            gemsIndicator_->setVisible(!value);
            inventoryDisplay_->setVisible(!value);
            upgradeDisplay_->setVisible(!value);
        }
    }

    float Scene::fixupAmmoAmount(float amount) const
    {
        if ((amount > 0.0f) && (upgradeCount(UpgradeIdMoreAmmo) > 0.0f)) {
            amount = std::floor(amount * 1.5f);
        }
        return amount;
    }

    void Scene::chainToNextLevel()
    {
        UInt32 gemsNeeded = 0;

        gemsNeeded += static_cast<UInt32>(upgrades_[UpgradeIdExtraLife]) * UpgradeManager::getCost(UpgradeIdExtraLife);

        for (int i = UpgradeIdMoreAmmo; i <= UpgradeIdMax; ++i) {
            UpgradeId upgrade = static_cast<UpgradeId>(i);
            if (upgrades_[upgrade] > 0.0f) {
                gemsNeeded += UpgradeManager::getCost(upgrade);
            }
        }

        userData.giveGems(gemsNeeded + player_->findComponent<PlayerComponent>()->numGems());

        for (int i = 0; i <= UpgradeIdMax; ++i) {
            upgradeManager.give(static_cast<UpgradeId>(i), upgrades_[i]);
        }
    }

    void Scene::setNextLevel(const std::string& scriptPath,
                             const std::string& assetPath)
    {
        nextScriptPath_ = scriptPath;
        nextAssetPath_ = assetPath;
    }

    bool Scene::getNextLevel(std::string& scriptPath,
                             std::string& assetPath)
    {
        if (nextScriptPath_.empty()) {
            return false;
        }

        scriptPath = nextScriptPath_;
        assetPath = nextAssetPath_;

        return true;
    }

    std::string Scene::basePath() const
    {
        std::string::size_type pos = scriptPath_.find_first_of('.');
        if (pos != std::string::npos) {
            return scriptPath_.substr(0, pos);
        } else {
            return scriptPath_;
        }
    }

    void Scene::spawnPowerup(const b2Vec2& pos)
    {
        SceneObjectPtr powerup;

        if (getRandom(0.0f, 1.0f) <= settings.powerupHealth.probability) {
            powerup = sceneObjectFactory.createPowerupHealth(settings.powerupHealth.timeout,
                settings.powerupHealth.amount);
        } else if (getRandom(0.0f, 1.0f) <= settings.powerupGem.probability) {
            powerup = sceneObjectFactory.createPowerupGem(settings.powerupGem.timeout);
        }

        if (powerup) {
            powerup->setPos(pos);

            addObject(powerup);
        }
    }

    void Scene::blast(const b2Vec2& p, float radius, float impulse, float damage,
        const SceneObjectTypes& types, UInt32 numRays, SceneObject* sourceObj)
    {
        static std::multimap<float, SceneObject*> objs;
        static std::map<SceneObject*, std::vector< std::pair<b2Vec2, b2Vec2> > > damagedObjs;

        damagedObjs.clear();

        objs.clear();

        queryPoint(p, boost::bind(&Scene::blastQueryPointCb, this, _1, sourceObj, boost::cref(types), boost::ref(objs)));

        for (std::multimap<float, SceneObject*>::const_iterator it = objs.begin();
             it != objs.end(); ++it) {
            if ((damagedObjs.count(it->second) == 0) &&
                (types[it->second->typeForBlast()] ||
                 (it->second->typeForBlast() == SceneObjectTypeGizmo) ||
                 (it->second->typeForBlast() == SceneObjectTypeGarbage))) {
                blastDamageObj(it->second, -damage * it->second->blastDamageMultiplier(), sourceObj);
            }

            b2Vec2 dir;

            if (it->second->pos() != p) {
                dir = it->second->pos() - p;
                dir.Normalize();
            } else {
                dir = b2Vec2(1.0f, 0.0f);
            }

            damagedObjs[it->second].push_back(std::make_pair(impulse * dir, p));
        }

        b2Rot rot((b2_pi * 2.0f) / numRays);
        b2Vec2 dir(1.0f, 0.0f);

        for (UInt32 i = 0; i < numRays; ++i) {
            b2Vec2 p2 = p + radius * dir;

            objs.clear();

            rayCast(p, p2, boost::bind(&Scene::blastRayCastCb, this,
                _1, _2, _3, _4, sourceObj, boost::cref(types), boost::ref(objs)));

            for (std::multimap<float, SceneObject*>::const_iterator it = objs.begin();
                 it != objs.end(); ++it) {
                b2Vec2 pt = it->first * p2 + (1.0f - it->first) * p;

                float factor = (1.0f - it->first) * (1.0f - it->first);

                if ((damagedObjs.count(it->second) == 0) &&
                    (types[it->second->typeForBlast()] ||
                     (it->second->typeForBlast() == SceneObjectTypeGizmo) ||
                     (it->second->typeForBlast() == SceneObjectTypeGarbage))) {
                    blastDamageObj(it->second, -damage * factor * it->second->blastDamageMultiplier(), sourceObj);
                }

                damagedObjs[it->second].push_back(std::make_pair(impulse * factor * dir, pt));
            }

            dir = b2Mul(rot, dir);
        }

        for (std::map<SceneObject*, std::vector< std::pair<b2Vec2, b2Vec2> > >::const_iterator it = damagedObjs.begin();
             it != damagedObjs.end(); ++it) {
            float factor = 1.0f / it->second.size();
            for (std::vector< std::pair<b2Vec2, b2Vec2> >::const_iterator jt = it->second.begin();
                 jt != it->second.end(); ++jt) {
                it->first->applyLinearImpulse(factor * jt->first, jt->second, true);
            }
        }
    }

    void Scene::blastDamageObj(SceneObject* obj, float value, SceneObject* sourceObj)
    {
        static SceneObjectTypes tmpTypes = SceneObjectTypes(SceneObjectTypeEnemy) |
            SceneObjectTypeEnemyBuilding;

        SceneObject* dr = NULL;
        bool alive = false;

        if (sourceObj && (sourceObj->type() == SceneObjectTypeGizmo)) {
            dr = obj->damageReceiver();
            alive = dr->alive();
        }

        obj->changeLife(value);

        if (alive && !dr->alive() && tmpTypes[dr->type()]) {
            gameShell->incAchievementProgress(AchievementEatThat, 1);
        }
    }

    void Scene::addCombatAlly(const SceneObjectPtr& ally)
    {
        if (std::find(combatAllies_.begin(), combatAllies_.end(), ally) != combatAllies_.end()) {
            return;
        }
        combatAllies_.push_back(ally);
    }

    void Scene::removeCombatAlly(const SceneObjectPtr& ally)
    {
        std::vector<SceneObjectPtr>::iterator it =
            std::find(combatAllies_.begin(), combatAllies_.end(), ally);
        if (it == combatAllies_.end()) {
            return;
        }
        combatAllies_.erase(it);
    }

    const std::set<SceneObjectPtr>& Scene::queryCombatTargets()
    {
        if (combatTargetsQueried_) {
            return combatTargets_;
        }

        combatTargets_.clear();

        CameraComponentPtr c = camera_->findComponent<CameraComponent>();

        queryAABB(c->getTrueAABB(),
            boost::bind(&Scene::combatTargetsQueryAABBCb, this, _1));

        combatTargetsQueried_ = true;
        return combatTargets_;
    }

    SceneInstancePtr Scene::script_instanciate(const std::string& assetPath,
        const b2Vec2& pos, float angle)
    {
        SceneInstancePtr res = boost::make_shared<SceneInstance>(pos, angle);

        SceneAssetPtr asset = assetManager.getSceneAsset(assetPath);

        asset->process(this, pos, angle);

        for (std::vector<SceneObject*>::iterator it = asset->objects().begin();
             it != asset->objects().end(); ++it) {
            if (*it) {
                res->objects().push_back((*it)->shared_from_this());
            }
        }

        for (std::vector<JointProxy*>::iterator it = asset->joints().begin();
             it != asset->joints().end(); ++it) {
            res->joints().push_back((*it)->sharedThis());
        }

        res->instances() = asset->instances();

        return res;
    }

    SceneInstancePtr Scene::script_instanciateByTransform(const std::string& assetPath,
        const b2Transform& xf)
    {
        return script_instanciate(assetPath, xf.p, xf.q.GetAngle());
    }

    std::vector<SceneInstancePtr> Scene::script_getInstances(const std::string& name) const
    {
        std::vector<SceneInstancePtr> res;

        for (Instances::const_iterator it = instances_.lower_bound(name);
             it != instances_.upper_bound(name); ++it) {
            res.push_back(it->second);
        }

        return res;
    }

    std::vector<SceneObjectPtr> Scene::script_queryObjects(const b2Vec2& center, float width, float height)
    {
        static std::set<SceneObjectPtr> tmp;

        b2AABB aabb;

        aabb.lowerBound = center - b2Vec2(width / 2.0f, height / 2.0f);
        aabb.upperBound = center + b2Vec2(width / 2.0f, height / 2.0f);

        queryAABB(aabb, boost::bind(&Scene::queryObjectsAABBCb, this, _1, boost::ref(tmp)));

        impl_->v_.renderComponentManager->queryObjects(aabb, tmp);

        std::vector<SceneObjectPtr> res;

        for (std::set<SceneObjectPtr>::const_iterator it = tmp.begin();
            it != tmp.end(); ++it) {
            res.push_back(*it);
        }

        tmp.clear();

        return res;
    }

    float Scene::blastRayCastCb(b2Fixture* fixture, const b2Vec2& point,
        const b2Vec2& normal, float fraction, SceneObject* sourceObj,
        const SceneObjectTypes& types,
        std::multimap<float, SceneObject*>& objs)
    {
        static SceneObjectTypes hitTypes = SceneObjectTypes(SceneObjectTypeRock) |
            SceneObjectTypeTerrain | SceneObjectTypeEnemyBuilding | SceneObjectTypeGizmo | SceneObjectTypeShield;

        if (fixture->IsSensor() || (SceneObject::fromFixture(fixture) == sourceObj)) {
            return -1;
        }

        if (types[SceneObject::fromFixture(fixture)->typeForBlast()] ||
            (SceneObject::fromFixture(fixture)->typeForBlast() == SceneObjectTypeGarbage) ||
            (SceneObject::fromFixture(fixture)->typeForBlast() == SceneObjectTypeVehicle)) {
            if ((fixture->GetFilterData().categoryBits & collisionBitEnemyRock) != 0) {
                objs.erase(objs.lower_bound(fraction), objs.end());
                if (fraction > 0.0f) {
                    objs.insert(std::make_pair(fraction, SceneObject::fromFixture(fixture)));
                }
                return fraction;
            } else {
                if (fraction > 0.0f) {
                    objs.insert(
                        std::make_pair(fraction, SceneObject::fromFixture(fixture)));
                }
                return -1;
            }
        } else if (hitTypes[SceneObject::fromFixture(fixture)->typeForBlast()]) {
            objs.erase(objs.lower_bound(fraction), objs.end());
            if ((fraction > 0.0f) &&
                ((SceneObject::fromFixture(fixture)->typeForBlast() == SceneObjectTypeRock) ||
                 (SceneObject::fromFixture(fixture)->typeForBlast() == SceneObjectTypeGizmo))) {
                objs.insert(std::make_pair(fraction, SceneObject::fromFixture(fixture)));
            }
            return fraction;
        } else {
            return -1;
        }
    }

    bool Scene::blastQueryPointCb(b2Fixture* fixture,
        SceneObject* sourceObj,
        const SceneObjectTypes& types,
        std::multimap<float, SceneObject*>& objs)
    {
        if (fixture->IsSensor() || (SceneObject::fromFixture(fixture) == sourceObj)) {
            return true;
        }

        if (types[SceneObject::fromFixture(fixture)->typeForBlast()]) {
            objs.insert(std::make_pair(0.0f, SceneObject::fromFixture(fixture)));
        } else if ((SceneObject::fromFixture(fixture)->typeForBlast() == SceneObjectTypeRock) ||
            (SceneObject::fromFixture(fixture)->typeForBlast() == SceneObjectTypeGizmo) ||
            (SceneObject::fromFixture(fixture)->typeForBlast() == SceneObjectTypeGarbage) ||
            (SceneObject::fromFixture(fixture)->typeForBlast() == SceneObjectTypeVehicle)) {
            objs.insert(std::make_pair(0.0f, SceneObject::fromFixture(fixture)));
        }

        return true;
    }

    void Scene::updateCombat()
    {
        bool erased;

        do {
            erased = false;
            for (std::vector<SceneObjectPtr>::iterator it = combatAllies_.begin();
                it != combatAllies_.end(); ++it) {
                if (!(*it)->scene()) {
                    combatAllies_.erase(it);
                    erased = true;
                    break;
                }
            }
        } while (erased);

        combatTargetsQueried_ = false;
    }

    bool Scene::combatTargetsQueryAABBCb(b2Fixture* fixture)
    {
        SceneObject* tmp = SceneObject::fromFixture(fixture);

        if (fixture->IsSensor()) {
            return true;
        }

        static SceneObjectTypes hitTypes = SceneObjectTypes(SceneObjectTypePlayer) |
            SceneObjectTypeAlly | SceneObjectTypeEnemy | SceneObjectTypeEnemyBuilding;

        if (hitTypes[tmp->type()]) {
            combatTargets_.insert(tmp->shared_from_this());
        }

        return true;
    }

    bool Scene::queryObjectsAABBCb(b2Fixture* fixture, std::set<SceneObjectPtr>& objs)
    {
        objs.insert(SceneObject::fromFixture(fixture)->shared_from_this());

        return true;
    }

    void Scene::freezeThawObjects(const b2AABB& aabb)
    {
        static std::vector<SceneObjectPtr> tmp;

        tmp.reserve(objects().size());

        for (std::set<SceneObjectPtr>::const_iterator it = objects().begin();
             it != objects().end();
             ++it ) {
            tmp.push_back(*it);
        }

        for (std::vector<SceneObjectPtr>::const_iterator it = tmp.begin();
             it != tmp.end();
             ++it ) {
            if ((*it)->frozen()) {
                if ((*it)->freezable()) {
                    /*
                     * Check if this object should be thawed.
                     */
                    b2Vec2 v((*it)->freezeRadius(), (*it)->freezeRadius());
                    b2AABB freezeAABB;
                    freezeAABB.lowerBound = (*it)->pos() - v;
                    freezeAABB.upperBound = (*it)->pos() + v;

                    if (b2TestOverlap(aabb, freezeAABB)) {
                        (*it)->thaw();
                    }
                } else {
                    /*
                     * This object doesn't want to be freezable anymore, thaw it.
                     */
                    (*it)->thaw();
                }
            } else if ((*it)->freezable()) {
                /*
                 * Check if this object should be frozen.
                 */
                b2Vec2 v((*it)->freezeRadius(), (*it)->freezeRadius());
                b2AABB freezeAABB;
                freezeAABB.lowerBound = (*it)->pos() - v;
                freezeAABB.upperBound = (*it)->pos() + v;

                if (!b2TestOverlap(aabb, freezeAABB)) {
                    (*it)->freeze();
                }
            }
        }

        tmp.clear();
    }

    void Scene::purchaseUpgrades()
    {
        UInt32 gemsNeeded = 0;

        gemsNeeded += static_cast<UInt32>(upgradeManager.count(UpgradeIdExtraLife)) * UpgradeManager::getCost(UpgradeIdExtraLife);

        for (int i = UpgradeIdMoreAmmo; i <= UpgradeIdMax; ++i) {
            UpgradeId upgrade = static_cast<UpgradeId>(i);
            if (upgradeManager.count(upgrade) > 0.0f) {
                gemsNeeded += UpgradeManager::getCost(upgrade);
            }
        }

        if (userData.numGems() < gemsNeeded) {
            upgradeManager.clear();
            return;
        }

        userData.takeGems(gemsNeeded);

        for (int i = 0; i <= UpgradeIdMax; ++i) {
            upgrades_[i] = upgradeManager.count(static_cast<UpgradeId>(i));
        }

        upgradeManager.clear();
    }

    SceneObjectPtr Scene::createPlayer()
    {
        SceneObjectPtr obj = sceneObjectFactory.createPlayer();

        if (upgradeCount(UpgradeIdArmor) > 0.0f) {
            obj->setBlastDamageMultiplier(2.0f / 3.0f);
            obj->setBulletDamageMultiplier(2.0f / 3.0f);
        }

        return obj;
    }
}
