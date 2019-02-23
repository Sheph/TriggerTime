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

#include "Logger.h"
#include "Game.h"
#include "OGL.h"
#include "af/OAL.h"
#include "Settings.h"
#include "PlatformAndroid.h"
#include "af/AndroidAppender.h"
#include "af/StreamAppConfig.h"
#include <log4cplus/configurator.h>
#include <log4cplus/spi/factory.h>
#include <boost/make_shared.hpp>
#include <boost/thread/mutex.hpp>
#include <jni.h>
#include <android/log.h>
#include <android/asset_manager_jni.h>

#define GL_GET_PROC(func, sym) af::ogl.func = &sym

#define AL_GET_PROC(func, sym) af::oal.func = &sym

#define AF_TAG "airforce"

struct InputEvent
{
    InputEvent(af::KeyCode keyCode, bool up)
    : keyCode(keyCode),
      type(up ? 1 : 0)
    {
    }

    InputEvent(int finger, const b2Vec2& point)
    : finger(finger),
      point(point),
      type(2)
    {
    }

    InputEvent(int finger)
    : finger(finger),
      type(3)
    {
    }

    int type;
    af::KeyCode keyCode;
    int finger;
    b2Vec2 point;
};

static af::Game game;
static bool JNIInitializeCalled = false;
static bool gameInitializeCalled = false;
static bool gameInitialized = false;
static int menuPressed = 0;
static boost::mutex inputMtx;
static std::vector<InputEvent> inputEvents;

static void OGLInit()
{
    GL_GET_PROC(GenTextures, glGenTextures);
    GL_GET_PROC(DeleteTextures, glDeleteTextures);
    GL_GET_PROC(BindTexture, glBindTexture);
    GL_GET_PROC(TexImage2D, glTexImage2D);
    GL_GET_PROC(TexParameteri, glTexParameteri);
    GL_GET_PROC(ClearColor, glClearColor);
    GL_GET_PROC(Clear, glClear);
    GL_GET_PROC(Viewport, glViewport);
    GL_GET_PROC(AttachShader, glAttachShader);
    GL_GET_PROC(BindAttribLocation, glBindAttribLocation);
    GL_GET_PROC(CompileShader, glCompileShader);
    GL_GET_PROC(CreateProgram, glCreateProgram);
    GL_GET_PROC(CreateShader, glCreateShader);
    GL_GET_PROC(DeleteProgram, glDeleteProgram);
    GL_GET_PROC(DeleteShader, glDeleteShader);
    GL_GET_PROC(DetachShader, glDetachShader);
    GL_GET_PROC(DisableVertexAttribArray, glDisableVertexAttribArray);
    GL_GET_PROC(EnableVertexAttribArray, glEnableVertexAttribArray);
    GL_GET_PROC(LinkProgram, glLinkProgram);
    GL_GET_PROC(ShaderSource, glShaderSource);
    GL_GET_PROC(UseProgram, glUseProgram);
    GL_GET_PROC(GetProgramiv, glGetProgramiv);
    GL_GET_PROC(GetProgramInfoLog, glGetProgramInfoLog);
    GL_GET_PROC(GetShaderiv, glGetShaderiv);
    GL_GET_PROC(GetShaderInfoLog, glGetShaderInfoLog);
    GL_GET_PROC(GetAttribLocation, glGetAttribLocation);
    GL_GET_PROC(GetUniformLocation, glGetUniformLocation);
    GL_GET_PROC(VertexAttribPointer, glVertexAttribPointer);
    GL_GET_PROC(DrawArrays, glDrawArrays);
    GL_GET_PROC(Uniform4fv, glUniform4fv);
    GL_GET_PROC(Enable, glEnable);
    GL_GET_PROC(Disable, glDisable);
    GL_GET_PROC(BlendFunc, glBlendFunc);
    GL_GET_PROC(UniformMatrix4fv, glUniformMatrix4fv);
    GL_GET_PROC(Uniform1i, glUniform1i);
    GL_GET_PROC(GetIntegerv, glGetIntegerv);
    GL_GET_PROC(GenerateMipmap, glGenerateMipmap);
    af::ogl.PointSize = NULL;
    GL_GET_PROC(LineWidth, glLineWidth);
    GL_GET_PROC(GenFramebuffers, glGenFramebuffers);
    GL_GET_PROC(DeleteFramebuffers, glDeleteFramebuffers);
    GL_GET_PROC(BindFramebuffer, glBindFramebuffer);
    GL_GET_PROC(FramebufferTexture2D, glFramebufferTexture2D);
    GL_GET_PROC(CheckFramebufferStatus, glCheckFramebufferStatus);
    GL_GET_PROC(Uniform1f, glUniform1f);
    GL_GET_PROC(Uniform2f, glUniform2f);
    GL_GET_PROC(ColorMask, glColorMask);
    GL_GET_PROC(StencilFunc, glStencilFunc);
    GL_GET_PROC(StencilOp, glStencilOp);
    GL_GET_PROC(GetString, glGetString);
    GL_GET_PROC(Scissor, glScissor);
}

