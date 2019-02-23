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

#ifndef _AF_AUDIOSOUND_H_
#define _AF_AUDIOSOUND_H_

#include "af/AudioSourceImpl.h"
#include "af/AudioData.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class AudioSound : public boost::enable_shared_from_this<AudioSound>,
                       public AudioSourceImpl
    {
    public:
        AudioSound(AudioDevice* device, const AudioSoundDataPtr& data);
        ~AudioSound();

        virtual AudioSourceImplPtr sharedThis() { return shared_from_this(); }

        virtual void update();

    private:
        virtual void doSetLoop(bool value);

        virtual void doPlay(bool needPrepare);

        virtual void doPause();

        virtual void doStop();

        virtual void doDetach();

        virtual Status doStatus() const;

        AudioSoundDataPtr data_;
    };

    typedef boost::shared_ptr<AudioSound> AudioSoundPtr;
}

#endif
