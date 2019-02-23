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

#include "RenderLightComponent.h"
#include "Renderer.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Settings.h"

namespace af
{
    RenderLightComponent::RenderLightComponent(int zOrder)
    : RenderComponent(zOrder, true),
      ambient_(settings.light.ambient),
      numBlur_(settings.light.numBlur),
      needLightsUpdate_(false)
    {
        setGammaCorrection(settings.light.gammaCorrection);
    }

    RenderLightComponent::~RenderLightComponent()
    {
    }

    void RenderLightComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitRenderComponent(shared_from_this());
    }

    void RenderLightComponent::update(float dt)
    {
        b2AABB prevAABB, aabb;
        b2Vec2 displacement;

        for (Lights::iterator it = lights_.begin(); it != lights_.end(); ++it) {
            if ((*it)->visible() && (*it)->updateAABB(prevAABB, aabb, displacement)) {
                manager()->moveAABB((*it)->cookie(), prevAABB, aabb, displacement);
                if (!(*it)->dynamic() || (*it)->xray()) {
                    (*it)->update();
                }
            }
        }

        needLightsUpdate_ = true;
    }

    void RenderLightComponent::render(void* const* parts, size_t numParts)
    {
        if (needLightsUpdate_) {
            needLightsUpdate_ = false;

            for (size_t i = 0; i < numParts; ++i) {
                Light* light = static_cast<Light*>(parts[i]);
                if (light && light->visible() && light->dynamic() && !light->xray()) {
                    light->update();
                }
            }
        }

        bool rendered = false;

        renderer.blendFunc(GL_SRC_ALPHA, GL_ONE);

        renderer.setProgramLight(true, gammaCorrection_);

        for (size_t i = 0; i < numParts; ++i) {
            Light* light = static_cast<Light*>(parts[i]);
            if (light && light->visible()) {
                light->render();
                rendered = true;
            }
        }

        renderer.blendFunc(GL_SRC_ALPHA, GL_ONE);

        renderer.setProgramLight(false, gammaCorrection_);

        for (size_t i = 0; i < numParts; ++i) {
            Light* light = static_cast<Light*>(parts[i]);
            if (light && light->visible() && !light->diffuse()) {
                light->render();
                rendered = true;
            }
        }

        if (blur() && rendered) {
            renderer.setProgramBlur(numBlur());

            RenderTriangleFan rop = renderer.renderTriangleFan();

            rop.addVertex(-1.0f, -1.0f);
            rop.addVertex(1.0f, -1.0f);
            rop.addVertex(1.0f, 1.0f);
            rop.addVertex(-1.0f, 1.0f);

            rop.addTexCoord(0.0f, 0.0f);
            rop.addTexCoord(1.0f, 0.0f);
            rop.addTexCoord(1.0f, 1.0f);
            rop.addTexCoord(0.0f, 1.0f);
        }

        {
            renderer.blendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

            renderer.setProgramShadow(Color(0.0, 0.0, 0.0, 0.0f));

            RenderTriangleFan rop = renderer.renderTriangleFan();

            rop.addVertex(-1.0f, -1.0f);
            rop.addVertex(1.0f, -1.0f);
            rop.addVertex(1.0f, 1.0f);
            rop.addVertex(-1.0f, 1.0f);

            rop.addTexCoord(0.0f, 0.0f);
            rop.addTexCoord(1.0f, 0.0f);
            rop.addTexCoord(1.0f, 1.0f);
            rop.addTexCoord(0.0f, 1.0f);
        }

        {
            renderer.blendFunc(GL_DST_COLOR, GL_ZERO);

            renderer.setProgramDiffuse(ambient_);

            RenderTriangleFan rop = renderer.renderTriangleFan();

            rop.addVertex(-1.0f, -1.0f);
            rop.addVertex(1.0f, -1.0f);
            rop.addVertex(1.0f, 1.0f);
            rop.addVertex(-1.0f, 1.0f);

            rop.addTexCoord(0.0f, 0.0f);
            rop.addTexCoord(1.0f, 0.0f);
            rop.addTexCoord(1.0f, 1.0f);
            rop.addTexCoord(0.0f, 1.0f);
        }

        renderer.blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void RenderLightComponent::setGammaCorrection(bool value)
    {
        gammaCorrection_ = value;
        gammaCorrectionParam_ = value ? 0.625f : 1.0f;
    }

    void RenderLightComponent::addLight(const LightPtr& light)
    {
        assert(!light->parent());

        if (light->parent()) {
            return;
        }

        SInt32 cookie = manager()->addAABB(this,
            light->adopt(this), light.get());

        light->setCookie(cookie);

        lights_.push_back(light);
    }

    void RenderLightComponent::removeLight(const LightPtr& light)
    {
        if (!light->parent()) {
            return;
        }

        assert(light->parent() == this);

        /*
         * Hold on to this light while removing.
         */
        LightPtr tmp = light;

        manager()->removeAABB(light->cookie());

        light->abandon();

        for (Lights::iterator it = lights_.begin(); it != lights_.end(); ++it) {
            if (*it == tmp) {
                lights_.erase(it);
                break;
            }
        }
    }

    void RenderLightComponent::onRegister()
    {
    }

    void RenderLightComponent::onUnregister()
    {
        while (!lights_.empty()) {
            lights_.back()->remove();
        }
    }
}
