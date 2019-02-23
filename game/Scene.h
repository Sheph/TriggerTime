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

#ifndef _SCENE_H_
#define _SCENE_H_

#include "SceneObjectManager.h"
#include "Component.h"
#include "JointProxy.h"
#include "RenderDamageComponent.h"
#include "RenderLightComponent.h"
#include "NumericIndicatorComponent.h"
#include "InventoryComponent.h"
#include "HealthBarComponent.h"
#include "StaminaBarComponent.h"
#include "LetterBoxComponent.h"
#include "CrosshairComponent.h"
#include "LRComponent.h"
#include "UpgradeDisplayComponent.h"
#include "SceneInstance.h"
#include "InputController.h"
#include "UpgradeManager.h"
#include "Statistics.h"
#include <boost/function.hpp>
#include <map>

namespace af
{
    class Scene : public SceneObjectManager
    {
    public:
        typedef boost::function<void(float)> TimerFn;
        typedef boost::function<float (b2Fixture*, const b2Vec2&,
            const b2Vec2&, float)> RayCastFn;
        typedef boost::function<bool (b2Fixture*)> QueryAABBFn;
        typedef boost::function<bool (b2Fixture*)> QueryPointFn;
        typedef std::multimap<std::string, SceneInstancePtr> Instances;

        Scene(float gameWidth, float gameHeight,
            const std::string& scriptPath, const std::string& assetPath);
        ~Scene();

        void prepare();

        void cleanup();

        inline float gameWidth() const { return gameWidth_; }
        inline float gameHeight() const { return gameHeight_; }

        void registerComponent(const ComponentPtr& component);

        void unregisterComponent(const ComponentPtr& component);

        void freezeComponent(const ComponentPtr& component);

        void thawComponent(const ComponentPtr& component);

        void update(float dt);

        JointProxyPtr addJoint(const b2JointDef* jointDef,
                               const std::string& name = "");

        JointProxyPtr addGearJoint(const SceneObjectPtr& objectA,
                                   const SceneObjectPtr& objectB,
                                   const JointProxyPtr& joint1,
                                   const JointProxyPtr& joint2,
                                   float ratio,
                                   bool collideConnected = false);

        WeldJointProxyPtr addWeldJoint(const SceneObjectPtr& objectA,
                                   const SceneObjectPtr& objectB,
                                   const b2Vec2& worldAnchor,
                                   bool collideConnected = false);

        JointProxyPtr addWeldJointEx(const SceneObjectPtr& objectA,
                                     const SceneObjectPtr& objectB,
                                     const b2Vec2& worldAnchor,
                                     float frequency,
                                     float dampingRatio,
                                     bool collideConnected = false);

        RevoluteJointProxyPtr addRevoluteJoint(const SceneObjectPtr& objectA,
                                       const SceneObjectPtr& objectB,
                                       const b2Vec2& localAnchorA,
                                       const b2Vec2& localAnchorB,
                                       bool collideConnected = false);

        RopeJointProxyPtr addRopeJoint(const SceneObjectPtr& objectA,
                                       const SceneObjectPtr& objectB,
                                       const b2Vec2& localAnchorA,
                                       const b2Vec2& localAnchorB,
                                       float maxLength,
                                       bool collideConnected = false);

        PrismaticJointProxyPtr addPrismaticJoint(const SceneObjectPtr& objectA,
                                        const SceneObjectPtr& objectB,
                                        const b2Vec2& localAnchorA,
                                        const b2Vec2& localAnchorB,
                                        const b2Vec2& localAxis,
                                        bool collideConnected = false);

        PrismaticJointProxyPtr addPrismaticJoint(const SceneObjectPtr& objectA,
                                        const SceneObjectPtr& objectB,
                                        const b2Vec2& localAnchorA,
                                        const b2Vec2& localAnchorB,
                                        float lowerLimit, float upperLimit,
                                        const b2Vec2& localAxis,
                                        bool collideConnected = false);

