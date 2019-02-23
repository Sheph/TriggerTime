#include "af/AudioDevice.h"
#include "af/AudioSound.h"
#include "af/AudioStream.h"
#include "af/Utils.h"
#include "Logger.h"
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>

namespace af
{
    AudioDevice::AudioDevice()
    : dev_(NULL), ctx_(NULL),
      nextCrossfadeCookie_(1),
      numStreamBuffers_(0),
      streamBufferSize_(0),
      shuttingDown_(false),
      soundVolume_(1.0f),
      musicVolume_(1.0f)
    {
    }

    AudioDevice::~AudioDevice()
    {
    }

    bool AudioDevice::init(UInt32 maxSources, UInt32 numStreamBuffers,
        UInt32 streamBufferSize)
    {
        assert(!dev_);

        oal.cMakeContextCurrent(NULL);

        dev_ = oal.cOpenDevice(NULL);

        if (!dev_) {
            LOG4CPLUS_ERROR(afutil::logger(), "Cannot open OpenAL device: " << oal.cGetError(dev_));
            return false;
        }

        LOG4CPLUS_INFO(afutil::logger(), "OpenAL device opened: " << oal.cGetString(dev_, ALC_ALL_DEVICES_SPECIFIER));

        ctx_ = oal.cCreateContext(dev_, NULL);

        if (!ctx_) {
            LOG4CPLUS_ERROR(afutil::logger(), "Cannot create OpenAL context: " << oal.cGetError(dev_));
            return false;
        }

        if (!oal.cMakeContextCurrent(ctx_)) {
            LOG4CPLUS_ERROR(afutil::logger(), "Cannot make OpenAL context current: " << oal.cGetError(dev_));
            return false;
        }

        LOG4CPLUS_INFO(afutil::logger(), "OpenAL info:");
        LOG4CPLUS_INFO(afutil::logger(), "Vendor: " << oal.GetString(AL_VENDOR));
        LOG4CPLUS_INFO(afutil::logger(), "Version: " << oal.GetString(AL_VERSION));

        ALCint maxMono = 0, maxStereo = 0;

        oal.cGetIntegerv(dev_, ALC_MONO_SOURCES, 1, &maxMono);
        oal.cGetIntegerv(dev_, ALC_STEREO_SOURCES, 1, &maxStereo);

        LOG4CPLUS_INFO(afutil::logger(), "Sources (mono) count: " << maxMono);
        LOG4CPLUS_INFO(afutil::logger(), "Sources (stereo) count: " << maxStereo);

        maxSources = (std::min)(static_cast<UInt32>(maxMono), maxSources);

        // Clear error.
        oal.GetError();

        UInt32 i;

        for (i = 0; i < maxSources; ++i) {
            ALuint id;
            oal.GenSources(1, &id);
            if (oal.GetError() != AL_NO_ERROR) {
                LOG4CPLUS_ERROR(afutil::logger(), "Cannot create " << i + 1 << "th source, using " << i << " sources");
                break;
            }
            availableSources_.insert(id);
        }

        if (i == maxSources) {
            LOG4CPLUS_INFO(afutil::logger(), "Using " << maxSources << " sources");
        }

        numStreamBuffers_ = numStreamBuffers;
        streamBufferSize_ = streamBufferSize;

        thread_ = boost::thread(boost::bind(&AudioDevice::streamUpdateThread,
            this));

        return true;
    }

    void AudioDevice::shutdown()
    {
        LOG4CPLUS_INFO(afutil::logger(), "Shutting down OpenAL...");

        shuttingDown_ = true;
        if (thread_.joinable()) {
            thread_.join();
        }

        for (ActiveSources::iterator it = activeSources_.begin();
             it != activeSources_.end(); ++it) {
            it->source->detach();
        }

        for (ActiveSources::iterator it = activeUpdateableSources_.begin();
             it != activeUpdateableSources_.end(); ++it) {
            it->source->detach();
        }

        oal.cMakeContextCurrent(NULL);
        oal.cDestroyContext(ctx_);
        oal.cCloseDevice(dev_);
        LOG4CPLUS_INFO(afutil::logger(), "OpenAL shut down");
    }

    void AudioDevice::suspend()
    {
        shuttingDown_ = true;
        if (thread_.joinable()) {
            thread_.join();
            LOG4CPLUS_INFO(afutil::logger(), "Suspending OpenAL...");
            oal.cSuspendContext(ctx_);
        }
    }

    void AudioDevice::resume()
    {
        if (!thread_.joinable()) {
            LOG4CPLUS_INFO(afutil::logger(), "Resuming OpenAL...");

            oal.cProcessContext(ctx_);
            shuttingDown_ = false;
            thread_ = boost::thread(boost::bind(&AudioDevice::streamUpdateThread,
                this));
        }
    }

