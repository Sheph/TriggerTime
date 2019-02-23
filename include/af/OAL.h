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

#ifndef _AF_OAL_H_
#define _AF_OAL_H_

#include "af/Single.h"
#include <AL/al.h>
#include <AL/alc.h>

namespace af
{
    class OAL : public Single<OAL>
    {
    public:
        OAL();
        ~OAL();

        ALCdevice* (ALC_APIENTRY* cOpenDevice)(const ALCchar* devicename);
        ALCboolean (ALC_APIENTRY* cCloseDevice)(ALCdevice* device);
        ALCcontext* (ALC_APIENTRY* cCreateContext)(ALCdevice* device, const ALCint* attrlist);
        ALCboolean (ALC_APIENTRY* cMakeContextCurrent)(ALCcontext* context);
        void (ALC_APIENTRY* cDestroyContext)(ALCcontext* context);
        void (ALC_APIENTRY* cGetIntegerv)(ALCdevice* device, ALCenum param, ALCsizei size, ALCint* values);
        ALCenum (ALC_APIENTRY* cGetError)(ALCdevice* device);
        const ALCchar* (ALC_APIENTRY* cGetString)(ALCdevice* device, ALCenum param);
        void (ALC_APIENTRY* cSuspendContext)(ALCcontext* context);
        void (ALC_APIENTRY* cProcessContext)(ALCcontext* context);

        const ALchar* (AL_APIENTRY* GetString)(ALenum param);
        void (AL_APIENTRY* BufferData)(ALuint buffer, ALenum format, const ALvoid* data, ALsizei size, ALsizei freq);
        void (AL_APIENTRY* GenBuffers)(ALsizei n, ALuint* buffers);
        void (AL_APIENTRY* DeleteBuffers)(ALsizei n, const ALuint* buffers);
        void (AL_APIENTRY* Sourcef)(ALuint source, ALenum param, ALfloat value);
        void (AL_APIENTRY* Source3f)(ALuint source, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3);
        void (AL_APIENTRY* GetSourceiv)(ALuint source,  ALenum param, ALint* values);
        void (AL_APIENTRY* GetSourcefv)(ALuint source, ALenum param, ALfloat* values);
        void (AL_APIENTRY* Sourcei)(ALuint source, ALenum param, ALint value);
        void (AL_APIENTRY* SourcePlay)(ALuint source);
        void (AL_APIENTRY* SourcePause)(ALuint source);
        void (AL_APIENTRY* SourceStop)(ALuint source);
        void (AL_APIENTRY* SourceQueueBuffers)(ALuint source, ALsizei nb, const ALuint* buffers);
        void (AL_APIENTRY* GenSources)(ALsizei n, ALuint* sources);
        void (AL_APIENTRY* DeleteSources)(ALsizei n, const ALuint* sources);
        ALenum (AL_APIENTRY* GetError)(void);
        void (AL_APIENTRY* SourceRewind)(ALuint source);
        void (AL_APIENTRY* Listenerf)(ALenum param, ALfloat value);
        void (AL_APIENTRY* GetListenerfv)(ALenum param, ALfloat* values);
        void (AL_APIENTRY* Listener3f)(ALenum param, ALfloat value1, ALfloat value2, ALfloat value3);
        void (AL_APIENTRY* SourceUnqueueBuffers)(ALuint source, ALsizei nb, ALuint* buffers);
    };

    extern OAL oal;
}

#endif
