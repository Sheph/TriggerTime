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

#include "PuzzleHintAreaComponent.h"
#include "CameraComponent.h"
#include "SceneObjectFactory.h"
#include "Scene.h"
#include "Settings.h"
#include "Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    PuzzleHintAreaComponent::PuzzleHintAreaComponent()
    : PhasedComponent(phasePreRender),
      triggered_(false),
      t_(0.0f),
      inside_(false),
      hintLastPos_(b2Vec2_zero)
    {
    }

    PuzzleHintAreaComponent::~PuzzleHintAreaComponent()
    {
    }

    void PuzzleHintAreaComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void PuzzleHintAreaComponent::preRender(float dt)
    {
        if (obj_ && obj_->dead()) {
            inside_ = false;
        }

        if (!triggered_) {
            if (!inside_) {
                return;
            }

            CameraComponentPtr cc = scene()->camera()->findComponent<CameraComponent>();

            for (HintTriggers::const_iterator it = hintTriggers_.begin();
                it != hintTriggers_.end(); ++it) {
                if (cc->entireRectVisible(it->first, it->second * 2.0f, it->second * 2.0f)) {
                    triggered_ = true;
                    break;
                }
            }

            return;
        }

        t_ -= dt;

        if (t_ >= 0.0f) {
            return;
        }

        for (Hints::iterator it = hints_.begin(); it != hints_.end();) {
            if (!it->obj) {
                it->obj = sceneObjectFactory.createPuzzleHint();
                it->obj->setPos(it->pos);
                scene()->addObject(it->obj);
            }

            if (it->removing) {
                it->alpha -= (settings.puzzleHint.alpha / settings.puzzleHint.timeout) * dt;
                if (it->alpha <= 0.0f) {
                    it->obj->removeFromParent();
                    it = hints_.erase(it);
                    continue;
                } else {
                    it->obj->setAlpha(it->alpha);
                }
            } else if (it->alpha < settings.puzzleHint.alpha) {
                it->alpha += (settings.puzzleHint.alpha / settings.puzzleHint.timeout) * dt;
                if (it->alpha >= settings.puzzleHint.alpha) {
                    it->alpha = settings.puzzleHint.alpha;
                }
                it->obj->setAlpha(it->alpha);
            }

            ++it;
        }

        if (inside_ && !indicator_) {
            indicator_ = boost::make_shared<PuzzleHintIndicatorComponent>(settings.puzzleHint.indicatorLength, -2);

            indicator_->setPadding(settings.puzzleHint.indicatorLength);
            indicator_->setColor(Color(1.0f, 1.0f, 1.0f, 0.0f));

            parent()->addComponent(indicator_);
        } else if (!inside_ && indicator_) {
            indicator_->setDirection(hintLastPos_ - obj_->smoothPos());
            if (fade(dt, false)) {
                indicator_->removeFromParent();
                indicator_.reset();
                obj_.reset();
            }
            return;
        }

        if (!indicator_) {
            return;
        }

        b2AABB aabb;

        if (!hints_.empty()) {
            aabb = scene()->camera()->findComponent<CameraComponent>()->getAABB();
        }

        const Hint* hint = NULL;
        float minDistSq = b2_maxFloat;

        for (Hints::const_iterator it = hints_.begin();
             it != hints_.end(); ++it) {
            b2Vec2 pos;
            if (it->removing) {
                continue;
            } else {
                pos = it->obj->pos();
            }
            bool visible = pointInRect(pos, aabb);
            if (visible) {
                hint = NULL;
                break;
            }

            float distSq = b2DistanceSquared(obj_->smoothPos(), pos);
            if (distSq <= minDistSq) {
                minDistSq = distSq;
                hint = &(*it);
            }
        }

        if (hint) {
            hintLastPos_ = hint->obj->pos();
        }

        fade(dt, hint != NULL);
        indicator_->setDirection(hintLastPos_ - obj_->smoothPos());
    }

    void PuzzleHintAreaComponent::sensorEnter(const SceneObjectPtr& other)
    {
        setObject(other);
    }

    void PuzzleHintAreaComponent::sensorExit(const SceneObjectPtr& other)
    {
        setObject(SceneObjectPtr());
    }

    void PuzzleHintAreaComponent::setObject(const SceneObjectPtr& value)
    {
        inside_ = !!value;
        if (value) {
            obj_ = value;
        }
    }

    void PuzzleHintAreaComponent::addHintTrigger(const b2Vec2& pos, float radius)
    {
        hintTriggers_.push_back(std::make_pair(pos, radius));
    }

    void PuzzleHintAreaComponent::addHint(const b2Vec2& pos)
    {
        removeHint(pos);
        hints_.push_back(Hint(pos));
    }

    void PuzzleHintAreaComponent::removeHint(const b2Vec2& pos)
    {
        for (Hints::iterator it = hints_.begin();
             it != hints_.end();) {
            if (it->pos == pos) {
                if (!it->obj) {
                    it = hints_.erase(it);
                    continue;
                } else {
                    it->removing = true;
                }
            }
            ++it;
        }
    }

    void PuzzleHintAreaComponent::removeAllHints()
    {
        for (Hints::iterator it = hints_.begin();
             it != hints_.end();) {
            if (!it->obj) {
                it = hints_.erase(it);
                continue;
            } else {
                it->removing = true;
            }
            ++it;
        }
    }

    void PuzzleHintAreaComponent::onRegister()
    {
    }

    void PuzzleHintAreaComponent::onUnregister()
    {
        obj_.reset();
        if (indicator_) {
            indicator_->removeFromParent();
            indicator_.reset();
        }
        for (Hints::iterator it = hints_.begin();
             it != hints_.end(); ++it) {
            if (it->obj) {
                it->obj->removeFromParent();
            }
        }
        hints_.clear();
    }

    bool PuzzleHintAreaComponent::fade(float dt, bool in)
    {
        Color c = indicator_->color();
        bool res = false;

        if (in) {
            c.rgba[3] += (settings.puzzleHint.alpha / settings.puzzleHint.timeout) * dt;
            if (c.rgba[3] > settings.puzzleHint.alpha) {
                c.rgba[3] = settings.puzzleHint.alpha;
                res = true;
            }
            indicator_->setColor(c);
        } else {
            c.rgba[3] -= (settings.puzzleHint.alpha / settings.puzzleHint.timeout) * dt;
            if (c.rgba[3] < 0.0f) {
                c.rgba[3] = 0.0f;
                res = true;
            }
            indicator_->setColor(c);
        }

        return res;
    }
}
