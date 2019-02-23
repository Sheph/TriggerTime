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

#ifndef _OGL_H_
#define _OGL_H_

#include "af/Single.h"
#if defined(ANDROID) || defined(__ANDROID__)
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glext.h>
#endif

#ifndef GLAPIENTRY
#define GLAPIENTRY GL_APIENTRY
#endif

namespace af
{
    class OGL : public Single<OGL>
    {
    public:
        OGL();
        ~OGL();

        void (GLAPIENTRY* GenTextures)(GLsizei n, GLuint* textures);
        void (GLAPIENTRY* DeleteTextures)(GLsizei n, const GLuint* textures);
        void (GLAPIENTRY* BindTexture)(GLenum target, GLuint texture);
        void (GLAPIENTRY* TexImage2D)(GLenum target, GLint level,
                                      GLint internalFormat,
                                      GLsizei width, GLsizei height,
                                      GLint border, GLenum format, GLenum type,
                                      const GLvoid* pixels);
        void (GLAPIENTRY* TexParameteri)(GLenum target, GLenum pname, GLint param);
        void (GLAPIENTRY* ClearColor)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
        void (GLAPIENTRY* Clear)(GLbitfield mask);
        void (GLAPIENTRY* Viewport)(GLint x, GLint y,
                                    GLsizei width, GLsizei height);
        void (GLAPIENTRY* AttachShader)(GLuint program, GLuint shader);
        void (GLAPIENTRY* BindAttribLocation)(GLuint program, GLuint index, const GLchar* name);
        void (GLAPIENTRY* CompileShader)(GLuint shader);
        GLuint (GLAPIENTRY* CreateProgram)();
        GLuint (GLAPIENTRY* CreateShader)(GLenum type);
        void (GLAPIENTRY* DeleteProgram)(GLuint program);
        void (GLAPIENTRY* DeleteShader)(GLuint shader);
        void (GLAPIENTRY* DetachShader)(GLuint program, GLuint shader);
        void (GLAPIENTRY* DisableVertexAttribArray)(GLuint index);
        void (GLAPIENTRY* EnableVertexAttribArray)(GLuint index);
        void (GLAPIENTRY* LinkProgram)(GLuint program);
        void (GLAPIENTRY* ShaderSource)(GLuint shader, GLsizei count, const GLchar** string, const GLint* length);
        void (GLAPIENTRY* UseProgram)(GLuint program);
        void (GLAPIENTRY* GetProgramiv)(GLuint program, GLenum pname, GLint* params);
        void (GLAPIENTRY* GetProgramInfoLog)(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
        void (GLAPIENTRY* GetShaderiv)(GLuint shader, GLenum pname, GLint* params);
        void (GLAPIENTRY* GetShaderInfoLog)(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
        GLint (GLAPIENTRY* GetAttribLocation)(GLuint program, const GLchar* name);
        GLint (GLAPIENTRY* GetUniformLocation)(GLuint program, const GLchar* name);
        void (GLAPIENTRY* VertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer);
        void (GLAPIENTRY* DrawArrays)(GLenum mode, GLint first, GLsizei count);
        void (GLAPIENTRY* Uniform4fv)(GLint location, GLsizei count, const GLfloat* value);
        void (GLAPIENTRY* Enable)(GLenum cap);
        void (GLAPIENTRY* Disable)(GLenum cap);
        void (GLAPIENTRY* BlendFunc)(GLenum sfactor, GLenum dfactor);
        void (GLAPIENTRY* UniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
        void (GLAPIENTRY* Uniform1i)(GLint location, GLint v0);
        void (GLAPIENTRY* GetIntegerv)(GLenum pname, GLint* params);
        void (GLAPIENTRY* GenerateMipmap)(GLenum target);
        void (GLAPIENTRY* PointSize)(GLfloat size);
        void (GLAPIENTRY* LineWidth)(GLfloat width);
        void (GLAPIENTRY* GenFramebuffers)(GLsizei n, GLuint* framebuffers);
        void (GLAPIENTRY* DeleteFramebuffers)(GLsizei n, const GLuint* framebuffers);
        void (GLAPIENTRY* BindFramebuffer)(GLenum target, GLuint framebuffer);
        void (GLAPIENTRY* FramebufferTexture2D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
        GLenum (GLAPIENTRY* CheckFramebufferStatus)(GLenum target);
        void (GLAPIENTRY* Uniform1f)(GLint location, GLfloat v0);
        void (GLAPIENTRY* Uniform2f)(GLint location, GLfloat v0, GLfloat v1);
        void (GLAPIENTRY* ColorMask)(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
        void (GLAPIENTRY* StencilFunc)(GLenum func, GLint ref, GLuint mask);
        void (GLAPIENTRY* StencilOp)(GLenum fail, GLenum zfail, GLenum zpass);
        const GLubyte* (GLAPIENTRY* GetString)(GLenum name);
        void (GLAPIENTRY* Scissor)(GLint x, GLint y, GLsizei width, GLsizei height);
    };

    extern OGL ogl;
}

#endif