static void OALInit()
{
    AL_GET_PROC(cOpenDevice, alcOpenDevice);
    AL_GET_PROC(cCloseDevice, alcCloseDevice);
    AL_GET_PROC(cCreateContext, alcCreateContext);
    AL_GET_PROC(cMakeContextCurrent, alcMakeContextCurrent);
    AL_GET_PROC(cDestroyContext, alcDestroyContext);
    AL_GET_PROC(cGetIntegerv, alcGetIntegerv);
    AL_GET_PROC(cGetError, alcGetError);
    AL_GET_PROC(cGetString, alcGetString);
    AL_GET_PROC(cSuspendContext, alcSuspendContext);
    AL_GET_PROC(cProcessContext, alcProcessContext);

    AL_GET_PROC(GetString, alGetString);
    AL_GET_PROC(BufferData, alBufferData);
    AL_GET_PROC(GenBuffers, alGenBuffers);
    AL_GET_PROC(DeleteBuffers, alDeleteBuffers);
    AL_GET_PROC(Sourcef, alSourcef);
    AL_GET_PROC(Source3f, alSource3f);
    AL_GET_PROC(GetSourceiv, alGetSourceiv);
    AL_GET_PROC(GetSourcefv, alGetSourcefv);
    AL_GET_PROC(Sourcei, alSourcei);
    AL_GET_PROC(SourcePlay, alSourcePlay);
    AL_GET_PROC(SourcePause, alSourcePause);
    AL_GET_PROC(SourceStop, alSourceStop);
    AL_GET_PROC(SourceQueueBuffers, alSourceQueueBuffers);
    AL_GET_PROC(GenSources, alGenSources);
    AL_GET_PROC(DeleteSources, alDeleteSources);
    AL_GET_PROC(GetError, alGetError);
    AL_GET_PROC(SourceRewind, alSourceRewind);
    AL_GET_PROC(Listenerf, alListenerf);
    AL_GET_PROC(GetListenerfv, alGetListenerfv);
    AL_GET_PROC(Listener3f, alListener3f);
    AL_GET_PROC(SourceUnqueueBuffers, alSourceUnqueueBuffers);
}

