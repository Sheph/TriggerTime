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

#ifndef _AUDIOMANAGER_H_
#define _AUDIOMANAGER_H_

#include "af/Single.h"
#include "af/AudioDevice.h"
#include <map>
#include <list>

namespace af
{
    class AudioManager : public Single<AudioManager>
    {
    public:
        AudioManager();
        ~AudioManager();

        bool init();

        void shutdown();

        void suspend();

        void resume();

        void setPos(const b2Vec2& value);
        b2Vec2 pos();

        void setVolume(float value);
        float volume();

        float soundVolume() const;
        void setSoundVolume(float value);

        float musicVolume() const;
        void setMusicVolume(float value);

        AudioSourcePtr createSound(const std::string& name);
        void playSound(const std::string& name);

        AudioSourcePtr createStream(const std::string& name);

        void crossfade(const AudioSourcePtr& src, const AudioSourcePtr& dest,
            float srcVelocity, float destVelocity, float crossPercentage);

        void stopAll(float crossfadeTimeout = 0.0f);

    private:
        class SoundAsset : public AudioSoundData
        {
        public:
            explicit SoundAsset(const std::string& name);
            ~SoundAsset();

            virtual void incNumUsers();

            virtual ALuint buffer();

            virtual void decNumUsers();

            bool invalidate();

        private:
            std::string name_;
            UInt32 numUsers_;
            ALuint buffer_;
        };

        typedef boost::shared_ptr<SoundAsset> SoundAssetPtr;

        class StreamAsset : public AudioStreamData
        {
        public:
            explicit StreamAsset(const std::string& name);
            ~StreamAsset();

            virtual AudioFilePtr openFile();

        private:
            std::string name_;
        };

        typedef std::map<std::string, SoundAssetPtr> SoundMap;
        typedef std::list<SoundAsset*> SoundCache;

        AudioDevicePtr device_;
        SoundMap soundMap_;
        size_t soundCacheSize_;
        SoundCache soundCache_;
    };

    extern AudioManager audio;
}

#endif
