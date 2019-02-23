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

#ifndef _AF_AUDIOSOURCEIMPL_H_
#define _AF_AUDIOSOURCEIMPL_H_

#include "af/AudioSource.h"
#include "af/OAL.h"
#include <boost/thread/mutex.hpp>

namespace af
{
    class AudioDevice;
    class AudioSourceImpl;

    typedef boost::shared_ptr<AudioSourceImpl> AudioSourceImplPtr;

    class AudioSourceImpl : public AudioSource
    {
    public:
        AudioSourceImpl(bool updateable, AudioDevice* device);
        virtual ~AudioSourceImpl();

        virtual AudioSourceImplPtr sharedThis() = 0;

        virtual Status status() const;

        virtual void setPitch(float value);
        virtual float pitch() const { return pitch_; }

        virtual void setPos(const b2Vec2& value);
        virtual b2Vec2 pos() const { return pos_; }

        virtual void setRelative(bool value);
        virtual bool relative() const { return relative_; }

        virtual void setVolume(float value);
        virtual float volume() const { return volume_; }

        virtual void setLoop(bool value);
        virtual bool loop() const { return loop_; }

        virtual void setIsMusic(bool value) { isMusic_ = value; }
        virtual bool isMusic() const { return isMusic_; }

        virtual void setFactor(float value) { factor_ = value; }
        virtual float factor() const { return factor_; }

        virtual bool play();

        virtual void pause();

        virtual void stop();

        /*
         * Internal, do not call.
         * @{
         */

        bool playNoLock(bool crossfade, float crossfadeVolume);

        void stopNoLock();

        Status statusNoLock () const;

        void detach();

        UInt32 crossfadeCookie() const;
        void setCrossfadeCookie(UInt32 cookie);

        float crossfadeVolume() const;
        void crossfadeSetVolume(float value);

        /*
         * Called on separate thread, device lock not held.
         */
        virtual void update() = 0;

        /*
         * @}
         */

    protected:
        inline ALuint source() const { return source_; }

    private:
        virtual void doSetLoop(bool value) = 0;

        virtual void doPlay(bool needPrepare) = 0;

        virtual void doPause() = 0;

        virtual void doStop() = 0;

        virtual void doDetach() = 0;

        virtual Status doStatus() const = 0;

        void disableCrossfade();

        void updateGain(float volume);

        boost::mutex& m_;

        bool updateable_;
        AudioDevice* device_;

        float pitch_;
        b2Vec2 pos_;
        bool relative_;
        float volume_;
        bool loop_;
        bool isMusic_;
        float factor_;

        ALuint source_;

        UInt32 crossfadeCookie_;
        float crossfadeVolume_;
    };
}

#endif
