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

#include "af/RUBEObject.h"
#include <boost/make_shared.hpp>
#include <json/json.h>

namespace af
{
    RUBEObject::RUBEObject(const std::string& name,
                           const b2Vec2& pos,
                           float angle)
    : RUBEBase(name),
      pos_(pos),
      angle_(angle)
    {
    }

    RUBEObject::~RUBEObject()
    {
    }

    RUBEObjectPtr RUBEObject::fromJsonValue(const Json::Value& jsonValue)
    {
        b2Vec2 pos = json2vec2(jsonValue["position"]);
        float angle = json2float(jsonValue["angle"]);
        std::string name = jsonValue.get("name", "").asString();

        RUBEObjectPtr obj = boost::make_shared<RUBEObject>(name,
                                                           pos,
                                                           angle);

        obj->setProps(jsonValue);

        return obj;
    }

    void RUBEObject::addBody(int bodyIndex)
    {
        bodies_.push_back(bodyIndex);
    }

    void RUBEObject::addJoint(int jointIndex)
    {
        joints_.push_back(jointIndex);
    }
}
