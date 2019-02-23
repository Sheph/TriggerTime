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

#ifndef _RENDER_H_
#define _RENDER_H_

#include "af/Single.h"
#include "RenderQueue.h"
#include <boost/thread.hpp>
#include <set>

namespace af
{
    class RenderSimple
    {
    public:
        RenderSimple();
        RenderSimple(RenderBatch* batch);
        ~RenderSimple();

        void addVertex(GLfloat x, GLfloat y);
        void addVertices(const GLfloat* v, GLsizei count);

        void addTexCoord(GLfloat x, GLfloat y);
        void addTexCoords(const GLfloat* v, GLsizei count);

        void addTexCoord4(GLfloat x, GLfloat y, GLfloat r, GLfloat q);

        void addGeneric1(const GLfloat* v, GLsizei count);

        void addColors(const Color& color = Color(1.0f, 1.0f, 1.0f));
        void addColors(const GLfloat* v, GLsizei count);
        void addColor(const Color& color);

        void addColors2(const Color& color = Color(1.0f, 1.0f, 1.0f));

    private:
        RenderBatch* batch_;
    };

    class RenderTriangleFan
    {
    public:
        RenderTriangleFan();
        RenderTriangleFan(RenderBatch* batch);
        ~RenderTriangleFan();

        void addVertex(GLfloat x, GLfloat y);
        void addVertices(const GLfloat* v, GLsizei count);

        void addTexCoord(GLfloat x, GLfloat y);
        void addTexCoords(const GLfloat* v, GLsizei count);

        void addColors(const Color& color = Color(1.0f, 1.0f, 1.0f));

    private:
        RenderBatch* batch_;
        size_t b_;
    };

    class RenderLineStrip
    {
    public:
        RenderLineStrip();
        RenderLineStrip(RenderBatch* batch);
        ~RenderLineStrip();

        void addVertex(GLfloat x, GLfloat y);

        void addTexCoord(GLfloat x, GLfloat y);

        void addColors(const Color& color = Color(1.0f, 1.0f, 1.0f));

    private:
        RenderBatch* batch_;
        size_t b_;
    };

    class Renderer : public Single<Renderer>
    {
    public:
        Renderer();
        ~Renderer();

        void uploadTexture(const TexturePtr& texture,
                           const std::string& path,
                           bool delayed);

        void genTexture(const TexturePtr& texture, const Byte* data);

        void unloadTexture(UInt32 id);

        void lookAt(const b2Vec2& pos, float width, float height, float angle);

        void blendFunc(GLenum sfactor, GLenum dfactor);

        void saturation(float factor);

        void stencilSet(GLenum fail, GLenum pass);

        void stencilUse(GLenum func, GLint ref);

        void stencilEnd();

        void checkScissor(int x, int y, int w, int h);

        void setScissor(int x, int y, int w, int h);

        void endScissor();

        void setProgramDef(const TexturePtr& texture);

        void setProgramColor();

        void setProgramColorPoints(float pointSize);

        void setProgramColorLines(float pointSize);

        void setProgramDamage();

        void setProgramLight(bool diffuse, bool gammaCorrection);

        void setProgramShadow(const Color& ambient);

        void setProgramDiffuse(const Color& ambient);

        void setProgramBlur(UInt32 numBlur);

        void setProgramMask(const TexturePtr& texture);

        void setProgramProjTex(const TexturePtr& texture);

        void setProgramFlash(const TexturePtr& texture);

        RenderSimple renderPoints();

        RenderLineStrip renderLineStrip();

        RenderTriangleFan renderTriangleFan();

        RenderSimple renderTriangles();

        void swap();

        void cancelSwap();

        void cancelUpdate();

        /*
         * These will be called on rendering thread.
         */

        bool init();
        void shutdown();

        bool update();

        bool reload();

        UInt32 currentGeneration() const;
        void advanceGeneration();

    private:
        typedef std::map<TexturePtr, std::string> TextureUploads;
        typedef std::map<TexturePtr, Byte*> TextureGens;
        typedef std::set<UInt32> TextureUnloads;

        static GLuint createShader(const std::string& source,
                                   GLenum type);

        static GLuint createProgram(GLuint vertexShaderId,
                                    GLuint fragmentShaderId);

        static void createOrtho(GLfloat left, GLfloat right,
                                GLfloat bottom, GLfloat top,
                                GLfloat nearf, GLfloat farf,
                                float ortho[16]);

        static void rotateOrtho(const float ortho[16],
                                GLfloat x, GLfloat y, GLfloat angle,
                                float res[16]);

        bool initDefProgram(bool saturated);

        bool initColorProgram(bool saturated);

