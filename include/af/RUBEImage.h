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

#ifndef _AF_RUBEIMAGE_H_
#define _AF_RUBEIMAGE_H_

#include "af/RUBEBase.h"
#include <boost/shared_ptr.hpp>

namespace af
{
    class RUBEImage;
    typedef boost::shared_ptr<RUBEImage> RUBEImagePtr;

    class RUBEImage : public RUBEBase
    {
    public:
        RUBEImage(const std::string& name,
                  int bodyIndex,
                  const b2Vec2& pos,
                  const std::string& fileName,
                  int zOrder,
                  float aspectScale,
                  float height,
                  float angle,
                  const Color& color,
                  bool flip);
        ~RUBEImage();

        static RUBEImagePtr fromJsonValue(const Json::Value& jsonValue);

        inline int bodyIndex() const { return bodyIndex_; }
        inline const b2Vec2& pos() const { return pos_; }
        inline const std::string& fileName() const { return fileName_; }
        inline int zOrder() const { return zOrder_; }
        inline float aspectScale() const { return aspectScale_; }
        inline float height() const { return height_; }
        inline float angle() const { return angle_; }
        inline const Color& color() const { return color_; }
        inline bool flip() const { return flip_; }

    private:
        int bodyIndex_;
        b2Vec2 pos_;
        std::string fileName_;
        int zOrder_;
        float aspectScale_;
        float height_;
        float angle_;
        Color color_;
        bool flip_;
    };
}

#endif
