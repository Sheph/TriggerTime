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

#ifndef _AF_RUBEOBJECT_H_
#define _AF_RUBEOBJECT_H_

#include "af/RUBEBody.h"
#include "af/RUBEJoint.h"
#include <boost/shared_ptr.hpp>

namespace af
{
    class RUBEObject;
    typedef boost::shared_ptr<RUBEObject> RUBEObjectPtr;

    class RUBEObject : public RUBEBase
    {
    public:
        typedef std::vector<int> Bodies;
        typedef std::vector<int> Joints;

        RUBEObject(const std::string& name,
                   const b2Vec2& pos,
                   float angle);
        ~RUBEObject();

        static RUBEObjectPtr fromJsonValue(const Json::Value& jsonValue);

        void addBody(int bodyIndex);

        void addJoint(int jointIndex);

        inline const b2Vec2& pos() const { return pos_; }
        inline float angle() const { return angle_; }
        inline const Bodies& bodies() const { return bodies_; }
        inline const Joints& joints() const { return joints_; }

    private:
        b2Vec2 pos_;
        float angle_;
        Bodies bodies_;
        Joints joints_;
    };
}

#endif
