#include "AudioManager.h"
#include "Settings.h"
#include "Platform.h"
#include "Logger.h"
#include "AssetManager.h"
#include "AudioSourceDummy.h"
#include "af/AudioOggFile.h"
#include "af/AudioListener.h"
#include <boost/make_shared.hpp>

namespace af
{
    AudioManager audio;

    template <>
    Single<AudioManager>* Single<AudioManager>::single = NULL;

    AudioManager::SoundAsset::SoundAsset(const std::string& name)
    : name_(name),
      numUsers_(0),
      buffer_(0)
    {
    }

    AudioManager::SoundAsset::~SoundAsset()
    {
        assert(numUsers_ == 0);
        invalidate();
    }

    void AudioManager::SoundAsset::incNumUsers()
    {
        if (buffer_ == 0) {
            assert(numUsers_ == 0);

            if (audio.soundCache_.size() >= audio.soundCacheSize_) {
                assert(audio.soundCache_.size() == audio.soundCacheSize_);

                for (SoundCache::iterator it = audio.soundCache_.begin();
                     it != audio.soundCache_.end(); ++it) {
                    if ((*it)->invalidate()) {
                        audio.soundCache_.erase(it);
                        break;
                    }
                }
            }

            oal.GenBuffers(1, &buffer_);

            AudioFilePtr audioFile =
                boost::make_shared<AudioOggFile>(new PlatformIFStream(name_), name_);

            audioFile->readIntoBuffer(buffer_, false);

            audio.soundCache_.push_back(this);
        }

        ++numUsers_;
    }

    ALuint AudioManager::SoundAsset::buffer()
    {
        return buffer_;
    }

    void AudioManager::SoundAsset::decNumUsers()
    {
        --numUsers_;
    }

    bool AudioManager::SoundAsset::invalidate()
    {
        if (numUsers_ != 0) {
            return false;
        }

        if (buffer_) {
            oal.DeleteBuffers(1, &buffer_);
            buffer_ = 0;
        }

        return true;
    }

    AudioManager::StreamAsset::StreamAsset(const std::string& name)
    : name_(name)
    {
    }

    AudioManager::StreamAsset::~StreamAsset()
    {
    }

    AudioFilePtr AudioManager::StreamAsset::openFile()
    {
        return boost::make_shared<AudioOggFile>(new PlatformIFStream(name_), name_);
    }

    AudioManager::AudioManager()
    {
    }

    AudioManager::~AudioManager()
    {
    }

    bool AudioManager::init()
    {
        soundCacheSize_ = settings.audio.soundCacheSize;

        if (settings.audio.enabled) {
            device_ = boost::make_shared<AudioDevice>();
            return device_->init(settings.audio.maxSources,
                settings.audio.numStreamBuffers,
                settings.audio.streamBufferSize);
        } else {
            return true;
        }
    }

    void AudioManager::shutdown()
    {
        if (device_) {
            device_->shutdown();
            device_.reset();
        }
    }

    void AudioManager::suspend()
    {
        if (device_) {
            device_->suspend();
        }
    }

    void AudioManager::resume()
    {
        if (device_) {
            device_->resume();
        }
    }

    void AudioManager::setPos(const b2Vec2& value)
    {
        if (device_) {
            AudioListener::setPos(value);
        }
    }

    b2Vec2 AudioManager::pos()
    {
        if (device_) {
            return AudioListener::pos();
        } else {
            return b2Vec2_zero;
        }
    }

    void AudioManager::setVolume(float value)
    {
        if (device_) {
            AudioListener::setVolume(value);
        }
    }

    float AudioManager::volume()
    {
        if (device_) {
            return AudioListener::volume();
        } else {
            return 1.0f;
        }
    }

    float AudioManager::soundVolume() const
    {
        if (device_) {
            return device_->soundVolume();
        } else {
            return 1.0f;
        }
    }

    void AudioManager::setSoundVolume(float value)
    {
        if (device_) {
            device_->setSoundVolume(value);
        }
    }

    float AudioManager::musicVolume() const
    {
        if (device_) {
            return device_->musicVolume();
        } else {
            return 1.0f;
        }
    }

    void AudioManager::setMusicVolume(float value)
    {
        if (device_) {
            device_->setMusicVolume(value);
        }
    }

    AudioSourcePtr AudioManager::createSound(const std::string& name)
    {
        assetManager.assetTouched(name);

        AudioSourcePtr sound;

        if (device_) {
            SoundMap::iterator it = soundMap_.find(name);

            if (it == soundMap_.end()) {
                it = soundMap_.insert(std::make_pair(name, boost::make_shared<SoundAsset>(name))).first;
            }

            sound = device_->createSound(it->second);
        } else {
            sound = boost::make_shared<AudioSourceDummy>();
        }

        std::map<std::string, float>::const_iterator it = settings.audio.factors.find(name);

        if (it != settings.audio.factors.end()) {
            sound->setFactor(it->second);
        }

        return sound;
    }

    void AudioManager::playSound(const std::string& name)
    {
        AudioSourcePtr sound = createSound(name);
        sound->play();
    }

    AudioSourcePtr AudioManager::createStream(const std::string& name)
    {
        assetManager.assetTouched(name);

        AudioSourcePtr stream;

        if (device_) {
            stream = device_->createStream(boost::make_shared<StreamAsset>(name));
        } else {
            stream = boost::make_shared<AudioSourceDummy>();
        }

        std::map<std::string, float>::const_iterator it = settings.audio.factors.find(name);

        if (it != settings.audio.factors.end()) {
            stream->setFactor(it->second);
        }

        return stream;
    }

    void AudioManager::crossfade(const AudioSourcePtr& src,
        const AudioSourcePtr& dest, float srcVelocity, float destVelocity,
        float crossPercentage)
    {
        if (device_) {
            device_->crossfade(boost::dynamic_pointer_cast<AudioSourceImpl>(src),
                boost::dynamic_pointer_cast<AudioSourceImpl>(dest), srcVelocity,
                destVelocity, crossPercentage);
        }
    }

    void AudioManager::stopAll(float crossfadeTimeout)
    {
        if (device_) {
            device_->stopAll(crossfadeTimeout);
        }
    }
}