        bool initDamageProgram(bool saturated);

        bool initLightProgram(bool gammaCorrection, bool saturated);

        bool initShadowProgram();

        bool initDiffuseProgram();

        bool initBlurProgram(UInt32 width, UInt32 height, bool diffuse);

        bool initMaskProgram();

        bool initProjTexProgram(bool saturated);

        bool initFlashProgram(bool saturated);

        bool initLightmap(UInt32 width, UInt32 height);

        void flush();

        void processDefBatch(const RenderBatch* batch);

        void processColorBatch(const RenderBatch* batch);

        void processDamageBatch(const RenderBatch* batch);

        void processLightBatch(const RenderBatch* batch);

        void processShadowBatch(const RenderBatch* batch);

        void processDiffuseBatch(const RenderBatch* batch);

        void processBlurBatch(const RenderBatch* batch);

        void processMaskBatch(const RenderBatch* batch);

        void processProjTexBatch(const RenderBatch* batch);

        void processFlashBatch(const RenderBatch* batch);

        void unuseNoneProgram(int arg);
        void unuseDefProgram(int arg);
        void unuseColorProgram(int arg);
        void unuseDamageProgram(int arg);
        void unuseLightProgram(int arg);
        void unuseShadowProgram(int arg);
        void unuseDiffuseProgram(int arg);
        void unuseBlurProgram(int arg);
        void unuseMaskProgram(int arg);
        void unuseProjTexProgram(int arg);
        void unuseFlashProgram(int arg);

        void genTextureInternal(const TexturePtr& texture, Byte* data);

        /*
         * These can be changed only on main thread.
         */

        RenderQueue* queue_;
        RenderBatch* batch_;

        /*
         * Stuff below may be changed on rendering thread.
         */

        boost::mutex m_;
        boost::condition_variable c_;
        bool cancelSwap_;
        bool cancelUpdate_;
        TextureUploads textureUploads_;
        TextureGens textureGens_;
        TextureUnloads textureUnloads_;
        RenderQueue* activeQueue_;
        UInt64 lastTimeUs_;

        float ortho_[16];

        struct
        {
            GLuint vsId;
            GLuint fsId;
            GLuint programId;
            GLint projLocation;
            GLint posLocation;
            GLint texCoordLocation;
            GLint colorLocation;
            GLint saturationLocation;
            bool orthoDirty;
        } def_[2], damage_[2], projTex_[2];

        struct
        {
            GLuint vsId;
            GLuint fsId;
            GLuint programId;
            GLint projLocation;
            GLint posLocation;
            GLint colorLocation;
            GLint saturationLocation;
            bool orthoDirty;
        } color_[2];

        struct
        {
            GLuint vsId;
            GLuint fsId;
            GLuint programId;
            GLint projLocation;
            GLint posLocation;
            GLint colorLocation;
            GLint sLocation;
            GLint saturationLocation;
            bool orthoDirty;
        } light_[2][2];

        struct
        {
            GLuint vsId;
            GLuint fsId;
            GLuint programId;
            GLint ambientLocation;
            GLint posLocation;
            GLint texCoordLocation;
        } shadow_;

        struct
        {
            GLuint vsId;
            GLuint fsId;
            GLuint programId;
            GLint ambientLocation;
            GLint gammaLocation;
            GLint posLocation;
            GLint texCoordLocation;
        } diffuse_;

        struct
        {
            GLuint vsId;
            GLuint fsId;
            GLuint programId;
            GLint dirLocation;
            GLint posLocation;
            GLint texCoordLocation;
        } blur_[2];

        struct
        {
            GLuint vsId;
            GLuint fsId;
            GLuint programId;
            GLint projLocation;
            GLint posLocation;
            GLint texCoordLocation;
            bool orthoDirty;
        } mask_;

        struct
        {
            GLuint vsId;
            GLuint fsId;
            GLuint programId;
            GLint projLocation;
            GLint posLocation;
            GLint texCoordLocation;
            GLint colorLocation;
            GLint flashColorLocation;
            GLint saturationLocation;
            bool orthoDirty;
        } flash_[2];

        struct
        {
            UInt32 width;
            UInt32 height;
            GLuint texId[2];
            GLuint fbId[2];
            GLuint blurTexId;
            GLuint blurFbId;
        } lightmap_;

        GLuint lastProgram_;
        void (Renderer::* unuseLastProgram_)(int arg);
        int unuseLastProgramArg_;
        GLuint lastTexture_;
        GLenum blendSfactor_;
        GLenum blendDfactor_;
        bool stencilDirty_;

        UInt32 generation_;
    };

    extern Renderer renderer;
}

#endif
