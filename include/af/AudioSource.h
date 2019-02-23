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

#ifndef _AF_AUDIOSOURCE_H_
#define _AF_AUDIOSOURCE_H_

#include "af/Types.h"
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace af
{
    class AudioSource : boost::noncopyable
    {
    public:
        enum Status
        {
            Stopped = 0,
            Playing,
            Paused,
        };

        AudioSource() {}
        virtual ~AudioSource() {}

        virtual Status status() const = 0;

        virtual void setPitch(float value) = 0;
        virtual float pitch() const = 0;

        virtual void setPos(const b2Vec2& value) = 0;
        virtual b2Vec2 pos() const = 0;

        virtual void setRelative(bool value) = 0;
        virtual bool relative() const = 0;

        virtual void setVolume(float value) = 0;
        virtual float volume() const = 0;

        virtual void setLoop(bool value) = 0;
        virtual bool loop() const = 0;

        virtual void setIsMusic(bool value) = 0;
        virtual bool isMusic() const = 0;

        virtual void setFactor(float value) = 0;
        virtual float factor() const = 0;

        virtual bool play() = 0;

        virtual void pause() = 0;

        virtual void stop() = 0;
    };

    typedef boost::shared_ptr<AudioSource> AudioSourcePtr;
}

#endif
