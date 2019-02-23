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

#include "FootageTrackComponent.h"
#include "SceneObject.h"
#include "Scene.h"
#include "SequentialTweening.h"
#include "AssetManager.h"
#include "PhysicsBodyComponent.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>

namespace af
{
    FootageTrackComponent::FootageTrackComponent(const SceneObjectPtr& target, const b2Vec2& infoPos, const std::string& infoImage, float infoImageHeight,
        const std::string& infoText, float infoTextSize, int zOrder)
    : PhasedComponent(phasePreRender),
      target_(target),
      infoPos_(infoPos),
      infoDrawable_(assetManager.getDrawable(infoImage)),
      infoImageHeight_(infoImageHeight),
      infoText_(infoText),
      infoTextSize_(infoTextSize),
      zOrder_(zOrder),
      aimMinScale_(1.0f),
      aimScale_(3.0f),
      aimTime_(0.5f),
      trackTime_(1.0f),
      transitionTime_(1.0f),
      imageTime_(0.5f),
      textTimeout_(0.01f),
      keepTime_(-1.0f),
      fadeTime_(-1.0f),
      trackColor_(0.0f, 1.0f, 0.0f, 0.5f),
      imageColor_(1.0f, 1.0f, 1.0f, 0.5f),
      textColor_(0.0f, 1.0f, 0.0f, 0.5f),
      state_(StateAim),
      t_(0.0f),
      t2_(0.0f)
    {
    }

    FootageTrackComponent::~FootageTrackComponent()
    {
    }

    void FootageTrackComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitPhasedComponent(shared_from_this());
    }

    void FootageTrackComponent::preRender(float dt)
    {
        t_ += dt;
        t2_ += dt;

        switch (state_) {
        case StateAim: {
            float v = tweening_->getValue(t_);
            rc_->setPos(parent()->getSmoothLocalPoint(target_->smoothPos()));
            rc_->setHeight((1.0f - v) * targetSize_ * aimScale_ + v * targetSize_ * aimMinScale_);

            Color c = trackColor_;
            c.rgba[3] = v * trackColor_.rgba[3];
            rc_->setColor(c);

            if (tweening_->finished(t_)) {
                state_ = StateTrack;
                t_ = 0.0f;
                break;
            }

            break;
        }
        case StateTrack: {
            rc_->setPos(parent()->getSmoothLocalPoint(target_->smoothPos()));

            if (t_ >= trackTime_) {
                state_ = StateTransition;
                t_ = 0.0f;

                targetCapturedPos_ = parent()->getSmoothLocalPoint(target_->smoothPos());
                tweening_ = boost::make_shared<SingleTweening>(transitionTime_, EaseOutQuad, 0.0f, 1.0f);

                break;
            }
            break;
        }
        case StateTransition: {
            float v = tweening_->getValue(t_);

            rc_->setPos((1.0f - v) * targetCapturedPos_ + v * infoPos_);

            float h = (1.0f - v) * targetSize_ * aimMinScale_ + v * infoImageHeight_;

            rc_->setSizes(h * infoDrawable_->image().aspect(), h);

            if (tweening_->finished(t_)) {
                state_ = StateImage;
                t_ = 0.0f;

                float h = rc_->height() * 0.935f;
                float w = rc_->drawable()->image().aspect() * h;

                rcInfo_ = boost::make_shared<RenderQuadComponent>(rc_->pos(), 0.0f,
                    w, h, infoDrawable_, zOrder_ - 2);
                rcInfo_->setColor(imageColor_);
                parent()->addComponent(rcInfo_);

                rcWhite_ = boost::make_shared<RenderQuadComponent>(rc_->pos(), 0.0f,
                    w, h, assetManager.getDrawable("dialog_bg.png"), zOrder_ - 1);
                parent()->addComponent(rcWhite_);

                tweening_ = boost::make_shared<SingleTweening>(transitionTime_, EaseOutQuad, 1.0f, 0.0f);

                break;
            }

            break;
        }
        case StateImage: {
            rcWhite_->setColor(Color(1.0f, 1.0f, 1.0f, tweening_->getValue(t_)));

            if (tweening_->finished(t_)) {
                state_ = StateText;
                t_ = 0.0f;
                t2_ = 0.0f;
                tweening_.reset();

                rcText_ = boost::make_shared<RenderTextComponent>(rc_->pos() + b2Vec2(rc_->width() / 2.0f + (infoTextSize_ / 2.0f), rc_->height() / 2.0f), 0.0f,
                    100.0f, infoText_, zOrder_);
                rcText_->setColor(textColor_);
                rcText_->setCharSize(infoTextSize_);
                rcText_->setNumLetters(0);
                parent()->addComponent(rcText_);

                break;
            }

            break;
        }
        case StateText: {
            if (tweening_) {
                float v = tweening_->getValue(t2_);

                Color c = trackColor_;
                c.rgba[3] = v * trackColor_.rgba[3];
                rc_->setColor(c);

                c = imageColor_;
                c.rgba[3] = v * imageColor_.rgba[3];
                rcInfo_->setColor(c);

                c = textColor_;
                c.rgba[3] = v * textColor_.rgba[3];
                rcText_->setColor(c);

                if (tweening_->finished(t2_)) {
                    removeFromParent();
                    return;
                }
            } else if ((keepTime_ >= 0.0f) && (t2_ >= keepTime_)) {
                tweening_ = boost::make_shared<SingleTweening>(fadeTime_, EaseOutQuad, 1.0f, 0.0f);
                t2_ = 0.0f;
                break;
            }
            if (t_ >= textTimeout_) {
                t_ = 0.0f;
                rcText_->setNumLetters(rcText_->numLetters() + 1);
            }
            break;
        }
        default:
            assert(false);
            break;
        }
    }

    void FootageTrackComponent::onRegister()
    {
        b2Transform xf;
        xf.SetIdentity();

        b2AABB aabb = target_->findComponent<PhysicsBodyComponent>()->computeAABB(xf);
        b2Vec2 tmp = 2.0f * aabb.GetExtents();
        targetSize_ = b2Max(tmp.x, tmp.y);

        tweening_ = boost::make_shared<SingleTweening>(aimTime_, EaseOutQuad, 0.0f, 1.0f);

        rc_ = boost::make_shared<RenderQuadComponent>(parent()->getSmoothLocalPoint(target_->smoothPos()), 0.0f,
            targetSize_ * aimScale_, targetSize_ * aimScale_, assetManager.getDrawable("subway1/cam_track1.png"), zOrder_);
        Color c = trackColor_;
        c.rgba[3] = 0.0f;
        rc_->setColor(c);
        parent()->addComponent(rc_);
    }

    void FootageTrackComponent::onUnregister()
    {
        target_.reset();
        if (rc_) {
            rc_->removeFromParent();
            rc_.reset();
        }
        if (rcInfo_) {
            rcInfo_->removeFromParent();
            rcInfo_.reset();
        }
        if (rcWhite_) {
            rcWhite_->removeFromParent();
            rcWhite_.reset();
        }
        if (rcText_) {
            rcText_->removeFromParent();
            rcText_.reset();
        }
    }
}
