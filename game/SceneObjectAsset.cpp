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

#include "SceneObjectAsset.h"
#include "AssetManager.h"
#include "PhysicsBodyComponent.h"
#include "PhysicsJointComponent.h"
#include "RenderQuadComponent.h"
#include "RenderTentacleComponent.h"
#include "PointLight.h"
#include "ConeLight.h"
#include "LineLight.h"
#include "RingLight.h"
#include "LightComponent.h"
#include "Logger.h"
#include "Settings.h"
#include "Utils.h"
#include <boost/make_shared.hpp>
#include <log4cplus/ndc.h>

namespace af
{
    SceneObjectAsset::SceneObjectAsset(const RUBEWorldPtr& world)
    : world_(world)
    {
        log4cplus::NDCContextCreator ndc(world->name());

        rootBody_ = world_->body("root");

        if (!rootBody_) {
            LOG4CPLUS_ERROR(logger(), "\"root\" body not found");
            return;
        }

        root_ = boost::make_shared<SceneObject>();

        objects_.resize(world_->numBodies());

        for (int i = 0; i < world_->numBodies(); ++i) {
            RUBEBodyPtr body = world_->body(i);

            SceneObjectPtr obj;

            if (body != rootBody_) {
                obj = boost::make_shared<SceneObject>();
            } else {
                obj = root_;
                obj->setFreezeRadius(body->floatProp("freezeRadius"));
                obj->setFreezable(body->boolProp("freezable"));
            }

            obj->setFreezePhysics(body->boolProp("freezePhysics", true));
            obj->setDetourable(body->boolProp("detourable", true));
            obj->setGravityGunAware(body->boolProp("gravityGunAware"));

            obj->setCollisionImpulseMultiplier(
                body->floatProp("collisionImpulseMultiplier", 1.0f));

            obj->setBodyDef(body->bodyDef());

            {
                ComponentPtr component =
                    boost::make_shared<PhysicsBodyComponent>(body);

                obj->addComponent(component);
            }

            for (int j = 0; j < body->numFixtures(); ++j) {
                const RUBEFixturePtr& fixture = body->fixture(j);

                if (!fixture->haveProp("class")) {
                    continue;
                }

                const std::string& fixtureClass = fixture->stringProp("class");

                if (fixtureClass == "tentacle") {
                    if (RUBEPolygonFixturePtr polygonFixture =
                        boost::dynamic_pointer_cast<RUBEPolygonFixture>(fixture)) {
                        Tentacle& tentacle = tentacles_[polygonFixture->name()];
                        tentacle.parts[body->intProp("segId")] = TentaclePart(obj.get(), polygonFixture);
                        if (body->intProp("segId") == 0) {
                            tentacle.image = polygonFixture->stringProp("image");
                            tentacle.zOrder = polygonFixture->intProp("zorder");
                            tentacle.color = polygonFixture->colorProp("color");
                            tentacle.width1 = polygonFixture->floatProp("width1");
                            tentacle.width2 = polygonFixture->floatProp("width2");
                            tentacle.flip = polygonFixture->boolProp("image_flip");
                            tentacle.numIterations = polygonFixture->intProp("num_iterations");
                            tentacle.tension = polygonFixture->floatProp("tension");
                            tentacle.step = polygonFixture->floatProp("step");
                        }
                    } else if (RUBECircleFixturePtr circleFixture =
                        boost::dynamic_pointer_cast<RUBECircleFixture>(fixture)) {
                        Tentacle& tentacle = tentacles_[circleFixture->name()];
                        tentacle.parts[body->intProp("segId")] = TentaclePart(obj.get(), circleFixture);
                        if (body->intProp("segId") == 0) {
                            tentacle.image = circleFixture->stringProp("image");
                            tentacle.zOrder = circleFixture->intProp("zorder");
                            tentacle.color = circleFixture->colorProp("color");
                            tentacle.width1 = circleFixture->floatProp("width1");
                            tentacle.width2 = circleFixture->floatProp("width2");
                            tentacle.flip = circleFixture->boolProp("image_flip");
                            tentacle.numIterations = circleFixture->intProp("num_iterations");
                            tentacle.tension = circleFixture->floatProp("tension");
                            tentacle.step = circleFixture->floatProp("step");
                        }
                    } else {
                        LOG4CPLUS_ERROR(logger(), "Tentacles can only contain polygon or circle fixtures, skipping");
                    }
                }
            }

            for (int j = 0; j < body->numImages(); ++j) {
                RUBEImagePtr image = body->image(j);

                std::string peffect = image->stringProp("peffect");
                std::string light = image->stringProp("light");
                bool dummy = image->boolProp("dummy");

                if (!peffect.empty()) {
                    ParticleEffectComponentPtr component =
                        assetManager.getParticleEffect(peffect, image->pos(),
                                                       image->angle());

                    component->setZOrder(image->zOrder());

                    component->resetEmit();

                    obj->addComponent(component);
                } else if (!light.empty()) {
                    processLight(light, image, obj.get());
                } else if (dummy) {
                    /*
                     * Do nothing.
                     */
                } else {
                    DrawablePtr drawable = assetManager.getDrawable(image->fileName());

                    RenderQuadComponentPtr component =
                        boost::make_shared<RenderQuadComponent>(
                            image->pos(), image->angle(),
                            image->aspectScale() * image->height() * drawable->image().aspect(), image->height(),
                            drawable,
                            image->zOrder());

                    component->setName(image->name());
                    component->setColor(image->color());
                    component->setVisible(image->boolProp("visible", true));
                    component->setFixedPos(image->boolProp("fixedPos"));
                    component->setFlip(image->flip());

                    obj->addComponent(component);
                }
            }

            objects_[i] = obj.get();

            if (body != rootBody_) {
                obj->setName(body->name());

                root_->addObject(obj);
            }
        }

        if (world_->numJoints() > 0) {
            ComponentPtr component =
                boost::make_shared<PhysicsJointComponent>(world_, objects_);

            root_->addComponent(component);
        }

        processTentacles();
    }

