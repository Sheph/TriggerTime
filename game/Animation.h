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

#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include "Image.h"
#include <vector>

namespace af
{
    class Animation
    {
    public:
        explicit Animation(bool loop = false);
        ~Animation();

        static Animation fromStream(const std::string& fileName, std::istream& is);

        void addFrame(const Image& image, float duration);

        Image getFrame(float timeVal) const;

        int getFrameIndex(float timeVal) const;

        bool finished(float timeVal) const;

        inline int numFrames() const { return frames_.size(); }

        inline float duration() const { return duration_; }

        inline bool loop() const { return loop_; }
        inline void setLoop(bool value) { loop_ = value; }

        void addSpecialIndex(int index);
        int getSpecialIndex(int i) const;

        Animation clone(float factor) const;

    private:
        typedef std::pair<Image, float> Frame;
        typedef std::vector<Frame> Frames;
        typedef std::vector<int> Specials;

        Frames frames_;
        bool loop_;
        float duration_;
        Specials specials_;
    };
}

#endif
