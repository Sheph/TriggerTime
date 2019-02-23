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

#include "af/RUBEImage.h"
#include <boost/make_shared.hpp>
#include <json/json.h>

namespace af
{
    RUBEImage::RUBEImage(const std::string& name,
                         int bodyIndex,
                         const b2Vec2& pos,
                         const std::string& fileName,
                         int zOrder,
                         float aspectScale,
                         float height,
                         float angle,
                         const Color& color,
                         bool flip)
    : RUBEBase(name),
      bodyIndex_(bodyIndex),
      pos_(pos),
      fileName_(fileName),
      zOrder_(zOrder),
      aspectScale_(aspectScale),
      height_(height),
      angle_(angle),
      color_(color),
      flip_(flip)
    {
    }

    RUBEImage::~RUBEImage()
    {
    }

    RUBEImagePtr RUBEImage::fromJsonValue(const Json::Value& jsonValue)
    {
        int bodyIndex = -1;

        if (jsonValue["body"].isInt()) {
            bodyIndex = jsonValue["body"].asInt();
        }

        b2Vec2 pos = json2vec2(jsonValue["center"]);

        std::string fileName = jsonValue["file"].asString();
        int zOrder = static_cast<int>(json2float(jsonValue["renderOrder"]));

        float aspectScale = json2float(jsonValue["aspectScale"]);
        float height = json2float(jsonValue["scale"]);
        float angle = json2float(jsonValue["angle"]);

        std::string name = jsonValue.get("name", "").asString();

        Color c(1.0f, 1.0f, 1.0f, 1.0f);

        if (jsonValue.isMember("colorTint")) {
            for (int i = 0; i < 4; i++) {
                if (jsonValue["colorTint"][i].isInt()) {
                    c.rgba[i] = static_cast<float>(jsonValue["colorTint"][i].asInt()) / 255.0f;
                }
            }
        }

        RUBEImagePtr image(new RUBEImage(name,
                                         bodyIndex,
                                         pos,
                                         fileName,
                                         zOrder,
                                         aspectScale,
                                         height,
                                         angle,
                                         c,
                                         jsonValue.get("flip", false).asBool()));

        image->setProps(jsonValue);

        return image;
    }
}