    SceneObjectAsset::~SceneObjectAsset()
    {
    }

    SceneObjectPtr SceneObjectAsset::object(const std::string& bodyName) const
    {
        for (int i = 0; i < numObjects(); ++i) {
            if (world_->body(i)->name() == bodyName) {
                return objects_[i]->shared_from_this();
            }
        }

        return SceneObjectPtr();
    }

    void SceneObjectAsset::processLight(const std::string& type,
        const RUBEImagePtr& image, SceneObject* obj)
    {
        LightPtr light;

        if (type == "point") {
            PointLightPtr tmp = boost::make_shared<PointLight>(image->name());

            tmp->setDistance(image->floatProp("distance"));

            light = tmp;
        } else if (type == "cone") {
            ConeLightPtr tmp = boost::make_shared<ConeLight>(image->name());

            tmp->setNearDistance(image->floatProp("nearDistance"));
            tmp->setFarDistance(image->floatProp("distance"));
            tmp->setConeAngle(deg2rad(image->floatProp("coneAngle", 45.0f)));

            light = tmp;
        } else if (type == "line") {
            LineLightPtr tmp = boost::make_shared<LineLight>(image->name());

            tmp->setLength(image->floatProp("length"));
            tmp->setDistance(image->floatProp("distance"));
            tmp->setBothWays(image->boolProp("bothWays"));

            light = tmp;
        } else if (type == "ring") {
            RingLightPtr tmp = boost::make_shared<RingLight>(image->name());

            tmp->setDistance(image->floatProp("distance"));
            tmp->setNearDistance(image->floatProp("nearDistance"));
            tmp->setFarDistance(image->floatProp("farDistance"));

            light = tmp;
        } else {
            LOG4CPLUS_ERROR(logger(), "Unknown light type - \"" << type << "\", skipping");
            return;
        }

        light->setDiffuse(image->boolProp("diffuse"));
        light->setPos(image->pos());
        light->setAngle(image->angle());
        light->setNumRays(image->intProp("numRays", settings.light.numRays));
        light->setColor(image->color());
        light->setVisible(image->boolProp("visible", true));
        light->setSoftLength(image->floatProp("softLength", 2.5f));
        light->setXray(image->boolProp("xray", false));
        light->setDynamic(image->boolProp("dynamic", true));
        light->setReverse(image->boolProp("reverse"));
        light->setIntensity(image->floatProp("intensity", 1.0f));
        light->setNearOffset(image->floatProp("nearOffset"));
        light->setHitBlockers(image->boolProp("hitBlockers"));

        LightComponentPtr component = obj->findComponent<LightComponent>();

        if (!component) {
            component = boost::make_shared<LightComponent>();
            obj->addComponent(component);
        }

        component->attachLight(light);
    }

    void SceneObjectAsset::processTentacles()
    {
        if (tentacles_.empty()) {
            return;
        }

        std::vector<SceneObjectPtr> objs;
        std::vector<RUBEPolygonFixturePtr> fixtures;
        std::vector<RUBECircleFixturePtr> circleFixtures;

        for (TentacleMap::const_iterator it = tentacles_.begin(); it != tentacles_.end(); ++it) {
            objs.clear();
            fixtures.clear();
            circleFixtures.clear();

            for (std::map<int, TentaclePart>::const_iterator jt = it->second.parts.begin();
                jt != it->second.parts.end(); ++jt) {
                objs.push_back(jt->second.obj->shared_from_this());
                if (jt->second.fixture) {
                    fixtures.push_back(jt->second.fixture);
                } else {
                    circleFixtures.push_back(jt->second.circleFixture);
                }
            }

            RenderTentacleComponentPtr component;

            if (fixtures.empty()) {
                component = boost::make_shared<RenderTentacleComponent>(objs, circleFixtures,
                    assetManager.getDrawable(it->second.image,
                        Texture::WrapModeRepeat,
                        Texture::WrapModeClamp),
                    it->second.zOrder,
                    it->second.numIterations, it->second.tension,
                    it->second.step);
            } else {
                component = boost::make_shared<RenderTentacleComponent>(objs, fixtures,
                    assetManager.getDrawable(it->second.image,
                        Texture::WrapModeRepeat,
                        Texture::WrapModeClamp),
                    it->second.zOrder,
                    it->second.numIterations, it->second.tension,
                    it->second.step);
            }

            component->setFlip(it->second.flip);
            component->resetTimeline1(2);
            component->resetTimeline2(2);
            component->set1At(0, 0.0f, it->second.width1 / 2.0f);
            component->set1At(1, 1.0f, it->second.width2 / 2.0f);
            component->set2At(0, 0.0f, it->second.width1 / 2.0f);
            component->set2At(1, 1.0f, it->second.width2 / 2.0f);
            component->setColor(it->second.color);

            (*it->second.parts.begin()).second.obj->addComponent(component);
        }
    }
}
