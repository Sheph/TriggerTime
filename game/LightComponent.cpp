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

#include "LightComponent.h"
#include "SceneObject.h"
#include "Scene.h"

namespace af
{
    LightComponent::LightComponent()
    : PhasedComponent(phasePreRender)
    {
    }

    LightComponent::~LightComponent()
    {
    }

    void LightComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void LightComponent::preRender(float dt)
    {
        for (Lights::const_iterator it = lights_.begin(); it != lights_.end(); ++it) {
            (*it)->setWorldTransform(parent()->getSmoothTransform());
        }
    }

    void LightComponent::attachLight(const LightPtr& light)
    {
        lights_.push_back(light);

        if (scene() && !light->active()) {
            light->setWorldTransform(parent()->getSmoothTransform());
            scene()->lighting()->addLight(light);
        }
    }

    void LightComponent::detachLight(const LightPtr& light)
    {
        LightPtr tmp = light;

        for (Lights::iterator it = lights_.begin(); it != lights_.end(); ++it) {
            if (*it == tmp) {
                lights_.erase(it);
                break;
            }
        }
    }

    LightComponent::Lights LightComponent::getLights(const std::string& name) const
    {
        Lights res;

        for (Lights::const_iterator it = lights_.begin(); it != lights_.end(); ++it) {
            if ((*it)->name() == name) {
                res.push_back(*it);
            }
        }

        return res;
    }

    void LightComponent::setDirty()
    {
        for (Lights::iterator it = lights_.begin(); it != lights_.end(); ++it) {
            (*it)->setDirty();
        }
    }

    void LightComponent::onRegister()
    {
        for (Lights::const_iterator it = lights_.begin(); it != lights_.end(); ++it) {
            if (!(*it)->active()) {
                (*it)->setWorldTransform(parent()->getSmoothTransform());
                scene()->lighting()->addLight(*it);
            }
        }
    }

    void LightComponent::onUnregister()
    {
        for (Lights::const_iterator it = lights_.begin(); it != lights_.end(); ++it) {
            (*it)->remove();
        }
    }
}
