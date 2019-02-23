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
#include "InputKeyboard.h"
#include "OGL.h"
#include "af/OAL.h"
#include "Settings.h"
#include "PlatformWin32.h"
#include "GameLogAppender.h"
#include "DummyShell.h"
#ifdef USE_STEAM_API
#include "SteamShell.h"
#endif
#include "af/Types.h"
#include "af/StreamAppConfig.h"
#include "af/SequentialAppConfig.h"
#include <log4cplus/configurator.h>
#include <log4cplus/spi/factory.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/hierarchy.h>
#include <boost/make_shared.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <fstream>
#include <GL/wglext.h>
#include <fcntl.h>
#include <io.h>
#include <process.h>
#include <conio.h>
#include <xinput.h>

#define WGL_GET_PROC(func, sym) \
    do { \
        *(void**)(&func) = GetProcAddress(gHandle, #sym); \
        if (!func) { \
            LOG4CPLUS_ERROR(af::logger(), "Unable to load WGL symbol " #sym); \
            return false; \
        } \
    } while (0)

#define WGL_GET_EXT_PROC(extName, func, sym) \
    do { \
        if ((strstr(extStr, #extName " ") == NULL)) { \
            LOG4CPLUS_ERROR(af::logger(), #extName " is not supported"); \
            return false; \
        } \
        *(void**)(&func) = gGetProcAddress((LPCSTR)#sym); \
        if (!func) { \
            LOG4CPLUS_ERROR(af::logger(), "Unable to load WGL symbol " #sym); \
            return false; \
        } \
    } while (0)

#define WGL_GET_EXT_PROC_OPT(extName, func, sym) \
    do { \
        if ((strstr(extStr, #extName " ") != NULL)) { \
            *(void**)(&func) = gGetProcAddress((LPCSTR)#sym); \
            if (!func) { \
                LOG4CPLUS_WARN(af::logger(), "Unable to load WGL symbol " #sym); \
            } \
        } else { \
            LOG4CPLUS_WARN(af::logger(), #extName " is not supported"); \
        } \
    } while (0)

#define GL_GET_PROC(func, sym) \
    do { \
        *(void**)(&af::ogl.func) = gGetProcAddress((LPCSTR)#sym); \
        if (!af::ogl.func) { \
            *(void**)(&af::ogl.func) = GetProcAddress(gHandle, #sym); \
            if (!af::ogl.func) { \
                LOG4CPLUS_ERROR(af::logger(), "Unable to load GL symbol " #sym); \
                return false; \
            } \
        } \
    } while (0)

#define AL_GET_PROC(func, sym) \
    do { \
        *(void**)(&af::oal.func) = GetProcAddress(handle, #sym); \
        if (!af::oal.func) { \
            LOG4CPLUS_ERROR(af::logger(), "Unable to load OpenAL symbol " #sym); \
            return false; \
        } \
    } while (0)

typedef HGLRC(WINAPI *PFNWGLCREATECONTEXTPROC)(HDC hdl);
typedef BOOL(WINAPI *PFNWGLDELETECONTEXTPROC)(HGLRC hdl);
typedef PROC(WINAPI *PFNWGLGETPROCADDRESSPROC)(LPCSTR sym);
typedef BOOL(WINAPI *PFNWGLMAKECURRENTPROC)(HDC dev_ctx, HGLRC rend_ctx);

static HINSTANCE gHandle;
static PFNWGLCREATECONTEXTPROC gCreateContext;
static PFNWGLDELETECONTEXTPROC gDeleteContext;
static PFNWGLGETPROCADDRESSPROC gGetProcAddress;
static PFNWGLMAKECURRENTPROC gMakeCurrent;
/* WGL extensions */
static PFNWGLGETEXTENSIONSSTRINGEXTPROC gGetExtensionsStringEXT;
static PFNWGLGETEXTENSIONSSTRINGARBPROC gGetExtensionsStringARB;
static PFNWGLCHOOSEPIXELFORMATARBPROC gChoosePixelFormatARB;
static PFNWGLGETPIXELFORMATATTRIBIVARBPROC gGetPixelFormatAttribivARB;
/* WGL_ARB_create_context */
static PFNWGLCREATECONTEXTATTRIBSARBPROC gCreateContextAttribsARB = NULL;
/* WGL_EXT_swap_control */
static PFNWGLSWAPINTERVALEXTPROC gSwapIntervalEXT = NULL;

static DEVMODEA desktopMode;

static HWND gWnd = NULL;
static HDC gDC;
static HGLRC gCtx;
static HCURSOR gCursor;

static af::Game game;

static bool gRunning = true;

static bool gTrueFullscreen = false;

struct InputEvent
{
    InputEvent(af::KeyIdentifier ki, bool up)
    : type(up ? 1 : 0),
      ki(ki)
    {
    }

    InputEvent(bool left, const b2Vec2& point)
    : type(2),
      left(left),
      point(point)
    {
    }

    explicit InputEvent(bool left)
    : type(3),
      left(left)
    {
    }

    explicit InputEvent(const b2Vec2& point)
    : type(4),
      point(point)
    {
    }

    InputEvent(int delta, int dummy1, int dummy2)
    : type(5),
      delta(delta)
    {
    }

    int type;
    af::KeyIdentifier ki;
    bool left;
    b2Vec2 point;
    int delta;
};

static boost::mutex gInputMtx;
static std::vector<InputEvent> gInputEvents;

static const int gCtxAttribs[] =
{
    WGL_CONTEXT_MAJOR_VERSION_ARB, 2,
    WGL_CONTEXT_MINOR_VERSION_ARB, 1,
    0
};

static int gMsaaConfigAttribs[] = {
    WGL_SUPPORT_OPENGL_ARB, TRUE,
    WGL_DOUBLE_BUFFER_ARB, TRUE,
    WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
    WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
    WGL_RED_BITS_ARB, 8,
    WGL_GREEN_BITS_ARB, 8,
    WGL_BLUE_BITS_ARB, 8,
    WGL_ALPHA_BITS_ARB, 8,
    WGL_COLOR_BITS_ARB, 32,
    WGL_DEPTH_BITS_ARB, 24,
    WGL_SAMPLES_ARB, 1,
    WGL_SAMPLE_BUFFERS_ARB, 1,
    0,
};

static const int gConfigAttribs[] = {
    WGL_SUPPORT_OPENGL_ARB, TRUE,
    WGL_DOUBLE_BUFFER_ARB, TRUE,
    WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
    WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
    WGL_RED_BITS_ARB, 8,
    WGL_GREEN_BITS_ARB, 8,
    WGL_BLUE_BITS_ARB, 8,
    WGL_ALPHA_BITS_ARB, 8,
    WGL_COLOR_BITS_ARB, 32,
    WGL_DEPTH_BITS_ARB, 24,
    0,
};

static bool OGLPreInit()
{
    PIXELFORMATDESCRIPTOR pixfmt;
    WNDCLASSEXA wcex;
    HWND hWnd = NULL;
    HDC hDC = NULL;
    HGLRC ctx = NULL;
    int configId = 0;
    const char *extStr = NULL;

    LOG4CPLUS_INFO(af::logger(), "Initializing OpenGL...");

    memset(&pixfmt, 0, sizeof(pixfmt));

    pixfmt.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pixfmt.nVersion = 1;
    pixfmt.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pixfmt.iPixelType = PFD_TYPE_RGBA;
    pixfmt.cColorBits = 32;
    pixfmt.cDepthBits = 24;
    pixfmt.iLayerType = PFD_MAIN_PLANE;

    memset(&wcex, 0, sizeof(wcex));

    wcex.cbSize = sizeof(wcex);
    wcex.lpfnWndProc = &DefWindowProcA;
    wcex.hInstance = GetModuleHandle(NULL);
    wcex.lpszClassName = "AirForceDummyWinClass";

    if (!RegisterClassExA(&wcex)) {
        LOG4CPLUS_ERROR(af::logger(), "Unable to register dummy win class");
        return false;
    }

    gHandle = LoadLibraryA("opengl32");

    if (!gHandle) {
        LOG4CPLUS_ERROR(af::logger(), "Unable to load opengl32.dll");
        return false;
    }

    WGL_GET_PROC(gCreateContext, wglCreateContext);
    WGL_GET_PROC(gDeleteContext, wglDeleteContext);
    WGL_GET_PROC(gGetProcAddress, wglGetProcAddress);
    WGL_GET_PROC(gMakeCurrent, wglMakeCurrent);

    hWnd = CreateWindowA("AirForceDummyWinClass", "AirForceDummyWin",
        WS_DISABLED | WS_POPUP,
        0, 0, 1, 1, NULL, NULL, 0, 0);

    if (!hWnd) {
        LOG4CPLUS_ERROR(af::logger(), "Unable to create dummy win");
        return false;
    }

    hDC = GetDC(hWnd);

    if (!hDC) {
        LOG4CPLUS_ERROR(af::logger(), "Unable to get dummy win DC");
        return false;
    }

    configId = ChoosePixelFormat(hDC, &pixfmt);

    if (!configId) {
        LOG4CPLUS_ERROR(af::logger(), "ChoosePixelFormat failed");
        return false;
    }

    if (!SetPixelFormat(hDC, configId, &pixfmt)) {
        LOG4CPLUS_ERROR(af::logger(), "SetPixelFormat failed");
        return false;
    }

    ctx = gCreateContext(hDC);
    if (!ctx) {
        LOG4CPLUS_ERROR(af::logger(), "wglCreateContext failed");
        return false;
    }

    if (!gMakeCurrent(hDC, ctx)) {
        LOG4CPLUS_ERROR(af::logger(), "wglMakeCurrent failed");
        return false;
    }

    /*
     * WGL extensions couldn't be queried by glGetString(), we need to use
     * wglGetExtensionsStringARB or wglGetExtensionsStringEXT for this, which
     * themselves are extensions
     */
    gGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)gGetProcAddress((LPCSTR)"wglGetExtensionsStringARB");
    gGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)gGetProcAddress((LPCSTR)"wglGetExtensionsStringEXT");

    if (gGetExtensionsStringARB) {
        extStr = gGetExtensionsStringARB(hDC);
    } else if (gGetExtensionsStringEXT) {
        extStr = gGetExtensionsStringEXT();
    }

    if (!extStr) {
        LOG4CPLUS_ERROR(af::logger(), "Unable to obtain WGL extension string");
        return false;
    }

    WGL_GET_EXT_PROC(WGL_ARB_pixel_format, gChoosePixelFormatARB, wglChoosePixelFormatARB);
    WGL_GET_EXT_PROC(WGL_ARB_pixel_format, gGetPixelFormatAttribivARB, wglGetPixelFormatAttribivARB);
    WGL_GET_EXT_PROC_OPT(WGL_ARB_create_context, gCreateContextAttribsARB, wglCreateContextAttribsARB);
    WGL_GET_EXT_PROC_OPT(WGL_EXT_swap_control, gSwapIntervalEXT, wglSwapIntervalEXT);

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
    GL_GET_PROC(GenerateMipmap, glGenerateMipmapEXT);
    GL_GET_PROC(PointSize, glPointSize);
    GL_GET_PROC(LineWidth, glLineWidth);
    GL_GET_PROC(GenFramebuffers, glGenFramebuffersEXT);
    GL_GET_PROC(DeleteFramebuffers, glDeleteFramebuffersEXT);
    GL_GET_PROC(BindFramebuffer, glBindFramebufferEXT);
    GL_GET_PROC(FramebufferTexture2D, glFramebufferTexture2DEXT);
    GL_GET_PROC(CheckFramebufferStatus, glCheckFramebufferStatusEXT);
    GL_GET_PROC(Uniform1f, glUniform1f);
    GL_GET_PROC(Uniform2f, glUniform2f);
    GL_GET_PROC(ColorMask, glColorMask);
    GL_GET_PROC(StencilFunc, glStencilFunc);
    GL_GET_PROC(StencilOp, glStencilOp);
    GL_GET_PROC(GetString, glGetString);
    GL_GET_PROC(Scissor, glScissor);

    const int numPixelFormatsQuery = WGL_NUMBER_PIXEL_FORMATS_ARB;
    int numFormats = 0;

    if (!gGetPixelFormatAttribivARB(hDC, 0, 0, 1, &numPixelFormatsQuery, &numFormats) || !numFormats) {
        LOG4CPLUS_ERROR(af::logger(), "wglGetPixelFormatAttribivARB failed to query number of formats");
        return false;
    }

    std::vector<int> configIds(numFormats, 0);

    UINT n = 0;

    if (!gChoosePixelFormatARB(hDC,
        gConfigAttribs,
        NULL,
        numFormats,
        &configIds[0],
        &n) || (n == 0)) {
        LOG4CPLUS_ERROR(af::logger(), "wglChoosePixelFormatARB failed to enumerate formats");
        return false;
    }

    const int queryList[6] = {
        WGL_RED_BITS_ARB,
        WGL_GREEN_BITS_ARB,
        WGL_BLUE_BITS_ARB,
        WGL_ALPHA_BITS_ARB,
        WGL_DEPTH_BITS_ARB,
        WGL_SAMPLES_ARB,
    };

    int attrVals[6];

    std::set<af::UInt32> ss;

    ss.insert(0);

    PIXELFORMATDESCRIPTOR pfd;

    for (UINT i = 0; i < n; ++i) {
        if (!DescribePixelFormat(hDC, configIds[i], sizeof(pfd), &pfd)) {
            continue;
        }

        if (!gGetPixelFormatAttribivARB(hDC, configIds[i], 0, 6, queryList, attrVals)) {
            continue;
        }
        
        if ((attrVals[0] == 8) && (attrVals[1] == 8) && (attrVals[2] == 8) &&
            (attrVals[3] == 8) && (attrVals[4] == 24) && (attrVals[5] >= 0)) {
            ss.insert(attrVals[5]);
        }
    }

    gMakeCurrent(NULL, NULL);
    gDeleteContext(ctx);
    ReleaseDC(hWnd, hDC);
    DestroyWindow(hWnd);

    std::vector<af::UInt32> msaaModes;

    std::copy(ss.begin(), ss.end(), std::back_inserter(msaaModes));
    
    af::platform->setMsaaModes(msaaModes);

    if (gSwapIntervalEXT) {
        af::platform->setVSyncSupported(true);
    }

    return true;
}

static void PopulateVideoModes()
{
    af::UInt32 maxWinWidth = af::settings.viewWidth;
    af::UInt32 maxWinHeight = af::settings.viewHeight;

    ZeroMemory(&desktopMode, sizeof(DEVMODEA));
    desktopMode.dmSize = sizeof(DEVMODEA);

    if (EnumDisplaySettingsA(NULL, ENUM_CURRENT_SETTINGS, &desktopMode)) {
        int modeIndex = 0;

        std::set<af::VideoMode> tmp;

        for (;;) {
            DEVMODEA dm;

            ZeroMemory(&dm, sizeof(DEVMODEA));
            dm.dmSize = sizeof(DEVMODEA);

            if (!EnumDisplaySettingsA(NULL, modeIndex, &dm)) {
                break;
            }

            modeIndex++;

            if (dm.dmBitsPerPel != 32) {
                continue;
            }

            if ((dm.dmPelsWidth <= desktopMode.dmPelsWidth) && (dm.dmPelsHeight <= desktopMode.dmPelsHeight) && (dm.dmDisplayFrequency == desktopMode.dmDisplayFrequency)) {
                tmp.insert(af::VideoMode(dm.dmPelsWidth, dm.dmPelsHeight));
            }
        }

        tmp.insert(af::VideoMode(desktopMode.dmPelsWidth, desktopMode.dmPelsHeight));

        if (desktopMode.dmPelsWidth > maxWinWidth) {
            maxWinWidth = desktopMode.dmPelsWidth;
        }

        if (desktopMode.dmPelsHeight > maxWinHeight) {
            maxWinHeight = desktopMode.dmPelsHeight;
        }

        int i = 0;

        for (std::set<af::VideoMode>::const_iterator it = tmp.begin(); it != tmp.end(); ++it) {
            if ((it->width == desktopMode.dmPelsWidth) && (it->height == desktopMode.dmPelsHeight)) {
                af::platform->setDesktopVideoMode(i);
                break;
            }
            ++i;
        }

        std::vector<af::VideoMode> videoModes;

        std::copy(tmp.begin(), tmp.end(), std::back_inserter(videoModes));

        af::platform->setDesktopVideoModes(videoModes);
    } else {
        maxWinWidth *= 2;
        maxWinHeight *= 2;
    }

    std::set<af::VideoMode> tmp = af::settings.winVideoModes;

    tmp.insert(af::VideoMode(af::settings.viewWidth, af::settings.viewHeight));

    int i = 0;

    std::vector<af::VideoMode> videoModes;

    for (std::set<af::VideoMode>::const_iterator it = tmp.begin(); it != tmp.end(); ++it) {
        if ((it->width > maxWinWidth) || (it->height > maxWinHeight)) {
            continue;
        }

        if ((it->width == af::settings.viewWidth) && (it->height == af::settings.viewHeight)) {
            af::platform->setDefaultVideoMode(i);
        }

        videoModes.push_back(*it);

        ++i;
    }

    af::platform->setWinVideoModes(videoModes);
}

static af::KeyIdentifier kiMap[256];

static void InitKIMap()
{
    memset(kiMap, 0, sizeof(kiMap));

    kiMap['A'] = Rocket::Core::Input::KI_A;
    kiMap['B'] = Rocket::Core::Input::KI_B;
    kiMap['C'] = Rocket::Core::Input::KI_C;
    kiMap['D'] = Rocket::Core::Input::KI_D;
    kiMap['E'] = Rocket::Core::Input::KI_E;
    kiMap['F'] = Rocket::Core::Input::KI_F;
    kiMap['G'] = Rocket::Core::Input::KI_G;
    kiMap['H'] = Rocket::Core::Input::KI_H;
    kiMap['I'] = Rocket::Core::Input::KI_I;
    kiMap['J'] = Rocket::Core::Input::KI_J;
    kiMap['K'] = Rocket::Core::Input::KI_K;
    kiMap['L'] = Rocket::Core::Input::KI_L;
    kiMap['M'] = Rocket::Core::Input::KI_M;
    kiMap['N'] = Rocket::Core::Input::KI_N;
    kiMap['O'] = Rocket::Core::Input::KI_O;
    kiMap['P'] = Rocket::Core::Input::KI_P;
    kiMap['Q'] = Rocket::Core::Input::KI_Q;
    kiMap['R'] = Rocket::Core::Input::KI_R;
    kiMap['S'] = Rocket::Core::Input::KI_S;
    kiMap['T'] = Rocket::Core::Input::KI_T;
    kiMap['U'] = Rocket::Core::Input::KI_U;
    kiMap['V'] = Rocket::Core::Input::KI_V;
    kiMap['W'] = Rocket::Core::Input::KI_W;
    kiMap['X'] = Rocket::Core::Input::KI_X;
    kiMap['Y'] = Rocket::Core::Input::KI_Y;
    kiMap['Z'] = Rocket::Core::Input::KI_Z;
    kiMap['0'] = Rocket::Core::Input::KI_0;
    kiMap['1'] = Rocket::Core::Input::KI_1;
    kiMap['2'] = Rocket::Core::Input::KI_2;
    kiMap['3'] = Rocket::Core::Input::KI_3;
    kiMap['4'] = Rocket::Core::Input::KI_4;
    kiMap['5'] = Rocket::Core::Input::KI_5;
    kiMap['6'] = Rocket::Core::Input::KI_6;
    kiMap['7'] = Rocket::Core::Input::KI_7;
    kiMap['8'] = Rocket::Core::Input::KI_8;
    kiMap['9'] = Rocket::Core::Input::KI_9;
    kiMap[VK_BACK] = Rocket::Core::Input::KI_BACK;
    kiMap[VK_TAB] = Rocket::Core::Input::KI_TAB;
    kiMap[VK_CLEAR] = Rocket::Core::Input::KI_CLEAR;
    kiMap[VK_RETURN] = Rocket::Core::Input::KI_RETURN;
    kiMap[VK_PAUSE] = Rocket::Core::Input::KI_PAUSE;
    kiMap[VK_CAPITAL] = Rocket::Core::Input::KI_CAPITAL;
    kiMap[VK_KANA] = Rocket::Core::Input::KI_KANA;
    kiMap[VK_HANGUL] = Rocket::Core::Input::KI_HANGUL;
    kiMap[VK_JUNJA] = Rocket::Core::Input::KI_JUNJA;
    kiMap[VK_FINAL] = Rocket::Core::Input::KI_FINAL;
    kiMap[VK_HANJA] = Rocket::Core::Input::KI_HANJA;
    kiMap[VK_KANJI] = Rocket::Core::Input::KI_KANJI;
    kiMap[VK_ESCAPE] = Rocket::Core::Input::KI_ESCAPE;
    kiMap[VK_CONVERT] = Rocket::Core::Input::KI_CONVERT;
    kiMap[VK_NONCONVERT] = Rocket::Core::Input::KI_NONCONVERT;
    kiMap[VK_ACCEPT] = Rocket::Core::Input::KI_ACCEPT;
    kiMap[VK_MODECHANGE] = Rocket::Core::Input::KI_MODECHANGE;
    kiMap[VK_SPACE] = Rocket::Core::Input::KI_SPACE;
    kiMap[VK_PRIOR] = Rocket::Core::Input::KI_PRIOR;
    kiMap[VK_NEXT] = Rocket::Core::Input::KI_NEXT;
    kiMap[VK_END] = Rocket::Core::Input::KI_END;
    kiMap[VK_HOME] = Rocket::Core::Input::KI_HOME;
    kiMap[VK_LEFT] = Rocket::Core::Input::KI_LEFT;
    kiMap[VK_UP] = Rocket::Core::Input::KI_UP;
    kiMap[VK_RIGHT] = Rocket::Core::Input::KI_RIGHT;
    kiMap[VK_DOWN] = Rocket::Core::Input::KI_DOWN;
    kiMap[VK_SELECT] = Rocket::Core::Input::KI_SELECT;
    kiMap[VK_PRINT] = Rocket::Core::Input::KI_PRINT;
    kiMap[VK_EXECUTE] = Rocket::Core::Input::KI_EXECUTE;
    kiMap[VK_SNAPSHOT] = Rocket::Core::Input::KI_SNAPSHOT;
    kiMap[VK_INSERT] = Rocket::Core::Input::KI_INSERT;
    kiMap[VK_DELETE] = Rocket::Core::Input::KI_DELETE;
    kiMap[VK_HELP] = Rocket::Core::Input::KI_HELP;
    kiMap[VK_LWIN] = Rocket::Core::Input::KI_LWIN;
    kiMap[VK_RWIN] = Rocket::Core::Input::KI_RWIN;
    kiMap[VK_APPS] = Rocket::Core::Input::KI_APPS;
    kiMap[VK_SLEEP] = Rocket::Core::Input::KI_SLEEP;
    kiMap[VK_NUMPAD0] = Rocket::Core::Input::KI_NUMPAD0;
    kiMap[VK_NUMPAD1] = Rocket::Core::Input::KI_NUMPAD1;
    kiMap[VK_NUMPAD2] = Rocket::Core::Input::KI_NUMPAD2;
    kiMap[VK_NUMPAD3] = Rocket::Core::Input::KI_NUMPAD3;
    kiMap[VK_NUMPAD4] = Rocket::Core::Input::KI_NUMPAD4;
    kiMap[VK_NUMPAD5] = Rocket::Core::Input::KI_NUMPAD5;
    kiMap[VK_NUMPAD6] = Rocket::Core::Input::KI_NUMPAD6;
    kiMap[VK_NUMPAD7] = Rocket::Core::Input::KI_NUMPAD7;
    kiMap[VK_NUMPAD8] = Rocket::Core::Input::KI_NUMPAD8;
    kiMap[VK_NUMPAD9] = Rocket::Core::Input::KI_NUMPAD9;
    kiMap[VK_MULTIPLY] = Rocket::Core::Input::KI_MULTIPLY;
    kiMap[VK_ADD] = Rocket::Core::Input::KI_ADD;
    kiMap[VK_SEPARATOR] = Rocket::Core::Input::KI_SEPARATOR;
    kiMap[VK_SUBTRACT] = Rocket::Core::Input::KI_SUBTRACT;
    kiMap[VK_DECIMAL] = Rocket::Core::Input::KI_DECIMAL;
    kiMap[VK_DIVIDE] = Rocket::Core::Input::KI_DIVIDE;
    kiMap[VK_F1] = Rocket::Core::Input::KI_F1;
    kiMap[VK_F2] = Rocket::Core::Input::KI_F2;
    kiMap[VK_F3] = Rocket::Core::Input::KI_F3;
    kiMap[VK_F4] = Rocket::Core::Input::KI_F4;
    kiMap[VK_F5] = Rocket::Core::Input::KI_F5;
    kiMap[VK_F6] = Rocket::Core::Input::KI_F6;
    kiMap[VK_F7] = Rocket::Core::Input::KI_F7;
    kiMap[VK_F8] = Rocket::Core::Input::KI_F8;
    kiMap[VK_F9] = Rocket::Core::Input::KI_F9;
    kiMap[VK_F10] = Rocket::Core::Input::KI_F10;
    kiMap[VK_F11] = Rocket::Core::Input::KI_F11;
    kiMap[VK_F12] = Rocket::Core::Input::KI_F12;
    kiMap[VK_F13] = Rocket::Core::Input::KI_F13;
    kiMap[VK_F14] = Rocket::Core::Input::KI_F14;
    kiMap[VK_F15] = Rocket::Core::Input::KI_F15;
    kiMap[VK_F16] = Rocket::Core::Input::KI_F16;
    kiMap[VK_F17] = Rocket::Core::Input::KI_F17;
    kiMap[VK_F18] = Rocket::Core::Input::KI_F18;
    kiMap[VK_F19] = Rocket::Core::Input::KI_F19;
    kiMap[VK_F20] = Rocket::Core::Input::KI_F20;
    kiMap[VK_F21] = Rocket::Core::Input::KI_F21;
    kiMap[VK_F22] = Rocket::Core::Input::KI_F22;
    kiMap[VK_F23] = Rocket::Core::Input::KI_F23;
    kiMap[VK_F24] = Rocket::Core::Input::KI_F24;
    kiMap[VK_NUMLOCK] = Rocket::Core::Input::KI_NUMLOCK;
    kiMap[VK_SCROLL] = Rocket::Core::Input::KI_SCROLL;
    kiMap[VK_OEM_NEC_EQUAL] = Rocket::Core::Input::KI_OEM_NEC_EQUAL;
    kiMap[VK_OEM_FJ_JISHO] = Rocket::Core::Input::KI_OEM_FJ_JISHO;
    kiMap[VK_OEM_FJ_MASSHOU] = Rocket::Core::Input::KI_OEM_FJ_MASSHOU;
    kiMap[VK_OEM_FJ_TOUROKU] = Rocket::Core::Input::KI_OEM_FJ_TOUROKU;
    kiMap[VK_OEM_FJ_LOYA] = Rocket::Core::Input::KI_OEM_FJ_LOYA;
    kiMap[VK_OEM_FJ_ROYA] = Rocket::Core::Input::KI_OEM_FJ_ROYA;
    kiMap[VK_SHIFT] = Rocket::Core::Input::KI_LSHIFT;
    kiMap[VK_CONTROL] = Rocket::Core::Input::KI_LCONTROL;
    kiMap[VK_MENU] = Rocket::Core::Input::KI_LMENU;
    kiMap[VK_BROWSER_BACK] = Rocket::Core::Input::KI_BROWSER_BACK;
    kiMap[VK_BROWSER_FORWARD] = Rocket::Core::Input::KI_BROWSER_FORWARD;
    kiMap[VK_BROWSER_REFRESH] = Rocket::Core::Input::KI_BROWSER_REFRESH;
    kiMap[VK_BROWSER_STOP] = Rocket::Core::Input::KI_BROWSER_STOP;
    kiMap[VK_BROWSER_SEARCH] = Rocket::Core::Input::KI_BROWSER_SEARCH;
    kiMap[VK_BROWSER_FAVORITES] = Rocket::Core::Input::KI_BROWSER_FAVORITES;
    kiMap[VK_BROWSER_HOME] = Rocket::Core::Input::KI_BROWSER_HOME;
    kiMap[VK_VOLUME_MUTE] = Rocket::Core::Input::KI_VOLUME_MUTE;
    kiMap[VK_VOLUME_DOWN] = Rocket::Core::Input::KI_VOLUME_DOWN;
    kiMap[VK_VOLUME_UP] = Rocket::Core::Input::KI_VOLUME_UP;
    kiMap[VK_MEDIA_NEXT_TRACK] = Rocket::Core::Input::KI_MEDIA_NEXT_TRACK;
    kiMap[VK_MEDIA_PREV_TRACK] = Rocket::Core::Input::KI_MEDIA_PREV_TRACK;
    kiMap[VK_MEDIA_STOP] = Rocket::Core::Input::KI_MEDIA_STOP;
    kiMap[VK_MEDIA_PLAY_PAUSE] = Rocket::Core::Input::KI_MEDIA_PLAY_PAUSE;
    kiMap[VK_LAUNCH_MAIL] = Rocket::Core::Input::KI_LAUNCH_MAIL;
    kiMap[VK_LAUNCH_MEDIA_SELECT] = Rocket::Core::Input::KI_LAUNCH_MEDIA_SELECT;
    kiMap[VK_LAUNCH_APP1] = Rocket::Core::Input::KI_LAUNCH_APP1;
    kiMap[VK_LAUNCH_APP2] = Rocket::Core::Input::KI_LAUNCH_APP2;
    kiMap[VK_OEM_1] = Rocket::Core::Input::KI_OEM_1;
    kiMap[VK_OEM_PLUS] = Rocket::Core::Input::KI_OEM_PLUS;
    kiMap[VK_OEM_COMMA] = Rocket::Core::Input::KI_OEM_COMMA;
    kiMap[VK_OEM_MINUS] = Rocket::Core::Input::KI_OEM_MINUS;
    kiMap[VK_OEM_PERIOD] = Rocket::Core::Input::KI_OEM_PERIOD;
    kiMap[VK_OEM_2] = Rocket::Core::Input::KI_OEM_2;
    kiMap[VK_OEM_3] = Rocket::Core::Input::KI_OEM_3;
    kiMap[VK_OEM_4] = Rocket::Core::Input::KI_OEM_4;
    kiMap[VK_OEM_5] = Rocket::Core::Input::KI_OEM_5;
    kiMap[VK_OEM_6] = Rocket::Core::Input::KI_OEM_6;
    kiMap[VK_OEM_7] = Rocket::Core::Input::KI_OEM_7;
    kiMap[VK_OEM_8] = Rocket::Core::Input::KI_OEM_8;
    kiMap[VK_OEM_AX] = Rocket::Core::Input::KI_OEM_AX;
    kiMap[VK_OEM_102] = Rocket::Core::Input::KI_OEM_102;
    kiMap[VK_ICO_HELP] = Rocket::Core::Input::KI_ICO_HELP;
    kiMap[VK_ICO_00] = Rocket::Core::Input::KI_ICO_00;
    kiMap[VK_PROCESSKEY] = Rocket::Core::Input::KI_PROCESSKEY;
    kiMap[VK_ICO_CLEAR] = Rocket::Core::Input::KI_ICO_CLEAR;
    kiMap[VK_ATTN] = Rocket::Core::Input::KI_ATTN;
    kiMap[VK_CRSEL] = Rocket::Core::Input::KI_CRSEL;
    kiMap[VK_EXSEL] = Rocket::Core::Input::KI_EXSEL;
    kiMap[VK_EREOF] = Rocket::Core::Input::KI_EREOF;
    kiMap[VK_PLAY] = Rocket::Core::Input::KI_PLAY;
    kiMap[VK_ZOOM] = Rocket::Core::Input::KI_ZOOM;
    kiMap[VK_PA1] = Rocket::Core::Input::KI_PA1;
    kiMap[VK_OEM_CLEAR] = Rocket::Core::Input::KI_OEM_CLEAR;
}

static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_KEYDOWN: {
        boost::mutex::scoped_lock lock(gInputMtx);        
        gInputEvents.push_back(InputEvent(kiMap[wParam], false));
        break;
    }
    case WM_KEYUP: {
        boost::mutex::scoped_lock lock(gInputMtx);
        gInputEvents.push_back(InputEvent(kiMap[wParam], true));
        break;
    }
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN: {
        SetCapture(hWnd);
        boost::mutex::scoped_lock lock(gInputMtx);
        gInputEvents.push_back(InputEvent(message == WM_LBUTTONDOWN, b2Vec2(LOWORD(lParam), HIWORD(lParam))));
        break;
    }
    case WM_LBUTTONUP:
    case WM_RBUTTONUP: {
        ReleaseCapture();
        boost::mutex::scoped_lock lock(gInputMtx);
        gInputEvents.push_back(InputEvent(message == WM_LBUTTONUP));
        break;
    }
    case WM_MOUSEMOVE: {
        int mx = LOWORD(lParam);
        int my = HIWORD(lParam);
        if (mx & 1 << 15) mx -= (1 << 16);
        if (my & 1 << 15) my -= (1 << 16);
        boost::mutex::scoped_lock lock(gInputMtx);
        gInputEvents.push_back(InputEvent(b2Vec2(mx, my)));
        break;
    }
    case WM_MOUSEWHEEL: {
        boost::mutex::scoped_lock lock(gInputMtx);
        gInputEvents.push_back(InputEvent(static_cast<short>(HIWORD(wParam)) / -WHEEL_DELTA, 0, 0));
        break;
    }
    case WM_SETCURSOR: {
       if (LOWORD(lParam) == HTCLIENT) {
           SetCursor(NULL);
       } else {
           SetCursor(gCursor);
       }
       return TRUE;
       break;
    }
    default:
        return DefWindowProcA(hWnd, message, wParam, lParam);
    }

    return 0;
}

static bool createWindow(af::UInt32 width, af::UInt32 height, bool fullscreen, af::UInt32 samples)
{
    WNDCLASSEXA wcex;
    int configId = 0;
    UINT n = 0;
    PIXELFORMATDESCRIPTOR pixfmt;

    memset(&wcex, 0, sizeof(wcex));

    wcex.cbSize = sizeof(wcex);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = wndProc;
    wcex.hInstance = GetModuleHandle(NULL);
    wcex.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(107));
    wcex.hCursor = NULL;
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = "AirForceWinClass";

    if (gWnd == NULL) {
        gCursor = LoadCursor(NULL, IDC_ARROW);

        if (!RegisterClassExA(&wcex)) {
            LOG4CPLUS_ERROR(af::logger(), "Unable to register win class");
            return false;
        }
    }

    RECT rect;

    rect.left = 0;
    rect.top = 0;
    rect.right = width;
    rect.bottom = height;

    DWORD dwExStyle;
    DWORD dwStyle;

    if (fullscreen) {
        dwExStyle = WS_EX_APPWINDOW;
        dwStyle = WS_POPUP;

        AdjustWindowRectEx(&rect,
            dwStyle,
            FALSE,
            dwExStyle);
    } else {
        dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
        dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU;

        AdjustWindowRectEx(&rect,
            WS_OVERLAPPEDWINDOW,
            FALSE,
            dwExStyle);
    }
    
    gWnd = CreateWindowExA(dwExStyle, "AirForceWinClass",
        "Trigger Time", dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        0, 0, rect.right - rect.left, rect.bottom - rect.top,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    if (!gWnd) {
        LOG4CPLUS_ERROR(af::logger(), "Unable to create win");
        return false;
    }

    gDC = GetDC(gWnd);

    if (!gDC) {
        LOG4CPLUS_ERROR(af::logger(), "Unable to get win DC");
        return false;
    }

    const int* attribs = NULL;

    if (samples > 0) {
        gMsaaConfigAttribs[21] = samples;

        attribs = gMsaaConfigAttribs;
    } else {
        attribs = gConfigAttribs;
    }
    
    if (!gChoosePixelFormatARB(gDC,
        attribs,
        NULL,
        1,
        &configId,
        &n) || (n == 0)) {        
        LOG4CPLUS_ERROR(af::logger(), "wglChoosePixelFormatARB failed");
        return false;        
    }

    if (!DescribePixelFormat(gDC,
        configId,
        sizeof(PIXELFORMATDESCRIPTOR),
        &pixfmt)) {
        LOG4CPLUS_ERROR(af::logger(), "DescribePixelFormat failed");
        return false;
    }

    if (!SetPixelFormat(gDC, configId, &pixfmt)) {
        LOG4CPLUS_ERROR(af::logger(), "SetPixelFormat failed");
        return false;
    }

    return true;
}

static bool OGLInit(bool vsync)
{    
    if (gCreateContextAttribsARB) {
        gCtx = gCreateContextAttribsARB(gDC, NULL, gCtxAttribs);
    } else {
        gCtx = gCreateContext(gDC);
    }

    if (!gCtx) {
        LOG4CPLUS_ERROR(af::logger(), "Unable to create OpenGL context");
        return false;
    }

    if (!gMakeCurrent(gDC, gCtx)) {
        LOG4CPLUS_ERROR(af::logger(), "Unable to make OpenGL context current");
        return false;
    }

    if (gSwapIntervalEXT) {
        gSwapIntervalEXT(vsync ? 1 : 0);
    }

    LOG4CPLUS_INFO(af::logger(), "OpenGL initialized");

    return true;
}

static void destroyWindow()
{
    LOG4CPLUS_INFO(af::logger(), "Destroying window...");

    if (!gMakeCurrent(NULL, NULL)) {
        LOG4CPLUS_WARN(af::logger(), "Unable to release current context");
    }

    gDeleteContext(gCtx);
    ReleaseDC(gWnd, gDC);
    DestroyWindow(gWnd);

    if (af::settings.fullscreen && gTrueFullscreen) {
        ChangeDisplaySettingsA(NULL, 0);
        gTrueFullscreen = false;
    }

    LOG4CPLUS_INFO(af::logger(), "Window destroyed");
}

/*
 * Gamepad stuff.
 * @{
 */

typedef DWORD(WINAPI *PFNXINPUTGETSTATE)(DWORD, XINPUT_STATE*);

static HINSTANCE gXInputHandle;
static PFNXINPUTGETSTATE gXInputGetState;

static struct
{    
    bool present;
    XINPUT_GAMEPAD prev;
} gamepad[XUSER_MAX_COUNT];

static void gamepadInit()
{
    LOG4CPLUS_INFO(af::logger(), "Initializing gamepads...");
    
    gXInputHandle = LoadLibraryA("XInput1_4.dll");  /* 1.4 Ships with Windows 8. */

    if (!gXInputHandle) {        
        gXInputHandle = LoadLibraryA("XInput1_3.dll");  /* 1.3 can be installed as a redistributable component. */
    }    

    if (!gXInputHandle) {
        /* "9.1.0" Ships with Vista and Win7, and is more limited than 1.3+ (e.g. XInputGetStateEx is not available.)  */
        gXInputHandle = LoadLibraryA("XInput9_1_0.dll");
    }

    if (gXInputHandle) {
        gXInputGetState = (PFNXINPUTGETSTATE)GetProcAddress(gXInputHandle, "XInputGetState");

        if (!gXInputGetState) {
            LOG4CPLUS_ERROR(af::logger(), "Unable to locale XInputGetState symbol, gamepad support not available");
            gXInputHandle = NULL;
        }        
    } else {
        LOG4CPLUS_ERROR(af::logger(), "Unable to load xinput dll, gamepad support not available");        
    }
    
    LOG4CPLUS_INFO(af::logger(), "gamepads initialized");
}

static void gamepadShutdown()
{
    LOG4CPLUS_INFO(af::logger(), "Shutting down gamepads...");
    LOG4CPLUS_INFO(af::logger(), "gamepads shut down");
}

static inline void gamepadUpdateButton(WORD buttons, WORD newButtons, WORD bit, af::GamepadButton gb)
{    
    if ((buttons & bit) != 0) {
        if ((newButtons & bit) != 0) {
            game.gamepadPress(gb);
        } else {
            game.gamepadRelease(gb);
        }
    }
}

static void gamepadUpdate()
{
    if (!gXInputGetState) {
        return;
    }

    for (DWORD i = 0; i < XUSER_MAX_COUNT; ++i) {
        XINPUT_STATE xs;

        if (gXInputGetState(i, &xs) == 0) {
            if (!gamepad[i].present) {
                LOG4CPLUS_INFO(af::logger(), "gamepad " << i << ": connected");
                gamepad[i].present = true;
            }

            WORD buttons = gamepad[i].prev.wButtons ^ xs.Gamepad.wButtons;

            gamepadUpdateButton(buttons, xs.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_UP, af::GamepadDPADUp);
            gamepadUpdateButton(buttons, xs.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN, af::GamepadDPADDown);
            gamepadUpdateButton(buttons, xs.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT, af::GamepadDPADLeft);
            gamepadUpdateButton(buttons, xs.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT, af::GamepadDPADRight);
            gamepadUpdateButton(buttons, xs.Gamepad.wButtons, XINPUT_GAMEPAD_START, af::GamepadStart);
            gamepadUpdateButton(buttons, xs.Gamepad.wButtons, XINPUT_GAMEPAD_BACK, af::GamepadBack);
            gamepadUpdateButton(buttons, xs.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_THUMB, af::GamepadLeftStick);
            gamepadUpdateButton(buttons, xs.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB, af::GamepadRightStick);
            gamepadUpdateButton(buttons, xs.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER, af::GamepadLeftBumper);
            gamepadUpdateButton(buttons, xs.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER, af::GamepadRightBumper);
            gamepadUpdateButton(buttons, xs.Gamepad.wButtons, XINPUT_GAMEPAD_A, af::GamepadA);
            gamepadUpdateButton(buttons, xs.Gamepad.wButtons, XINPUT_GAMEPAD_B, af::GamepadB);
            gamepadUpdateButton(buttons, xs.Gamepad.wButtons, XINPUT_GAMEPAD_X, af::GamepadX);
            gamepadUpdateButton(buttons, xs.Gamepad.wButtons, XINPUT_GAMEPAD_Y, af::GamepadY);

            if (gamepad[i].prev.bLeftTrigger != xs.Gamepad.bLeftTrigger) {
                game.gamepadMoveTrigger(true, static_cast<float>(xs.Gamepad.bLeftTrigger) / 255.0f);
            }

            if (gamepad[i].prev.bRightTrigger != xs.Gamepad.bRightTrigger) {
                game.gamepadMoveTrigger(false, static_cast<float>(xs.Gamepad.bRightTrigger) / 255.0f);
            }

            if ((gamepad[i].prev.sThumbLX != xs.Gamepad.sThumbLX) || (gamepad[i].prev.sThumbLY != xs.Gamepad.sThumbLY)) {
                SHORT tmpX = xs.Gamepad.sThumbLX;
                if (tmpX < -32767) {
                    tmpX = -32767;
                }
                SHORT tmpY = xs.Gamepad.sThumbLY;
                if (tmpY < -32767) {
                    tmpY = -32767;
                }
                game.gamepadMoveStick(true, b2Vec2(static_cast<float>(tmpX) / 32767.0f, static_cast<float>(tmpY) / 32767.0f));
            }

            if ((gamepad[i].prev.sThumbRX != xs.Gamepad.sThumbRX) || (gamepad[i].prev.sThumbRY != xs.Gamepad.sThumbRY)) {
                SHORT tmpX = xs.Gamepad.sThumbRX;
                if (tmpX < -32767) {
                    tmpX = -32767;
                }
                SHORT tmpY = xs.Gamepad.sThumbRY;
                if (tmpY < -32767) {
                    tmpY = -32767;
                }
                game.gamepadMoveStick(false, b2Vec2(static_cast<float>(tmpX) / 32767.0f, static_cast<float>(tmpY) / 32767.0f));
            }
            
            gamepad[i].prev = xs.Gamepad;
        } else if (gamepad[i].present) {
            LOG4CPLUS_INFO(af::logger(), "gamepad " << i << ": disconnected");
            memset(&gamepad[i].prev, 0, sizeof(gamepad[i].prev));
            gamepad[i].present = false;            
        }
    }
}

/*
 * @}
 */

static void gameThread()
{
    LOG4CPLUS_INFO(af::logger(), "Game thread started");

    bool pressed = false;

    std::vector<InputEvent> tmp;

    while (gRunning && game.update()) {
        {
            boost::mutex::scoped_lock lock(gInputMtx);
            tmp.swap(gInputEvents);
        }

        for (std::vector<InputEvent>::const_iterator it = tmp.begin();
             it != tmp.end(); ++it) {
            switch (it->type) {
            case 0:
                game.keyPress(it->ki);
                break;
            case 1:
                game.keyRelease(it->ki);
                break;
            case 2:
                game.touchDown(0, it->point);
                game.mouseDown(it->left);
                pressed = true;
                break;
            case 3:
                game.touchUp(0);
                game.mouseUp(it->left);
                pressed = false;
                break;
            case 4:
                if (pressed) {
                    game.touchDown(0, it->point);
                }
                game.mouseMove(it->point);
                break;
            case 5:
                game.mouseWheel(it->delta);
                break;
            default:
                assert(false);
                break;
            }
        }

        gamepadUpdate();

        af::gameShell->update();

        tmp.clear();
    }
   
    gRunning = false;

#ifdef USE_STEAM_API
    if (steamShell) {
        steamShell->shutdown();
    }
#endif

    game.cancelRender();

    LOG4CPLUS_INFO(af::logger(), "Game thread finished");
}

static bool OALInit()
{
    LOG4CPLUS_INFO(af::logger(), "Initializing OpenAL...");

    HINSTANCE handle = LoadLibraryA("openal32");

    if (!handle) {
        LOG4CPLUS_ERROR(af::logger(), "Unable to load openal32.dll");
        return false;
    }

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

    LOG4CPLUS_INFO(af::logger(), "OpenAL initialized");

    return true;
}

static bool cmFullscreen;
static int cmVideoMode;
static int cmMsaaMode;
static bool cmVsync;
static bool cmRes;

static boost::mutex cmMutex;
static boost::condition_variable cmCond;
static bool cmDone = false;

void changeVideoModeInternal()
{
    af::UInt32 samples = af::platform->msaaModes()[cmMsaaMode];
    af::VideoMode vm;

    if (cmFullscreen) {
        vm = af::platform->desktopVideoModes()[cmVideoMode];
    } else {
        vm = af::platform->winVideoModes()[cmVideoMode];
    }

    if (gWnd) {
        destroyWindow();

        MSG msg;

        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {            
        }
    }

    if (cmFullscreen) {
        if ((vm.width == desktopMode.dmPelsWidth) && (vm.height == desktopMode.dmPelsHeight)) {
            gTrueFullscreen = false;
        } else {
            gTrueFullscreen = true;

            DEVMODEA dm;
        
            memset(&dm, 0, sizeof(dm));
            dm.dmSize = sizeof(dm);

            dm.dmPelsWidth = vm.width;
            dm.dmPelsHeight = vm.height;
            dm.dmBitsPerPel = 32;
            dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

            if (desktopMode.dmDisplayFrequency != 0) {
                dm.dmDisplayFrequency = desktopMode.dmDisplayFrequency;
                dm.dmFields |= DM_DISPLAYFREQUENCY;
            }

            LONG res = ChangeDisplaySettingsA(&dm, CDS_FULLSCREEN);
        
            if (res != DISP_CHANGE_SUCCESSFUL) {
                LOG4CPLUS_ERROR(af::logger(), "ChangeDisplaySettingsA(" << vm.width << ", " << vm.height << ", " << desktopMode.dmDisplayFrequency << ") failed: " << res);
                return;
            }        
        }
    }

    if (!createWindow(vm.width, vm.height, cmFullscreen, samples)) {
        return;
    }

    if (!OGLInit(cmVsync)) {
        return;
    }

    cmRes = true;
}

bool af::PlatformWin32::changeVideoMode(bool fullscreen, int videoMode, int msaaMode, bool vsync, bool trilinearFilter)
{
    af::VideoMode vm;

    if (fullscreen) {
        vm = platform->desktopVideoModes()[videoMode];
    } else {
        vm = platform->winVideoModes()[videoMode];
    }

    cmFullscreen = fullscreen;
    cmVideoMode = videoMode;
    cmMsaaMode = msaaMode;
    cmVsync = vsync;
    cmRes = false;

    bool notify = false;

    if (gWnd == NULL) {
        changeVideoModeInternal();
    } else {
        notify = true;

        game.cancelRender();

        boost::mutex::scoped_lock lock(cmMutex);

        while (!cmDone) {
            cmCond.wait(lock);
        }
    }

    if (!cmRes) {
        if (notify) {
            {
                boost::mutex::scoped_lock lock(cmMutex);
                cmDone = false;
            }

            cmCond.notify_one();
        }

        return false;
    }

    settings.videoMode = videoMode;
    settings.msaaMode = msaaMode;
    settings.vsync = vsync;
    settings.fullscreen = fullscreen;
    settings.trilinearFilter = trilinearFilter;

    if (settings.viewAspect >= 1.0f) {
        settings.viewWidth = vm.width;
        settings.viewHeight = static_cast<float>(vm.width) / settings.viewAspect;

        if (settings.viewHeight <= vm.height) {
            settings.viewX = 0;
            settings.viewY = static_cast<float>(vm.height - settings.viewHeight) / 2.0f;
        } else {
            settings.viewHeight = vm.height;
            settings.viewWidth = static_cast<float>(settings.viewHeight) * settings.viewAspect;
            settings.viewX = static_cast<float>(vm.width - settings.viewWidth) / 2.0f;
            settings.viewY = 0;
        }
    } else {
        settings.viewHeight = vm.height;
        settings.viewWidth = static_cast<float>(settings.viewHeight) * settings.viewAspect;

        if (settings.viewWidth <= vm.width) {
            settings.viewX = static_cast<float>(vm.width - settings.viewWidth) / 2.0f;
            settings.viewY = 0;
        } else {
            settings.viewWidth = vm.width;
            settings.viewHeight = static_cast<float>(vm.width) / settings.viewAspect;
            settings.viewX = 0;
            settings.viewY = static_cast<float>(vm.height - settings.viewHeight) / 2.0f;
        }
    }

    game.reload();

    if (!notify) {
        ShowWindow(gWnd, SW_SHOW);
        SetForegroundWindow(gWnd);
        SetFocus(gWnd);

        game.renderReload();
    }

    {
        boost::mutex::scoped_lock lock(cmMutex);
        cmDone = false;
    }

    cmCond.notify_one();

    return true;
}

extern const char configIniStr[];
extern const char configEasyIniStr[];
extern const char configHardIniStr[];

static void DoAllocConsole()
{
    static const WORD MAX_CONSOLE_LINES = 2048;
    int hConHandle;
    long lStdHandle;
    CONSOLE_SCREEN_BUFFER_INFO coninfo;
    FILE *fp;
   
    AllocConsole();
    
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
    coninfo.dwSize.Y = MAX_CONSOLE_LINES;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);
    
    lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen(hConHandle, "w");

    *stdout = *fp;
    setvbuf(stdout, NULL, _IONBF, 0);

    lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen(hConHandle, "r");

    *stdin = *fp;
    setvbuf(stdin, NULL, _IONBF, 0);
    
    lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen(hConHandle, "w");
    *stderr = *fp;

    setvbuf(stderr, NULL, _IONBF, 0);
    
    HMENU hMenu = ::GetSystemMenu(GetConsoleWindow(), FALSE);
    if (hMenu != NULL) {
        DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
    }

    ShowWindow(GetConsoleWindow(), SW_SHOW);
}

static void startupFailed()
{
    MessageBox(NULL, "Game startup failed, see $AppData\\TriggerTime\\log.txt for details", "Error", MB_OK | MB_ICONSTOP);
}

static af::AppConfigPtr getNormalAppConfig(const af::AppConfigPtr& appConfig1)
{
    boost::shared_ptr<af::SequentialAppConfig> appConfig =
        boost::make_shared<af::SequentialAppConfig>();

    appConfig->add(appConfig1);

    std::ifstream is("config.ini");

    if (is) {
        boost::shared_ptr<af::StreamAppConfig> appConfig2 =
            boost::make_shared<af::StreamAppConfig>();

        if (!appConfig2->load(is)) {
            MessageBox(NULL, "Cannot read config.ini", "Error", MB_OK | MB_ICONSTOP);
            return af::AppConfigPtr();
        }

        is.close();

        appConfig->add(appConfig2);
    }

    return appConfig;
}

static af::AppConfigPtr getEasyAppConfig(const af::AppConfigPtr& appConfig1)
{
    std::istringstream is2(configEasyIniStr);

    boost::shared_ptr<af::StreamAppConfig> appConfig2 =
        boost::make_shared<af::StreamAppConfig>();

    if (!appConfig2->load(is2)) {
        MessageBox(NULL, "Cannot read built-in config-easy.ini", "Error", MB_OK | MB_ICONSTOP);
        return af::AppConfigPtr();
    }

    boost::shared_ptr<af::SequentialAppConfig> appConfig =
        boost::make_shared<af::SequentialAppConfig>();

    appConfig->add(appConfig1);
    appConfig->add(appConfig2);

    std::ifstream is3("config.ini");

    if (is3) {
        boost::shared_ptr<af::StreamAppConfig> appConfig3 =
            boost::make_shared<af::StreamAppConfig>();

        if (!appConfig3->load(is3)) {
            MessageBox(NULL, "Cannot read config.ini", "Error", MB_OK | MB_ICONSTOP);
            return af::AppConfigPtr();
        }

        is3.close();

        appConfig->add(appConfig3);
    }

    std::ifstream is4("config-easy.ini");

    if (is4) {
        boost::shared_ptr<af::StreamAppConfig> appConfig4 =
            boost::make_shared<af::StreamAppConfig>();

        if (!appConfig4->load(is4)) {
            MessageBox(NULL, "Cannot read config-easy.ini", "Error", MB_OK | MB_ICONSTOP);
            return af::AppConfigPtr();
        }

        is4.close();

        appConfig->add(appConfig4);
    }

    return appConfig;
}

static af::AppConfigPtr getHardAppConfig(const af::AppConfigPtr& appConfig1)
{
    std::istringstream is2(configHardIniStr);

    boost::shared_ptr<af::StreamAppConfig> appConfig2 =
        boost::make_shared<af::StreamAppConfig>();

    if (!appConfig2->load(is2)) {
        MessageBox(NULL, "Cannot read built-in config-hard.ini", "Error", MB_OK | MB_ICONSTOP);
        return af::AppConfigPtr();
    }

    boost::shared_ptr<af::SequentialAppConfig> appConfig =
        boost::make_shared<af::SequentialAppConfig>();

    appConfig->add(appConfig1);
    appConfig->add(appConfig2);

    std::ifstream is3("config.ini");

    if (is3) {
        boost::shared_ptr<af::StreamAppConfig> appConfig3 =
            boost::make_shared<af::StreamAppConfig>();

        if (!appConfig3->load(is3)) {
            MessageBox(NULL, "Cannot read config.ini", "Error", MB_OK | MB_ICONSTOP);
            return af::AppConfigPtr();
        }

        is3.close();

        appConfig->add(appConfig3);
    }

    std::ifstream is4("config-hard.ini");

    if (is4) {
        boost::shared_ptr<af::StreamAppConfig> appConfig4 =
            boost::make_shared<af::StreamAppConfig>();

        if (!appConfig4->load(is4)) {
            MessageBox(NULL, "Cannot read config-hard.ini", "Error", MB_OK | MB_ICONSTOP);
            return af::AppConfigPtr();
        }

        is4.close();

        appConfig->add(appConfig4);
    }

    return appConfig;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
    
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    srand(static_cast<unsigned int>(time(NULL)));

    std::istringstream is1(configIniStr);

    boost::shared_ptr<af::StreamAppConfig> appConfig1 =
        boost::make_shared<af::StreamAppConfig>();

    if (!appConfig1->load(is1)) {
        MessageBox(NULL, "Cannot read built-in config.ini", "Error", MB_OK | MB_ICONSTOP);
        return 1;
    }

    af::AppConfigPtr appConfig = getNormalAppConfig(appConfig1);

    if (!appConfig) {
        return 1;
    }

    af::AppConfigPtr easyAppConfig = getEasyAppConfig(appConfig1);

    if (!easyAppConfig) {
        return 1;
    }

    af::AppConfigPtr hardAppConfig = getHardAppConfig(appConfig1);

    if (!hardAppConfig) {
        return 1;
    }

    std::istringstream iss(appConfig->getLoggerConfig());

    log4cplus::spi::AppenderFactoryRegistry& reg
        = log4cplus::spi::getAppenderFactoryRegistry();
    LOG4CPLUS_REG_APPENDER(reg, GameLogAppender);

    log4cplus::PropertyConfigurator loggerConfigurator(iss);
    loggerConfigurator.configure();

    af::settings.init(appConfig, easyAppConfig, hardAppConfig);

    if (!platformWin32->init(af::settings.assets)) {        
        MessageBox(NULL, "Cannot init win32 platform", "Error", MB_OK | MB_ICONSTOP);
        return 1;
    }

    bool withConsole = false;

    log4cplus::SharedAppenderPtr consoleAppender = af::logger().getDefaultHierarchy().getRoot().getAppender("console");    
    if (consoleAppender && dynamic_cast<log4cplus::ConsoleAppender*>(consoleAppender.get())) {
        DoAllocConsole();
        withConsole = true;
    }

    LOG4CPLUS_INFO(af::logger(), "Starting...");

#ifdef USE_STEAM_API
    af::gameShell.reset(new af::SteamShell());

    if (!steamShell->init()) {
        if (af::settings.steamApiRequired) {
            startupFailed();
            return 1;
        } else {
            LOG4CPLUS_INFO(af::logger(), "Not using SteamAPI");
            af::gameShell.reset();
            af::gameShell.reset(new af::DummyShell());
        }
    }
#else
    af::gameShell.reset(new af::DummyShell());
#endif

    gamepadInit();

    InitKIMap();

    if (!OGLPreInit()) {
        startupFailed();
        return 1;
    }

    PopulateVideoModes();

    if (!OALInit()) {
        startupFailed();
        return 1;
    }

    bool res;

    if ((argc >= 2) && af::settings.debugKeys) {
        std::wstring startScript = argv[1], startAsset;
        if (argc >= 3) {
            startAsset = argv[2];
        }        
        res = game.init(std::string(startScript.begin(), startScript.end()),
            std::string(startAsset.begin(), startAsset.end()));
    } else {
        res = game.init();
    }

    if (!res) {
        startupFailed();
        return 1;
    }
    
    boost::thread thr(&gameThread);

    MSG msg;

    while (gRunning) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                gRunning = false;
            } else {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        if (game.render()) {
            SwapBuffers(gDC);
        } else if (gRunning) {
            changeVideoModeInternal();

            {
                boost::mutex::scoped_lock lock(cmMutex);
                cmDone = true;
            }

            cmCond.notify_one();

            {
                boost::mutex::scoped_lock lock(cmMutex);

                while (cmDone) {
                    cmCond.wait(lock);
                }
            }

            ShowWindow(gWnd, SW_SHOW);
            SetForegroundWindow(gWnd);
            SetFocus(gWnd);

            game.renderReload();            
        }
    }

    {
        {
            boost::mutex::scoped_lock lock(cmMutex);
            cmDone = true;
        }

        cmCond.notify_one();
    }

    game.cancelUpdate();

    thr.join();

    game.shutdown();

    destroyWindow();

    gamepadShutdown();

    platformWin32->shutdown();

    if (withConsole) {
        std::cout << "Press any key..." << std::endl;
        _getch();
    }

    return 0;
}