        PrismaticJointProxyPtr addPrismaticJoint(const SceneObjectPtr& objectA,
                                        const SceneObjectPtr& objectB,
                                        const b2Vec2& localAnchorA,
                                        const b2Vec2& localAnchorB,
                                        float lowerLimit, float upperLimit,
                                        const b2Vec2& localAxis,
                                        float referenceAngle,
                                        bool collideConnected = false);

        MotorJointProxyPtr addMotorJoint(const SceneObjectPtr& objectA,
                                         const SceneObjectPtr& objectB,
                                         const b2Vec2& linearOffset,
                                         float maxForce,
                                         float maxTorque,
                                         float correctionFactor,
                                         bool collideConnected = false);

        FrictionJointProxyPtr addFrictionJoint(const SceneObjectPtr& objectA,
                                               const SceneObjectPtr& objectB,
                                               const b2Vec2& localAnchorA,
                                               const b2Vec2& localAnchorB,
                                               float maxForce,
                                               float maxTorque,
                                               bool collideConnected = false);

        DistanceJointProxyPtr addDistanceJoint(const SceneObjectPtr& objectA,
                                               const SceneObjectPtr& objectB,
                                               const b2Vec2& localAnchorA,
                                               const b2Vec2& localAnchorB,
                                               float length,
                                               float frequency,
                                               float dampingRatio,
                                               bool collideConnected = false);

        void removeJoint(const JointProxyPtr& joint);

        std::vector<JointProxyPtr> getJoints(const std::string& name) const;

        SInt32 addTimer(const TimerFn& fn);

        void removeTimer(SInt32 cookie);

        void addInstances(const Instances& instances);

        void rayCast(const b2Vec2& p1, const b2Vec2& p2, const RayCastFn& fn);

        void queryAABB(const b2AABB& aabb, const QueryAABBFn& fn);

        void queryPoint(const b2Vec2& point, const QueryPointFn& fn);

        inline const InputControllerPtr& input() const { return input_; }

        inline InputContextUI* inputMenuUI() { return inputMenuUI_; }

        inline InputContextUI* inputGameUI() { return inputGameUI_; }

        inline InputContextPlayer* inputPlayer() { return inputPlayer_; }

        inline InputContextCutscene* inputCutscene() { return inputCutscene_; }

        inline InputContextRoboArm* inputRoboArm() { return inputRoboArm_; }

        inline InputContextBoat* inputBoat() { return inputBoat_; }

        inline InputContextMech* inputMech() { return inputMech_; }

        inline InputContextTurret* inputTurret() { return inputTurret_; }

        inline const SceneObjectPtr& camera() const { return camera_; }

        inline const SceneObjectPtr& player() const { return player_; }

        inline const RenderDamageComponentPtr& damage() const { return damageComponent_; }

        inline const RenderLightComponentPtr& lighting() const { return lightsComponent_; }

        inline const CrosshairComponentPtr& crosshair() const { return crosshair_; }

        inline bool playable() const { return playable_; }

        inline const b2Transform& respawnPoint() const { return respawnPoint_; }
        void setRespawnPoint(const b2Transform& value);

        inline void setPaused(bool value) { paused_ = value; }
        inline bool paused() const { return paused_; }

        inline bool cutscene() const { return cutscene_; }
        void setCutscene(bool value);

        inline bool quit() const { return quit_; }
        inline void setQuit(bool value) { quit_ = value; }

        inline float upgradeCount(UpgradeId upgrade) const { return upgrades_[upgrade]; }

        float fixupAmmoAmount(float amount) const;

        void chainToNextLevel();

        void setNextLevel(const std::string& scriptPath,
                          const std::string& assetPath);
        bool getNextLevel(std::string& scriptPath,
                          std::string& assetPath);

        inline const std::string& scriptPath() const { return scriptPath_; }
        inline const std::string& assetPath() const { return assetPath_; }
        std::string basePath() const;

        void spawnPowerup(const b2Vec2& pos);