extern "C"
{
    JNIEXPORT jboolean JNICALL Java_com_airforce_AirForceJNILib_init(JNIEnv* env, jobject obj, jobject assetManager)
    {
        if (JNIInitializeCalled) {
            return gameInitialized;
        }

        JNIInitializeCalled = true;

        log4cplus::spi::AppenderFactoryRegistry& reg
            = log4cplus::spi::getAppenderFactoryRegistry();
        LOG4CPLUS_REG_APPENDER(reg, AndroidAppender);

        srand(static_cast<unsigned int>(time(NULL)));

        AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
        assert(mgr != NULL);

        if (!platformAndroid->init(mgr)) {
            __android_log_print(ANDROID_LOG_ERROR, AF_TAG, "Cannot init android platform");
            return JNI_FALSE;
        }

        af::PlatformIFStream is("config_android.ini");

        if (!is) {
            __android_log_print(ANDROID_LOG_ERROR, AF_TAG, "Cannot find config_android.ini");
            return JNI_FALSE;
        }

        boost::shared_ptr<af::StreamAppConfig> appConfig =
            boost::make_shared<af::StreamAppConfig>();

        if (!appConfig->load(is)) {
            __android_log_print(ANDROID_LOG_ERROR, AF_TAG, "Cannot read config_android.ini");
            return JNI_FALSE;
        }

        is.close();

        std::istringstream iss(appConfig->getLoggerConfig());

        log4cplus::PropertyConfigurator loggerConfigurator(iss);
        loggerConfigurator.configure();

        af::settings.init(appConfig);

        LOG4CPLUS_INFO(af::logger(), "Starting...");

        OGLInit();

        OALInit();

        return JNI_TRUE;
    }

    JNIEXPORT void JNICALL Java_com_airforce_AirForceJNILib_suspend(JNIEnv* env, jobject obj)
    {
        if (!gameInitialized) {
            return;
        }

        game.suspend();
    }

    JNIEXPORT jboolean JNICALL Java_com_airforce_AirForceJNILib_resize(JNIEnv* env, jobject obj, jint width, jint height)
    {
        if (gameInitializeCalled && !gameInitialized) {
            return JNI_FALSE;
        }

        gameInitializeCalled = true;

        if (gameInitialized) {
            game.reload();
        } else {
            if (!game.init(width, height)) {
                return JNI_FALSE;
            }

            gameInitialized = true;
        }

        return JNI_TRUE;
    }

    JNIEXPORT jboolean JNICALL Java_com_airforce_AirForceJNILib_step(JNIEnv* env, jobject obj)
    {
        jboolean res = JNI_TRUE;

        if (!gameInitialized) {
            return res;
        }

        res = game.update();

        boost::mutex::scoped_lock lock(inputMtx);

        if (menuPressed == 1) {
            inputEvents.push_back(InputEvent(af::KEY_ESCAPE, true));
            menuPressed = 0;
        } else if (menuPressed > 0) {
            --menuPressed;
        }

        for (std::vector<InputEvent>::const_iterator it = inputEvents.begin();
             it != inputEvents.end(); ++it) {
            switch (it->type) {
            case 0:
                game.keyPress(it->keyCode);
                break;
            case 1:
                game.keyRelease(it->keyCode);
                break;
            case 2:
                game.touchDown(it->finger, it->point);
                break;
            case 3:
                game.touchUp(it->finger);
                break;
            default:
                assert(false);
                break;
            }
        }

        inputEvents.clear();

        return res;
    }

    JNIEXPORT void JNICALL Java_com_airforce_AirForceJNILib_cancelStep(JNIEnv* env, jobject obj)
    {
        if (!gameInitialized) {
            return;
        }

        game.cancelUpdate();
    }

    JNIEXPORT void JNICALL Java_com_airforce_AirForceJNILib_render(JNIEnv* env, jobject obj)
    {
        if (!gameInitialized) {
            return;
        }

        game.render();
    }

    JNIEXPORT void JNICALL Java_com_airforce_AirForceJNILib_input(JNIEnv* env, jobject obj, jint finger, jfloat x, jfloat y, jboolean up)
    {
        if (!gameInitialized) {
            return;
        }

        boost::mutex::scoped_lock lock(inputMtx);

        if (up) {
            inputEvents.push_back(InputEvent(finger));
        } else {
            inputEvents.push_back(InputEvent(finger, b2Vec2(x, y)));
        }
    }

    JNIEXPORT void JNICALL Java_com_airforce_AirForceJNILib_menu(JNIEnv* env, jobject obj)
    {
        if (!gameInitialized) {
            return;
        }

        boost::mutex::scoped_lock lock(inputMtx);

        if (menuPressed <= 0) {
            /*
             * hold 'ESC' for 3 frames.
             */
            menuPressed = 3;
            inputEvents.push_back(InputEvent(af::KEY_ESCAPE, false));
        }
    }
}
