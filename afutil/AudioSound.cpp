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

#include "af/AudioSound.h"

namespace af
{
    AudioSound::AudioSound(AudioDevice* device, const AudioSoundDataPtr& data)
    : AudioSourceImpl(false, device),
      data_(data)
    {
    }

    AudioSound::~AudioSound()
    {
    }

    void AudioSound::update()
    {
    }

    void AudioSound::doSetLoop(bool value)
    {
        oal.Sourcei(source(), AL_LOOPING, value);
    }

    void AudioSound::doPlay(bool needPrepare)
    {
        if (needPrepare) {
            data_->incNumUsers();

            ALuint tmp = data_->buffer();
            oal.SourceQueueBuffers(source(), 1, &tmp);
        }

        oal.SourcePlay(source());
    }

    void AudioSound::doPause()
    {
        oal.SourcePause(source());
    }

    void AudioSound::doStop()
    {
        oal.SourceStop(source());
    }

    void AudioSound::doDetach()
    {
        data_->decNumUsers();
    }

    AudioSound::Status AudioSound::doStatus() const
    {
        ALint state = AL_INITIAL;

        oal.GetSourceiv(source(), AL_SOURCE_STATE, &state);

        switch (state) {
        case AL_PLAYING: return AudioSource::Playing;
        case AL_PAUSED: return AudioSource::Paused;
        default: return AudioSource::Stopped;
        }
    }
}