        void blast(const b2Vec2& p, float radius, float impulse, float damage,
            const SceneObjectTypes& types, UInt32 numRays,
            SceneObject* sourceObj = NULL);

        const std::vector<SceneObjectPtr>& combatAllies() const { return combatAllies_; }
        void addCombatAlly(const SceneObjectPtr& ally);
        void removeCombatAlly(const SceneObjectPtr& ally);

        const std::set<SceneObjectPtr>& queryCombatTargets();

        inline const StatisticsPtr& stats() const { return stats_; }

        SceneInstancePtr script_instanciate(const std::string& assetPath,
            const b2Vec2& pos, float angle);

        SceneInstancePtr script_instanciateByTransform(const std::string& assetPath,
            const b2Transform& xf);

        std::vector<SceneInstancePtr> script_getInstances(const std::string& name) const;

        std::vector<SceneObjectPtr> script_queryObjects(const b2Vec2& center, float width, float height);

    private:
        enum InputMode
        {
            InputModeMenu = 0,
            InputModeGame,
            InputModeCutscene
        };

        void freezeThawObjects(const b2AABB& aabb);

        float blastRayCastCb(b2Fixture* fixture, const b2Vec2& point,
            const b2Vec2& normal, float fraction, SceneObject* sourceObj,
            const SceneObjectTypes& types,
            std::multimap<float, SceneObject*>& objs);

        bool blastQueryPointCb(b2Fixture* fixture,
            SceneObject* sourceObj,
            const SceneObjectTypes& types,
            std::multimap<float, SceneObject*>& objs);

        void blastDamageObj(SceneObject* obj, float value, SceneObject* sourceObj);

        void updateCombat();

        bool combatTargetsQueryAABBCb(b2Fixture* fixture);

        bool queryObjectsAABBCb(b2Fixture* fixture, std::set<SceneObjectPtr>& objs);

        void purchaseUpgrades();

        SceneObjectPtr createPlayer();

        class Impl;
        Impl* impl_;

        float gameWidth_;
        float gameHeight_;

        InputMode inputMode_;
        bool inputActive_[6];
        InputControllerPtr input_;
        InputContextUI* inputMenuUI_;
        InputContextUI* inputGameUI_;
        InputContextPlayer* inputPlayer_;
        InputContextCutscene* inputCutscene_;
        InputContextRoboArm* inputRoboArm_;
        InputContextBoat* inputBoat_;
        InputContextMech* inputMech_;
        InputContextTurret* inputTurret_;
        Instances instances_;
        SceneObjectPtr camera_;
        SceneObjectPtr player_;
        SceneObjectPtr healthBar_;
        SceneObjectPtr staminaBar_;
        SceneObjectPtr indicators_;
        SceneObjectPtr dummy_;
        LRComponentPtr lrComponent_;
        RenderLightComponentPtr lightsComponent_;
        RenderDamageComponentPtr damageComponent_;
        HealthBarComponentPtr healthBarComponent_;
        StaminaBarComponentPtr staminaBarComponent_;
        NumericIndicatorComponentPtr heartsIndicator_;
        NumericIndicatorComponentPtr gemsIndicator_;
        InventoryComponentPtr inventoryDisplay_;
        UpgradeDisplayComponentPtr upgradeDisplay_;
        LetterBoxComponentPtr letterBox_;
        CrosshairComponentPtr crosshair_;

        std::vector<SceneObjectPtr> combatAllies_;
        std::set<SceneObjectPtr> combatTargets_;
        bool combatTargetsQueried_;

        bool playable_;
        b2Transform respawnPoint_;

        bool paused_;
        bool cutscene_;
        bool quit_;
        bool cameraMoveStarted_;

        std::string nextScriptPath_;
        std::string nextAssetPath_;

        std::string scriptPath_;
        std::string assetPath_;

        bool gameover_;
        float gameoverTime_;

        bool firstUpdate_;

        float upgrades_[UpgradeIdMax + 1];

        StatisticsPtr stats_;
    };
}

#endif
