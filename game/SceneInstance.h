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

#ifndef _SCENEINSTANCE_H_
#define _SCENEINSTANCE_H_

#include "SceneObject.h"
#include <map>

namespace af
{
    class SceneInstance;
    typedef boost::shared_ptr<SceneInstance> SceneInstancePtr;

    class SceneInstance : boost::noncopyable
    {
    public:
        SceneInstance(const b2Vec2& pos, float angle);
        ~SceneInstance();

        inline const b2Vec2& pos() const { return pos_; }

        inline float angle() const { return angle_; }

        inline const std::vector<SceneObjectPtr>& objects() const { return objects_; }
        inline std::vector<SceneObjectPtr>& objects() { return objects_; }

        inline const std::vector<JointProxyPtr>& joints() const { return joints_; }
        inline std::vector<JointProxyPtr>& joints() { return joints_; }

        inline const std::multimap<std::string, SceneInstancePtr>& instances() const { return instances_; }
        inline std::multimap<std::string, SceneInstancePtr>& instances() { return instances_; }

        std::vector<SceneInstancePtr> script_getInstances(const std::string& name) const;

    private:
        b2Vec2 pos_;
        float angle_;
        std::vector<SceneObjectPtr> objects_;
        std::vector<JointProxyPtr> joints_;
        std::multimap<std::string, SceneInstancePtr> instances_;
    };
}

#endif