    AudioSourceImplPtr AudioDevice::createSound(const AudioSoundDataPtr& audioData)
    {
        return boost::make_shared<AudioSound>(this, audioData);
    }

    AudioSourceImplPtr AudioDevice::createStream(const AudioStreamDataPtr& audioData)
    {
        return boost::make_shared<AudioStream>(this, audioData,
            numStreamBuffers_, streamBufferSize_);
    }

    void AudioDevice::crossfade(const AudioSourceImplPtr& src,
        const AudioSourceImplPtr& dest, float srcVelocity, float destVelocity,
        float crossPercentage)
    {
        boost::mutex::scoped_lock lock(mtx_);

        AudioSourceImplPtr a1 = src, a2 = dest;

        if (a1 && (a1->statusNoLock() != AudioSource::Playing)) {
            a1->setCrossfadeCookie(0);
            a1.reset();
        }

        if (a2 && (a2->statusNoLock() != AudioSource::Stopped)) {
            a2.reset();
        }

        if (a1) {
            a1->setCrossfadeCookie(nextCrossfadeCookie_);
        }

        if (a2) {
            a2->setCrossfadeCookie(nextCrossfadeCookie_);
        }

        if (a1 || a2) {
            Crossfade crossfade(a1, a2, srcVelocity, destVelocity, crossPercentage);

            if (a1 && ((a1->volume() * srcVelocity) > 0.0f)) {
                /*
                 * fixup srcT when second crossfade takes over.
                 */
                crossfade.srcT = (a1->volume() - a1->crossfadeVolume()) / (a1->volume() * srcVelocity);
            }

            crossfades_[nextCrossfadeCookie_] = crossfade;

            LOG4CPLUS_DEBUG(afutil::logger(), "Crossfade " << nextCrossfadeCookie_ << " started");

            ++nextCrossfadeCookie_;
        }
    }

    void AudioDevice::stopAll(float crossfadeTimeout)
    {
        /*
         * We need to capture and walk crossfades map because it may
         * contain sources that are not yet in active sources, but they also
         * need to be stopped.
         */

        Crossfades crossfades;

        {
            boost::mutex::scoped_lock lock(mtx_);

            crossfades = crossfades_;
        }

        for (Crossfades::iterator it = crossfades.begin();
             it != crossfades.end();
             ++it) {
            if (it->second.src) {
                if (crossfadeTimeout > 0.0f) {
                    crossfade(it->second.src, AudioSourceImplPtr(),
                        1.0f / crossfadeTimeout, 0.0f, 0.0f);
                } else {
                    it->second.src->stop();
                }
            }
            if (it->second.dest) {
                if (crossfadeTimeout > 0.0f) {
                    crossfade(it->second.dest, AudioSourceImplPtr(),
                        1.0f / crossfadeTimeout, 0.0f, 0.0f);
                } else {
                    it->second.dest->stop();
                }
            }
        }

        /*
         * Update thread does not modify sources list, thus, we can walk it
         * here without any locking.
         */

        for (ActiveSources::iterator it = activeUpdateableSources_.begin();
             it != activeUpdateableSources_.end(); ++it) {
            if (crossfadeTimeout > 0.0f) {
                crossfade(it->source, AudioSourceImplPtr(),
                    1.0f / crossfadeTimeout, 0.0f, 0.0f);
            } else {
                it->source->stop();
            }
        }

        for (ActiveSources::iterator it = activeSources_.begin();
             it != activeSources_.end(); ++it) {
            it->source->stop();
        }
    }

    void AudioDevice::setSoundVolume(float value)
    {
        boost::mutex::scoped_lock lock(mtx_);

        soundVolume_ = value;

        updateAllVolumes();
    }

    void AudioDevice::setMusicVolume(float value)
    {
        boost::mutex::scoped_lock lock(mtx_);

        musicVolume_ = value;

        updateAllVolumes();
    }

    ALuint AudioDevice::attach(const AudioSourceImplPtr& source, bool updateable)
    {
        ALuint id = 0;

        if (!availableSources_.empty()) {
            AvailableSources::iterator it = availableSources_.begin();
            id = *it;
            availableSources_.erase(it);
        } else {
            id = getFreeId(activeSources_);
            if (id == 0) {
                id = getFreeId(activeUpdateableSources_);
            }
        }

        if (id == 0) {
            /*
             * No free sources, preempt.
             */
            for (ActiveSources::iterator it = activeSources_.begin();
                 it != activeSources_.end(); ++it) {
                if ((it->source->crossfadeCookie() == 0) && !it->source->loop()) {
                    id = it->id;

                    it->source->stopNoLock();
                    it->source->detach();

                    oal.SourceRewind(id);
                    oal.Sourcei(id, AL_BUFFER, 0);
                    oal.Sourcei(id, AL_LOOPING, AL_FALSE);

                    activeSources_.erase(it);

                    break;
                }
            }
        }

        if (id != 0) {
            if (updateable) {
                activeUpdateableSources_.push_back(Attachment(source, id));
            } else {
                activeSources_.push_back(Attachment(source, id));
            }
        } else {
            LOG4CPLUS_WARN(afutil::logger(), "No free sources in audio device...");
        }

        return id;
    }

