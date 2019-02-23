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

#include "AudioSourceDummy.h"

namespace af
{
    AudioSourceDummy::AudioSourceDummy()
    : pitch_(1.0f),
      pos_(b2Vec2_zero),
      relative_(false),
      volume_(1.0f),
      loop_(false),
      isMusic_(false),
      factor_(1.0f)
    {
    }

    AudioSourceDummy::~AudioSourceDummy()
    {
    }

    AudioSource::Status AudioSourceDummy::status() const
    {
        return AudioSource::Stopped;
    }

    void AudioSourceDummy::setPitch(float value)
    {
        pitch_ = value;
    }

    float AudioSourceDummy::pitch() const
    {
        return pitch_;
    }

    void AudioSourceDummy::setPos(const b2Vec2& value)
    {
        pos_ = value;
    }

    b2Vec2 AudioSourceDummy::pos() const
    {
        return pos_;
    }

    void AudioSourceDummy::setRelative(bool value)
    {
        relative_ = value;
    }

    bool AudioSourceDummy::relative() const
    {
        return relative_;
    }

    void AudioSourceDummy::setVolume(float value)
    {
        volume_ = value;
    }

    float AudioSourceDummy::volume() const
    {
        return volume_;
    }

    void AudioSourceDummy::setLoop(bool value)
    {
        loop_ = value;
    }

    bool AudioSourceDummy::loop() const
    {
        return loop_;
    }

    void AudioSourceDummy::setIsMusic(bool value)
    {
        isMusic_ = value;
    }

    bool AudioSourceDummy::isMusic() const
    {
        return isMusic_;
    }

    void AudioSourceDummy::setFactor(float value)
    {
        factor_ = value;
    }

    float AudioSourceDummy::factor() const
    {
        return factor_;
    }

    bool AudioSourceDummy::play()
    {
        return false;
    }

    void AudioSourceDummy::pause()
    {
    }

    void AudioSourceDummy::stop()
    {
    }
}
