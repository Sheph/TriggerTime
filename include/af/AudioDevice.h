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

#ifndef _AF_AUDIODEVICE_H_
#define _AF_AUDIODEVICE_H_

#include "af/AudioSourceImpl.h"
#include "af/AudioData.h"
#include <boost/thread.hpp>
#include <set>
#include <list>

namespace af
{
    class AudioDevice : boost::noncopyable
    {
    public:
        AudioDevice();
        ~AudioDevice();

        bool init(UInt32 maxSources, UInt32 numStreamBuffers,
            UInt32 streamBufferSize);

        void shutdown();

        void suspend();

        void resume();

        AudioSourceImplPtr createSound(const AudioSoundDataPtr& audioData);

        AudioSourceImplPtr createStream(const AudioStreamDataPtr& audioData);

        void crossfade(const AudioSourceImplPtr& src,
            const AudioSourceImplPtr& dest, float srcVelocity,
            float destVelocity, float crossPercentage);

        void stopAll(float crossfadeTimeout);

        inline float soundVolume() const { return soundVolume_; }
        void setSoundVolume(float value);

        inline float musicVolume() const { return musicVolume_; }
        void setMusicVolume(float value);

        /*
         * Internal, do not call.
         * @{
         */

        inline boost::mutex& mtx() { return mtx_; }
        ALuint attach(const AudioSourceImplPtr& source, bool updateable);

        /*
         * @}
         */

    private:
        struct Attachment
        {
            Attachment(const AudioSourceImplPtr& source, ALuint id)
            : source(source), id(id)
            {
            }

            AudioSourceImplPtr source;
            ALuint id;
        };

        struct Crossfade
        {
            Crossfade()
            {
            }

            Crossfade(const AudioSourceImplPtr& src, const AudioSourceImplPtr& dest,
                float srcVelocity, float destVelocity, float crossPercentage)
            : src(src), dest(dest),
              srcVelocity(srcVelocity),
              destVelocity(destVelocity),
              crossPercentage(crossPercentage),
              srcT(0.0f), destT(0.0f),
              destStarted(false)
            {
            }

            AudioSourceImplPtr src;
            AudioSourceImplPtr dest;
            float srcVelocity;
            float destVelocity;
            float crossPercentage;
            float srcT;
            float destT;
            bool destStarted;
        };

        typedef std::set<ALuint> AvailableSources;
        typedef std::list<Attachment> ActiveSources;
        typedef std::map<UInt32, Crossfade> Crossfades;

        static ALuint getFreeId(ActiveSources& sources);

        void streamUpdateThread();

        void processCrossfades(float dt);

        void updateAllVolumes();

        boost::mutex mtx_;

        ALCdevice* dev_;
        ALCcontext* ctx_;
        AvailableSources availableSources_;
        ActiveSources activeSources_;
        ActiveSources activeUpdateableSources_;
        Crossfades crossfades_;
        UInt32 nextCrossfadeCookie_;

        UInt32 numStreamBuffers_;
        UInt32 streamBufferSize_;

        boost::thread thread_;
        bool shuttingDown_;

        float soundVolume_;
        float musicVolume_;
    };

    typedef boost::shared_ptr<AudioDevice> AudioDevicePtr;
}

#endif