    ALuint AudioDevice::getFreeId(ActiveSources& sources)
    {
        for (ActiveSources::iterator it = sources.begin();
             it != sources.end(); ++it) {
            if (it->source->statusNoLock() == AudioSource::Stopped) {
                it->source->detach();
                ALuint id = it->id;

                oal.SourceRewind(id);
                oal.Sourcei(id, AL_BUFFER, 0);
                oal.Sourcei(id, AL_LOOPING, AL_FALSE);

                sources.erase(it);
                return id;
            }
        }

        return 0;
    }

    void AudioDevice::streamUpdateThread()
    {
        ActiveSources tmp;

        LOG4CPLUS_INFO(afutil::logger(), "Audio stream update thread started");

        UInt64 lastTimeUs = 0;

        while (!shuttingDown_) {
            UInt64 timeUs = getTimeUs();

            float dt = 0.0f;

            if (lastTimeUs != 0) {
                dt = static_cast<float>(timeUs - lastTimeUs) / 1000000.0f;
            }

            lastTimeUs = timeUs;

            {
                boost::mutex::scoped_lock lock(mtx_);

                tmp = activeUpdateableSources_;

                if (dt > FLT_EPSILON) {
                    processCrossfades(dt);
                }
            }

            for (ActiveSources::iterator it = tmp.begin();
                 it != tmp.end(); ++it) {
                it->source->update();
            }

            boost::this_thread::sleep(boost::posix_time::milliseconds(10));
        }

        LOG4CPLUS_INFO(afutil::logger(), "Audio stream update thread finished");
    }

    void AudioDevice::processCrossfades(float dt)
    {
        bool hadCrossfades = !crossfades_.empty();

        for (Crossfades::iterator it = crossfades_.begin(); it != crossfades_.end();) {
            AudioSourceImplPtr src = it->second.src;
            AudioSourceImplPtr dest = it->second.dest;

            if (src && (src->crossfadeCookie() != it->first)) {
                src.reset();
                it->second.src.reset();
            }

            if (dest && (dest->crossfadeCookie() != it->first)) {
                dest.reset();
                it->second.dest.reset();
            }

            float srcRatio = 0.0f;

            if (src) {
                it->second.srcT += dt;

                srcRatio = 1.0f - it->second.srcT * it->second.srcVelocity;

                float srcVolume = srcRatio * src->volume();

                if (srcVolume > 0.0f) {
                    src->crossfadeSetVolume(srcVolume);
                } else {
                    src->stopNoLock();
                    it->second.src.reset();
                }
            }

            if (dest && (srcRatio <= it->second.crossPercentage)) {
                it->second.destT += dt;

                float destVolume = it->second.destT * it->second.destVelocity * dest->volume();

                if (destVolume > dest->volume()) {
                    destVolume = dest->volume();
                }

                dest->crossfadeSetVolume(destVolume);

                if (!it->second.destStarted) {
                    dest->playNoLock(true, destVolume);
                    it->second.destStarted = true;
                }

                if (destVolume >= dest->volume()) {
                    dest->setCrossfadeCookie(0);
                    it->second.dest.reset();
                }
            }

            if (!it->second.src && !it->second.dest) {
                LOG4CPLUS_DEBUG(afutil::logger(), "Crossfade " << it->first << " processed");
                crossfades_.erase(it++);
            } else {
                ++it;
            }
        }

        if (hadCrossfades && crossfades_.empty()) {
            LOG4CPLUS_DEBUG(afutil::logger(), "Audio crossfades processed");
        }
    }

    void AudioDevice::updateAllVolumes()
    {
        for (ActiveSources::iterator it = activeUpdateableSources_.begin();
             it != activeUpdateableSources_.end(); ++it) {
            if (it->source->crossfadeCookie() == 0) {
                it->source->crossfadeSetVolume(it->source->volume());
            }
        }

        for (ActiveSources::iterator it = activeSources_.begin();
             it != activeSources_.end(); ++it) {
            if (it->source->crossfadeCookie() == 0) {
                it->source->crossfadeSetVolume(it->source->volume());
            }
        }
    }
}
