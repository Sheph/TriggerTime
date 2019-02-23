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

#include "af/AudioSourceImpl.h"
#include "af/AudioDevice.h"

namespace af
{
    AudioSourceImpl::AudioSourceImpl(bool updateable, AudioDevice* device)
    : m_(device->mtx()),
      updateable_(updateable),
      device_(device),
      pitch_(1.0f),
      pos_(b2Vec2_zero),
      relative_(false),
      volume_(1.0f),
      loop_(false),
      isMusic_(false),
      factor_(1.0f),
      source_(0),
      crossfadeCookie_(0),
      crossfadeVolume_(1.0f)
    {
    }

    AudioSourceImpl::~AudioSourceImpl()
    {
    }

    AudioSource::Status AudioSourceImpl::statusNoLock () const
    {
        if (!source_) {
            return AudioSource::Stopped;
        }

        return doStatus();
    }

    void AudioSourceImpl::detach()
    {
        if (source_ == 0) {
            disableCrossfade();
            return;
        }

        doDetach();

        source_ = 0;

        disableCrossfade();
    }

    AudioSource::Status AudioSourceImpl::status() const
    {
        boost::mutex::scoped_lock lock(m_);

        if (!source_) {
            return AudioSource::Stopped;
        }

        return doStatus();
    }

    void AudioSourceImpl::setPitch(float value)
    {
        boost::mutex::scoped_lock lock(m_);

        pitch_ = value;

        if (!source_) {
            return;
        }

        oal.Sourcef(source_, AL_PITCH, value);
    }

    void AudioSourceImpl::setPos(const b2Vec2& value)
    {
        boost::mutex::scoped_lock lock(m_);

        pos_ = value;

        if (!source_) {
            return;
        }

        oal.Source3f(source_, AL_POSITION, value.x, 0.0f, value.y);
    }

    void AudioSourceImpl::setRelative(bool value)
    {
        boost::mutex::scoped_lock lock(m_);

        relative_ = value;

        if (!source_) {
            return;
        }

        oal.Sourcei(source_, AL_SOURCE_RELATIVE, value);
    }

    void AudioSourceImpl::setVolume(float value)
    {
        boost::mutex::scoped_lock lock(m_);

        volume_ = value;

        if (!source_) {
            return;
        }

        updateGain(value);
    }

    void AudioSourceImpl::setLoop(bool value)
    {
        boost::mutex::scoped_lock lock(m_);

        loop_ = value;

        if (!source_) {
            return;
        }

        doSetLoop(value);
    }

    bool AudioSourceImpl::play()
    {
        boost::mutex::scoped_lock lock(m_);

        return playNoLock(false, 0.0f);
    }

    void AudioSourceImpl::pause()
    {
        boost::mutex::scoped_lock lock(m_);

        disableCrossfade();

        if (!source_ || (doStatus() != Playing)) {
            return;
        }

        doPause();
    }

    void AudioSourceImpl::stop()
    {
        boost::mutex::scoped_lock lock(m_);

        stopNoLock();
    }

    bool AudioSourceImpl::playNoLock(bool crossfade, float crossfadeVolume)
    {
        bool newSource = false;

        if (!source_) {
            source_ = device_->attach(sharedThis(), updateable_);

            if (!source_) {
                if (!crossfade) {
                    disableCrossfade();
                }
                return false;
            }

            newSource = true;

            oal.Sourcef(source_, AL_PITCH, pitch_);
            oal.Source3f(source_, AL_POSITION, pos_.x, 0.0f, pos_.y);
            oal.Sourcei(source_, AL_SOURCE_RELATIVE, relative_);
            if (crossfade) {
                updateGain(crossfadeVolume);
            } else {
                updateGain(volume_);
            }
            doSetLoop(loop_);
        }

        doPlay(newSource);

        if (!crossfade) {
            disableCrossfade();
        }

        return true;
    }

    void AudioSourceImpl::stopNoLock()
    {
        disableCrossfade();

        if (!source_ || (doStatus() == Stopped)) {
            return;
        }

        doStop();
    }

    UInt32 AudioSourceImpl::crossfadeCookie() const
    {
        return crossfadeCookie_;
    }

    void AudioSourceImpl::setCrossfadeCookie(UInt32 cookie)
    {
        crossfadeCookie_ = cookie;
    }

    float AudioSourceImpl::crossfadeVolume() const
    {
        ALfloat value = 0.0f;

        if (source_) {
            value = crossfadeVolume_;
        }

        return value;
    }

    void AudioSourceImpl::crossfadeSetVolume(float value)
    {
        if (source_) {
            updateGain(value);
        }
    }

    void AudioSourceImpl::disableCrossfade()
    {
        crossfadeCookie_ = 0.0f;

        if (source_) {
            updateGain(volume_);
        }
    }

    void AudioSourceImpl::updateGain(float volume)
    {
        if (isMusic()) {
            oal.Sourcef(source_, AL_GAIN, volume * device_->musicVolume() * factor_);
        } else {
            oal.Sourcef(source_, AL_GAIN, volume * device_->soundVolume() * factor_);
        }
        crossfadeVolume_ = volume;
    }
}
