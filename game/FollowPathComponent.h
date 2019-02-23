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

#ifndef _FOLLOWPATHCOMPONENT_H_
#define _FOLLOWPATHCOMPONENT_H_

#include "PhasedComponent.h"
#include "Path.h"
#include "Tweening.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class FollowPathComponent : public boost::enable_shared_from_this<FollowPathComponent>,
                                public PhasedComponent
    {
    public:
        FollowPathComponent();
        ~FollowPathComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        bool finished() const;

        inline const b2Transform& pathTransform() const { return pathXf_; }
        inline void setPathTransform(const b2Transform& value) { pathXf_ = value; }

        inline const PathPtr& path() const { return path_; }
        void setPath(const PathPtr& value);

        inline const TweeningPtr& tweening() const { return tweening_; }
        void setTweening(const TweeningPtr& value);

        inline float time() const { return t_; }
        void setTime(float value);

    private:
        virtual void onRegister();

        virtual void onUnregister();

        void restart(float t = 0.0f);

        b2Transform pathXf_;
        PathPtr path_;
        TweeningPtr tweening_;

        float t_;
        float lastPos_;

        PathIteratorPtr lastIt_;
        PathIteratorPtr it_;
    };

    typedef boost::shared_ptr<FollowPathComponent> FollowPathComponentPtr;
}

#endif
