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

#include "Renderer.h"
#include "Logger.h"
#include "Utils.h"
#include "Platform.h"
#include "Settings.h"
#include "af/PNGDecoder.h"
#include <boost/format.hpp>

namespace af
{
    static const std::string defVsSource =
        "attribute vec4 pos;\n"
        "uniform mat4 proj;\n"
        "attribute vec2 texCoord;\n"
        "attribute vec4 color;\n"
        "varying vec2 v_texCoord;\n"
        "varying vec4 v_color;\n"
        "void main()\n"
        "{\n"
        "    v_texCoord = texCoord;\n"
        "    v_color = color;\n"
        "    gl_Position = proj * pos;\n"
        "}\n";

    static const std::string defFsSource =
        "#ifdef GL_ES\n"
        "precision mediump float;\n"
        "#endif\n"
        "uniform sampler2D tex;\n"
        "varying vec2 v_texCoord;\n"
        "varying vec4 v_color;\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = texture2D(tex, v_texCoord) * v_color;\n"
        "}\n";

    static const std::string defSaturatedFsSource =
        "#ifdef GL_ES\n"
        "precision mediump float;\n"
        "#endif\n"
        "uniform sampler2D tex;\n"
        "varying vec2 v_texCoord;\n"
        "varying vec4 v_color;\n"
        "uniform float saturation;\n"
        "void main()\n"
        "{\n"
        "    const vec3 W = vec3(0.2125, 0.7154, 0.0721);\n"
        "    vec4 tmp = texture2D(tex, v_texCoord) * v_color;\n"
        "    gl_FragColor = vec4(mix(vec3(dot(tmp.rgb, W)), tmp.rgb, saturation), tmp.a);\n"
        "}\n";

    static const std::string colorVsSource =
        "attribute vec4 pos;\n"
        "uniform mat4 proj;\n"
        "attribute vec4 color;\n"
        "varying vec4 v_color;\n"
        "void main()\n"
        "{\n"
        "    v_color = color;\n"
        "    gl_Position = proj * pos;\n"
        "}\n";

    static const std::string colorSaturatedVsSource =
        "attribute vec4 pos;\n"
        "uniform mat4 proj;\n"
        "attribute vec4 color;\n"
        "varying vec4 v_color;\n"
        "uniform float saturation;\n"
        "void main()\n"
        "{\n"
        "    const vec3 W = vec3(0.2125, 0.7154, 0.0721);\n"
        "    v_color = vec4(mix(vec3(dot(color.rgb, W)), color.rgb, saturation), color.a);\n"
        "    gl_Position = proj * pos;\n"
        "}\n";

    static const std::string colorFsSource =
        "#ifdef GL_ES\n"
        "precision mediump float;\n"
        "#endif\n"
        "varying vec4 v_color;\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = v_color;\n"
        "}\n";

    static const std::string damageVsSource = defVsSource;

    static const std::string damageSaturatedVsSource =
        "attribute vec4 pos;\n"
        "uniform mat4 proj;\n"
        "attribute vec2 texCoord;\n"
        "attribute vec4 color;\n"
        "varying vec2 v_texCoord;\n"
        "varying vec4 v_color;\n"
        "uniform float saturation;\n"
        "void main()\n"
        "{\n"
        "    const vec3 W = vec3(0.2125, 0.7154, 0.0721);\n"
        "    v_texCoord = texCoord;\n"
        "    v_color = vec4(mix(vec3(dot(color.rgb, W)), color.rgb, saturation), color.a);\n"
        "    gl_Position = proj * pos;\n"
        "}\n";

    static const std::string damageFsSource =
        "#ifdef GL_ES\n"
        "precision mediump float;\n"
        "#endif\n"
        "varying vec2 v_texCoord;\n"
        "varying vec4 v_color;\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = 2.0 * length(v_texCoord - vec2(0.5, 0.5)) * v_color;\n"
        "}\n";

    static const std::string lightVsSource =
        "attribute vec4 pos;\n"
        "uniform mat4 proj;\n"
        "attribute vec4 color;\n"
        "attribute float s;\n"
        "varying vec4 v_color;\n"
        "void main()\n"
        "{\n"
        "    v_color = s * color;\n"
        "    gl_Position = proj * pos;\n"
        "}\n";

    static const std::string lightSaturatedVsSource =
        "attribute vec4 pos;\n"
        "uniform mat4 proj;\n"
        "attribute vec4 color;\n"
        "attribute float s;\n"
        "varying vec4 v_color;\n"
        "uniform float saturation;\n"
        "void main()\n"
        "{\n"
        "    const vec3 W = vec3(0.2125, 0.7154, 0.0721);\n"
        "    vec4 tmp = s * color;\n"
        "    v_color = vec4(mix(vec3(dot(tmp.rgb, W)), tmp.rgb, saturation), tmp.a);\n"
        "    gl_Position = proj * pos;\n"
        "}\n";

    static const std::string lightFsSource =
        "#ifdef GL_ES\n"
        "precision mediump float;\n"
        "#endif\n"
        "varying vec4 v_color;\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = %1%(v_color);\n"
        "}\n";

    static const std::string shadowVsSource =
        "attribute vec4 pos;\n"
        "attribute vec2 texCoord;\n"
        "varying vec2 v_texCoord;\n"
        "void main()\n"
        "{\n"
        "    v_texCoord = texCoord;\n"
        "    gl_Position = pos;\n"
        "}\n";

    static const std::string shadowFsSource =
        "#ifdef GL_ES\n"
        "precision mediump float;\n"
        "#endif\n"
        "uniform sampler2D tex;\n"
        "uniform vec4 ambient;\n"
        "varying vec2 v_texCoord;\n"
        "void main()\n"
        "{\n"
        "    vec4 c = texture2D(tex, v_texCoord);\n"
        "    gl_FragColor.rgb = c.rgb * c.a + ambient.rgb;\n"
        "    gl_FragColor.a = ambient.a - c.a;\n"
        "}\n";

    static const std::string diffuseVsSource =
        "attribute vec4 pos;\n"
        "attribute vec2 texCoord;\n"
        "varying vec2 v_texCoord;\n"
        "void main()\n"
        "{\n"
        "    v_texCoord = texCoord;\n"
        "    gl_Position = pos;\n"
        "}\n";

    static const std::string diffuseFsSource =
        "#ifdef GL_ES\n"
        "precision mediump float;\n"
        "#endif\n"
        "uniform sampler2D tex;\n"
        "uniform vec4 ambient;\n"
        "uniform float gamma;\n"
        "varying vec2 v_texCoord;\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor.rgb = pow(ambient.rgb + texture2D(tex, v_texCoord).rgb, vec3(1.0 / gamma));\n"
        "    gl_FragColor.a = 1.0;\n"
        "}\n";

    static const std::string blurVsSource =
        "attribute vec4 pos;\n"
        "uniform vec2 dir;\n"
        "attribute vec2 texCoord;\n"
        "varying vec2 v_texCoord0;\n"
        "varying vec2 v_texCoord1;\n"
        "varying vec2 v_texCoord2;\n"
        "varying vec2 v_texCoord3;\n"
        "varying vec2 v_texCoord4;\n"
        "#define FBO_W %1%.0\n"
        "#define FBO_H %2%.0\n"
        "const vec2 futher = vec2(3.2307692308 / FBO_W, 3.2307692308 / FBO_H );\n"
        "const vec2 closer = vec2(1.3846153846 / FBO_W, 1.3846153846 / FBO_H );\n"
        "void main()\n"
        "{\n"
        "    vec2 f = futher * dir;\n"
        "    vec2 c = closer * dir;\n"
        "    v_texCoord0 = texCoord - f;\n"
        "    v_texCoord1 = texCoord - c;\n"
        "    v_texCoord2 = texCoord;\n"
        "    v_texCoord3 = texCoord + c;\n"
        "    v_texCoord4 = texCoord + f;\n"
        "    gl_Position = pos;\n"
        "}\n";

    static const std::string blurFsSource =
        "#ifdef GL_ES\n"
        "precision lowp float;\n"
        "#define MED mediump\n"
        "#else\n"
        "#define MED\n"
        "#endif\n"
        "uniform sampler2D tex;\n"
        "varying MED vec2 v_texCoord0;\n"
        "varying MED vec2 v_texCoord1;\n"
        "varying MED vec2 v_texCoord2;\n"
        "varying MED vec2 v_texCoord3;\n"
        "varying MED vec2 v_texCoord4;\n"
        "const float center = 0.2270270270;\n"
        "const float close = 0.3162162162;\n"
        "const float far = 0.0702702703;\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor%1% = far * texture2D(tex, v_texCoord0)%1%\n"
        "        + close * texture2D(tex, v_texCoord1)%1%\n"
        "        + center * texture2D(tex, v_texCoord2)%1%\n"
        "        + close * texture2D(tex, v_texCoord3)%1%\n"
        "        + far * texture2D(tex, v_texCoord4)%1%;\n"
        "}\n";

    static const std::string maskVsSource =
        "attribute vec4 pos;\n"
        "uniform mat4 proj;\n"
        "attribute vec2 texCoord;\n"
        "varying vec2 v_texCoord;\n"
        "void main()\n"
        "{\n"
        "    v_texCoord = texCoord;\n"
        "    gl_Position = proj * pos;\n"
        "}\n";

    static const std::string maskFsSource =
        "#ifdef GL_ES\n"
        "precision mediump float;\n"
        "#endif\n"
        "uniform sampler2D tex;\n"
        "varying vec2 v_texCoord;\n"
        "void main()\n"
        "{\n"
        "    vec4 color = texture2D(tex, v_texCoord);\n"
        "    if (color.a == 0.0) {\n"
        "        discard;\n"
        "    }\n"
        "    gl_FragColor = color;\n"
        "}\n";

    static const std::string projTexVsSource =
        "attribute vec4 pos;\n"
        "uniform mat4 proj;\n"
        "attribute vec4 texCoord;\n"
        "attribute vec4 color;\n"
        "varying vec4 v_texCoord;\n"
        "varying vec4 v_color;\n"
        "void main()\n"
        "{\n"
        "    v_texCoord = texCoord;\n"
        "    v_color = color;\n"
        "    gl_Position = proj * pos;\n"
        "}\n";

    static const std::string projTexFsSource =
        "#ifdef GL_ES\n"
        "precision mediump float;\n"
        "#endif\n"
        "uniform sampler2D tex;\n"
        "varying vec4 v_texCoord;\n"
        "varying vec4 v_color;\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = texture2DProj(tex, v_texCoord) * v_color;\n"
        "}\n";

    static const std::string projTexSaturatedFsSource =
        "#ifdef GL_ES\n"
        "precision mediump float;\n"
        "#endif\n"
        "uniform sampler2D tex;\n"
        "varying vec4 v_texCoord;\n"
        "varying vec4 v_color;\n"
        "uniform float saturation;\n"
        "void main()\n"
        "{\n"
        "    const vec3 W = vec3(0.2125, 0.7154, 0.0721);\n"
        "    vec4 tmp = texture2DProj(tex, v_texCoord) * v_color;\n"
        "    gl_FragColor = vec4(mix(vec3(dot(tmp.rgb, W)), tmp.rgb, saturation), tmp.a);\n"
        "}\n";

    static const std::string flashVsSource =
        "attribute vec4 pos;\n"
        "uniform mat4 proj;\n"
        "attribute vec2 texCoord;\n"
        "attribute vec4 color;\n"
        "attribute vec4 flashColor;\n"
        "varying vec2 v_texCoord;\n"
        "varying vec4 v_color;\n"
        "varying vec4 v_flashColor;\n"
        "varying float v_flashAmount;\n"
        "void main()\n"
        "{\n"
        "    v_texCoord = texCoord;\n"
        "    v_color = color;\n"
        "    v_flashColor = flashColor;\n"
        "    gl_Position = proj * pos;\n"
        "}\n";

    static const std::string flashFsSource =
        "#ifdef GL_ES\n"
        "precision mediump float;\n"
        "#endif\n"
        "uniform sampler2D tex;\n"
        "varying vec2 v_texCoord;\n"
        "varying vec4 v_color;\n"
        "varying vec4 v_flashColor;\n"
        "void main()\n"
        "{\n"
        "    vec4 tmp = texture2D(tex, v_texCoord) * v_color;\n"
        "    gl_FragColor = vec4(mix(tmp.rgb, v_flashColor.rgb, v_flashColor.a), tmp.a);\n"
        "}\n";

    static const std::string flashSaturatedFsSource =
        "#ifdef GL_ES\n"
        "precision mediump float;\n"
        "#endif\n"
        "uniform sampler2D tex;\n"
        "varying vec2 v_texCoord;\n"
        "varying vec4 v_color;\n"
        "varying vec4 v_flashColor;\n"
        "uniform float saturation;\n"
        "void main()\n"
        "{\n"
        "    const vec3 W = vec3(0.2125, 0.7154, 0.0721);\n"
        "    vec4 tmp = texture2D(tex, v_texCoord) * v_color;\n"
        "    tmp.rgb = mix(tmp.rgb, v_flashColor.rgb, v_flashColor.a);\n"
        "    gl_FragColor = vec4(mix(vec3(dot(tmp.rgb, W)), tmp.rgb, saturation), tmp.a);\n"
        "}\n";

    Renderer renderer;

    template <>
    Single<Renderer>* Single<Renderer>::single = NULL;

    RenderSimple::RenderSimple()
    {
    }

    RenderSimple::RenderSimple(RenderBatch* batch)
    : batch_(batch)
    {
    }

    RenderSimple::~RenderSimple()
    {
    }

    void RenderSimple::addVertex(GLfloat x, GLfloat y)
    {
        batch_->vertices.push_back(x);
        batch_->vertices.push_back(y);
    }

    void RenderSimple::addVertices(const GLfloat* v, GLsizei count)
    {
        batch_->vertices.insert(batch_->vertices.end(), v, v + count * 2);
    }

    void RenderSimple::addTexCoord(GLfloat x, GLfloat y)
    {
        batch_->texCoords.push_back(x);
        batch_->texCoords.push_back(y);
    }

    void RenderSimple::addTexCoords(const GLfloat* v, GLsizei count)
    {
        batch_->texCoords.insert(batch_->texCoords.end(), v, v + count * 2);
    }

    void RenderSimple::addTexCoord4(GLfloat x, GLfloat y, GLfloat r, GLfloat q)
    {
        batch_->texCoords.push_back(x);
        batch_->texCoords.push_back(y);
        batch_->texCoords.push_back(r);
        batch_->texCoords.push_back(q);
    }

    void RenderSimple::addGeneric1(const GLfloat* v, GLsizei count)
    {
        batch_->generic1.insert(batch_->generic1.end(), v, v + count);
    }

    void RenderSimple::addColors(const Color& color)
    {
        appendColors(batch_->colors,
                     color,
                     (batch_->vertices.size() / 2) - (batch_->colors.size() / 4));
    }

    void RenderSimple::addColors(const GLfloat* v, GLsizei count)
    {
        batch_->colors.insert(batch_->colors.end(), v, v + count * 4);
    }

    void RenderSimple::addColor(const Color& color)
    {
        batch_->colors.insert(batch_->colors.end(), &color.rgba[0], &color.rgba[0] + 4);
    }

    void RenderSimple::addColors2(const Color& color)
    {
        appendColors(batch_->generic1,
                     color,
                     (batch_->vertices.size() / 2) - (batch_->generic1.size() / 4));
    }

    RenderTriangleFan::RenderTriangleFan()
    {
    }

    RenderTriangleFan::RenderTriangleFan(RenderBatch* batch)
    : batch_(batch),
      b_(batch->vertices.size())
    {
    }

    RenderTriangleFan::~RenderTriangleFan()
    {
    }

    void RenderTriangleFan::addVertex(GLfloat x, GLfloat y)
    {
        appendTriangleFan(batch_->vertices, x, y, b_);
    }

    void RenderTriangleFan::addVertices(const GLfloat* v, GLsizei count)
    {
        batch_->vertices.insert(batch_->vertices.end(), v, v + count * 2);
    }

    void RenderTriangleFan::addTexCoord(GLfloat x, GLfloat y)
    {
        appendTriangleFan(batch_->texCoords, x, y, b_);
    }

    void RenderTriangleFan::addTexCoords(const GLfloat* v, GLsizei count)
    {
        batch_->texCoords.insert(batch_->texCoords.end(), v, v + count * 2);
    }

    void RenderTriangleFan::addColors(const Color& color)
    {
        appendColors(batch_->colors,
                     color,
                     (batch_->vertices.size() / 2) - (batch_->colors.size() / 4));
    }

    RenderLineStrip::RenderLineStrip()
    {
    }

    RenderLineStrip::RenderLineStrip(RenderBatch* batch)
    : batch_(batch),
      b_(batch->vertices.size())
    {
    }

    RenderLineStrip::~RenderLineStrip()
    {
    }

    void RenderLineStrip::addVertex(GLfloat x, GLfloat y)
    {
        FastVector<GLfloat>& v = batch_->vertices;

        size_t i = v.size();

        if ((i - b_) >= 4) {
            v.push_back(v[i - 2]);
            v.push_back(v[i - 1]);
        }

        v.push_back(x);
        v.push_back(y);
    }

    void RenderLineStrip::addTexCoord(GLfloat x, GLfloat y)
    {
        FastVector<GLfloat>& v = batch_->texCoords;

        size_t i = v.size();

        if ((i - b_) >= 4) {
            v.push_back(v[i - 2]);
            v.push_back(v[i - 1]);
        }

        v.push_back(x);
        v.push_back(y);
    }

    void RenderLineStrip::addColors(const Color& color)
    {
        appendColors(batch_->colors,
                     color,
                     (batch_->vertices.size() / 2) - (batch_->colors.size() / 4));
    }

    Renderer::Renderer()
    : queue_(NULL),
      batch_(NULL),
      cancelSwap_(false),
      cancelUpdate_(false),
      activeQueue_(NULL),
      lastTimeUs_(0),
      generation_(0)
    {
    }

    Renderer::~Renderer()
    {
    }

    void Renderer::uploadTexture(const TexturePtr& texture,
                                 const std::string& path,
                                 bool delayed)
    {
        if (delayed) {
            boost::mutex::scoped_lock lock(m_);

            textureUploads_.insert(std::make_pair(texture, path));

            return;
        }

        PlatformIFStream is(path);

        PNGDecoder decoder(path, is);

        std::vector<Byte> data;

        if (decoder.init(false) && decoder.decode(data)) {
            GLuint curTex = 0;
            ogl.GetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&curTex);

            GLuint id;
            ogl.GenTextures(1, &id);

            LOG4CPLUS_DEBUG(logger(), "Uploading " << path << "("
                << texture->width() << "x" << texture->height() << ", "
                << ((texture->wrapX() == Texture::WrapModeClamp) ? "clampX" : "repX")
                << ", "
                << ((texture->wrapY() == Texture::WrapModeClamp) ? "clampY" : "repY")
                << ") = " << id << "...");

            ogl.BindTexture(GL_TEXTURE_2D, id);

            ogl.TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                           texture->width(), texture->height(), 0, GL_RGBA,
                           GL_UNSIGNED_BYTE, reinterpret_cast<const GLvoid*>(&data[0]));

            if (settings.trilinearFilter) {
                ogl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            } else {
                ogl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
            }
            ogl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            if (texture->wrapX() == Texture::WrapModeClamp) {
                ogl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            }
            if (texture->wrapY() == Texture::WrapModeClamp) {
                ogl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }
            ogl.GenerateMipmap(GL_TEXTURE_2D);

            ogl.BindTexture(GL_TEXTURE_2D, curTex);

            texture->setId(generation_, id);
        }
    }

    void Renderer::genTexture(const TexturePtr& texture, const Byte* data)
    {
        Byte* dc = new Byte[texture->width() * texture->height() * 4];

        for (UInt32 i = 0; i < texture->height(); ++i) {
            ::memcpy(dc + texture->width() * 4 * (texture->height() - 1 - i), data + texture->width() * 4 * i, texture->width() * 4);
        }

        boost::mutex::scoped_lock lock(m_);

        textureGens_.insert(std::make_pair(texture, dc));
    }

    void Renderer::genTextureInternal(const TexturePtr& texture, Byte* data)
    {
        GLuint curTex = 0;
        ogl.GetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&curTex);

        GLuint id;
        ogl.GenTextures(1, &id);

        LOG4CPLUS_DEBUG(logger(), "Uploading ("
            << texture->width() << "x" << texture->height() << ", "
            << ((texture->wrapX() == Texture::WrapModeClamp) ? "clampX" : "repX")
            << ", "
            << ((texture->wrapY() == Texture::WrapModeClamp) ? "clampY" : "repY")
            << ") = " << id << "...");

        ogl.BindTexture(GL_TEXTURE_2D, id);

        ogl.TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                       texture->width(), texture->height(), 0, GL_RGBA,
                       GL_UNSIGNED_BYTE, reinterpret_cast<const GLvoid*>(data));

        if (settings.trilinearFilter) {
            ogl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        } else {
            ogl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        }
        ogl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if (texture->wrapX() == Texture::WrapModeClamp) {
            ogl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        }
        if (texture->wrapY() == Texture::WrapModeClamp) {
            ogl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        ogl.GenerateMipmap(GL_TEXTURE_2D);

        ogl.BindTexture(GL_TEXTURE_2D, curTex);

        texture->setId(generation_, id);

        delete [] data;
    }

    void Renderer::unloadTexture(UInt32 id)
    {
        boost::mutex::scoped_lock lock(m_);

        textureUnloads_.insert(id);
    }

    void Renderer::lookAt(const b2Vec2& pos, float width, float height, float angle)
    {
        flush();

        if (angle == 0.0f) {
            createOrtho(pos.x - (width / 2),
                        pos.x + (width / 2),
                        pos.y - (height / 2),
                        pos.y + (height / 2),
                        0.0f, 1.0f,
                        batch_->ortho);
        } else {
            float tmpOrtho[16];

            createOrtho(pos.x - (width / 2),
                        pos.x + (width / 2),
                        pos.y - (height / 2),
                        pos.y + (height / 2),
                        0.0f, 1.0f,
                        tmpOrtho);

            rotateOrtho(tmpOrtho, pos.x, pos.y, angle, batch_->ortho);
        }

        batch_->orthoDirty = true;
    }

    void Renderer::blendFunc(GLenum sfactor, GLenum dfactor)
    {
        if ((batch_->blendSfactor != sfactor) ||
            (batch_->blendDfactor != dfactor)) {
            flush();
        }

        batch_->blendSfactor = sfactor;
        batch_->blendDfactor = dfactor;
    }

    void Renderer::saturation(float factor)
    {
        if (batch_->saturation != factor) {
            flush();
        }

        batch_->saturation = factor;
    }

    void Renderer::stencilSet(GLenum fail, GLenum pass)
    {
        flush();

        batch_->stencilSet = true;
        batch_->stencilFail = fail;
        batch_->stencilPass = pass;
    }

    void Renderer::stencilUse(GLenum func, GLint ref)
    {
        flush();

        batch_->stencilUse = true;
        batch_->stencilFunc = func;
        batch_->stencilRef = ref;
    }

    void Renderer::stencilEnd()
    {
        flush();

        batch_->stencilSet = false;
        batch_->stencilUse = false;
    }

    void Renderer::checkScissor(int x, int y, int w, int h)
    {
        if (!batch_->scissor || (batch_->scissorX != x) ||
            (batch_->scissorY != y) || (batch_->scissorW != w) ||
            (batch_->scissorH != h)) {
            flush();
        }
    }

    void Renderer::setScissor(int x, int y, int w, int h)
    {
        batch_->scissor = true;
        batch_->scissorX = x;
        batch_->scissorY = y;
        batch_->scissorW = w;
        batch_->scissorH = h;
    }

    void Renderer::endScissor()
    {
        if (batch_->scissor) {
            flush();
            batch_->scissor = false;
        }
    }

    void Renderer::setProgramDef(const TexturePtr& texture)
    {
        if (!batch_->empty() &&
            ((batch_->programId != RenderBatch::ProgramIdDef) ||
             (batch_->texture != texture))) {
            flush();
        }

        batch_->programId = RenderBatch::ProgramIdDef;
        batch_->texture = texture;
    }

    void Renderer::setProgramColor()
    {
        if (!batch_->empty() &&
            ((batch_->programId != RenderBatch::ProgramIdColor) ||
             (batch_->mode != GL_TRIANGLES))) {
            flush();
        }

        batch_->programId = RenderBatch::ProgramIdColor;
        batch_->mode = GL_TRIANGLES;
        batch_->pointSize = 0.0f;
    }

    void Renderer::setProgramColorPoints(float pointSize)
    {
        if (!batch_->empty() &&
            ((batch_->programId != RenderBatch::ProgramIdColor) ||
             (batch_->mode != GL_POINTS) ||
             (batch_->pointSize != pointSize))) {
            flush();
        }

        batch_->programId = RenderBatch::ProgramIdColor;
        batch_->mode = GL_POINTS;
        batch_->pointSize = pointSize;
    }

    void Renderer::setProgramColorLines(float pointSize)
    {
        if (!batch_->empty() &&
            ((batch_->programId != RenderBatch::ProgramIdColor) ||
             (batch_->mode != GL_LINES) ||
             (batch_->pointSize != pointSize))) {
            flush();
        }

        batch_->programId = RenderBatch::ProgramIdColor;
        batch_->mode = GL_LINES;
        batch_->pointSize = pointSize;
    }

    void Renderer::setProgramDamage()
    {
        if (!batch_->empty() &&
            (batch_->programId != RenderBatch::ProgramIdDamage)) {
            flush();
        }

        batch_->programId = RenderBatch::ProgramIdDamage;
    }

    void Renderer::setProgramLight(bool diffuse, bool gammaCorrection)
    {
        RenderBatch::ProgramId progId = gammaCorrection ?
            RenderBatch::ProgramIdLightGamma : RenderBatch::ProgramIdLight;

        if (!batch_->empty() &&
            ((batch_->programId != progId) || (batch_->diffuse != diffuse))) {
            flush();
        }

        batch_->programId = progId;
        batch_->diffuse = diffuse;
    }

    void Renderer::setProgramShadow(const Color& ambient)
    {
        if (!batch_->empty() &&
            ((batch_->programId != RenderBatch::ProgramIdShadow) ||
             (memcmp(batch_->ambient, ambient.rgba, sizeof(ambient.rgba)) != 0))) {
            flush();
        }

        batch_->programId = RenderBatch::ProgramIdShadow;
        memcpy(batch_->ambient, ambient.rgba, sizeof(ambient.rgba));
    }

    void Renderer::setProgramDiffuse(const Color& ambient)
    {
        if (!batch_->empty() &&
            ((batch_->programId != RenderBatch::ProgramIdDiffuse) ||
             (memcmp(batch_->ambient, ambient.rgba, sizeof(ambient.rgba)) != 0))) {
            flush();
        }

        batch_->programId = RenderBatch::ProgramIdDiffuse;
        memcpy(batch_->ambient, ambient.rgba, sizeof(ambient.rgba));
    }

    void Renderer::setProgramBlur(UInt32 numBlur)
    {
        if (!batch_->empty() &&
            ((batch_->programId != RenderBatch::ProgramIdBlur) ||
             (batch_->numBlur != numBlur))) {
            flush();
        }

        batch_->programId = RenderBatch::ProgramIdBlur;
        batch_->numBlur = numBlur;
    }

    void Renderer::setProgramMask(const TexturePtr& texture)
    {
        if (!batch_->empty() &&
            ((batch_->programId != RenderBatch::ProgramIdMask) ||
             (batch_->texture != texture))) {
            flush();
        }

        batch_->programId = RenderBatch::ProgramIdMask;
        batch_->texture = texture;
    }

    void Renderer::setProgramProjTex(const TexturePtr& texture)
    {
        if (!batch_->empty() &&
            ((batch_->programId != RenderBatch::ProgramIdProjTex) ||
             (batch_->texture != texture))) {
            flush();
        }

        batch_->programId = RenderBatch::ProgramIdProjTex;
        batch_->texture = texture;
    }

    void Renderer::setProgramFlash(const TexturePtr& texture)
    {
        if (!batch_->empty() &&
            ((batch_->programId != RenderBatch::ProgramIdFlash) ||
             (batch_->texture != texture))) {
            flush();
        }

        batch_->programId = RenderBatch::ProgramIdFlash;
        batch_->texture = texture;
    }

    RenderSimple Renderer::renderPoints()
    {
        return RenderSimple(batch_);
    }

    RenderLineStrip Renderer::renderLineStrip()
    {
        return RenderLineStrip(batch_);
    }

    RenderTriangleFan Renderer::renderTriangleFan()
    {
        return RenderTriangleFan(batch_);
    }

    RenderSimple Renderer::renderTriangles()
    {
        return RenderSimple(batch_);
    }

    void Renderer::swap()
    {
        flush();

        batch_->saturation = 1.0f;

        {
            boost::mutex::scoped_lock lock(m_);

            while (!cancelSwap_ && activeQueue_) {
                c_.wait(lock);
            }

            if (cancelSwap_) {
                cancelSwap_ = false;
                renderQueuePool.release(queue_);
                queue_ = renderQueuePool.allocate();
                return;
            }

            activeQueue_ = queue_;
            queue_ = renderQueuePool.allocate();
        }

        c_.notify_one();
    }

    void Renderer::cancelSwap()
    {
        {
            boost::mutex::scoped_lock lock(m_);

            cancelSwap_ = true;
        }

        c_.notify_one();
    }

    void Renderer::cancelUpdate()
    {
        {
            boost::mutex::scoped_lock lock(m_);

            cancelUpdate_ = true;
        }

        c_.notify_one();
    }

    bool Renderer::init()
    {
        queue_ = renderQueuePool.allocate();
        batch_ = renderBatchPool.allocate();

        return true;
    }

    void Renderer::shutdown()
    {
        for (TextureGens::const_iterator it = textureGens_.begin();
            it != textureGens_.end(); ++it) {
            delete [] it->second;
        }

        for (TextureUnloads::const_iterator it = textureUnloads_.begin();
            it != textureUnloads_.end(); ++it) {
            GLuint id = *it;

            LOG4CPLUS_DEBUG(logger(), "Unload texture " << id);

            ogl.DeleteTextures(1, &id);
        }

        textureUploads_.clear();
        textureGens_.clear();
        textureUnloads_.clear();

        renderQueuePool.release(queue_);
        queue_ = NULL;
        renderBatchPool.release(batch_);
        batch_ = NULL;
        renderQueuePool.release(activeQueue_);
        activeQueue_ = NULL;

        ogl.UseProgram(0);

        for (int j = 0; j < 2; ++j) {
            ogl.DetachShader(def_[j].programId, def_[j].vsId);
            ogl.DetachShader(def_[j].programId, def_[j].fsId);
            ogl.DeleteShader(def_[j].vsId);
            ogl.DeleteShader(def_[j].fsId);
            ogl.DeleteProgram(def_[j].programId);

            ogl.DetachShader(color_[j].programId, color_[j].vsId);
            ogl.DetachShader(color_[j].programId, color_[j].fsId);
            ogl.DeleteShader(color_[j].vsId);
            ogl.DeleteShader(color_[j].fsId);
            ogl.DeleteProgram(color_[j].programId);

            ogl.DetachShader(damage_[j].programId, damage_[j].vsId);
            ogl.DetachShader(damage_[j].programId, damage_[j].fsId);
            ogl.DeleteShader(damage_[j].vsId);
            ogl.DeleteShader(damage_[j].fsId);
            ogl.DeleteProgram(damage_[j].programId);

            for (int i = 0; i < 2; ++i) {
                ogl.DetachShader(light_[i][j].programId, light_[i][j].vsId);
                ogl.DetachShader(light_[i][j].programId, light_[i][j].fsId);
                ogl.DeleteShader(light_[i][j].vsId);
                ogl.DeleteShader(light_[i][j].fsId);
                ogl.DeleteProgram(light_[i][j].programId);
            }
        }

        ogl.DetachShader(shadow_.programId, shadow_.vsId);
        ogl.DetachShader(shadow_.programId, shadow_.fsId);
        ogl.DeleteShader(shadow_.vsId);
        ogl.DeleteShader(shadow_.fsId);
        ogl.DeleteProgram(shadow_.programId);

        ogl.DetachShader(diffuse_.programId, diffuse_.vsId);
        ogl.DetachShader(diffuse_.programId, diffuse_.fsId);
        ogl.DeleteShader(diffuse_.vsId);
        ogl.DeleteShader(diffuse_.fsId);
        ogl.DeleteProgram(diffuse_.programId);

        for (int i = 0; i < 2; ++i) {
            ogl.DetachShader(blur_[i].programId, blur_[i].vsId);
            ogl.DetachShader(blur_[i].programId, blur_[i].fsId);
            ogl.DeleteShader(blur_[i].vsId);
            ogl.DeleteShader(blur_[i].fsId);
            ogl.DeleteProgram(blur_[i].programId);
        }

        ogl.DetachShader(mask_.programId, mask_.vsId);
        ogl.DetachShader(mask_.programId, mask_.fsId);
        ogl.DeleteShader(mask_.vsId);
        ogl.DeleteShader(mask_.fsId);
        ogl.DeleteProgram(mask_.programId);

        for (int j = 0; j < 2; ++j) {
            ogl.DetachShader(projTex_[j].programId, projTex_[j].vsId);
            ogl.DetachShader(projTex_[j].programId, projTex_[j].fsId);
            ogl.DeleteShader(projTex_[j].vsId);
            ogl.DeleteShader(projTex_[j].fsId);
            ogl.DeleteProgram(projTex_[j].programId);
        }

        for (int j = 0; j < 2; ++j) {
            ogl.DetachShader(flash_[j].programId, flash_[j].vsId);
            ogl.DetachShader(flash_[j].programId, flash_[j].fsId);
            ogl.DeleteShader(flash_[j].vsId);
            ogl.DeleteShader(flash_[j].fsId);
            ogl.DeleteProgram(flash_[j].programId);
        }

        ogl.DeleteTextures(2, &lightmap_.texId[0]);
        ogl.DeleteFramebuffers(2, &lightmap_.fbId[0]);
        ogl.DeleteTextures(1, &lightmap_.blurTexId);
        ogl.DeleteFramebuffers(1, &lightmap_.blurFbId);
    }

    bool Renderer::update()
    {
        UInt64 timeUs = getTimeUs();

        UInt32 dt = settings.minRenderDt;

        if (lastTimeUs_ != 0) {
            dt = static_cast<UInt32>(timeUs - lastTimeUs_);
        }

        if (dt < settings.minRenderDt) {
            boost::this_thread::sleep(boost::posix_time::microseconds(settings.minRenderDt - dt));
            timeUs = getTimeUs();
        }

        lastTimeUs_ = timeUs;

        TextureUploads textureUploads;
        TextureGens textureGens;
        TextureUnloads textureUnloads;

        {
            boost::mutex::scoped_lock lock(m_);

            while (!cancelUpdate_ && !activeQueue_) {
                c_.wait(lock);
            }

            if (cancelUpdate_) {
                renderQueuePool.release(activeQueue_);
                activeQueue_ = NULL;
                cancelUpdate_ = false;
                return false;
            }

            textureUploads = textureUploads_;
            textureGens = textureGens_;
            textureUnloads = textureUnloads_;

            textureUploads_.clear();
            textureGens_.clear();
            textureUnloads_.clear();
        }

        for (TextureUploads::const_iterator it = textureUploads.begin();
             it != textureUploads.end(); ++it) {
            uploadTexture(it->first, it->second, false);
        }

        for (TextureGens::const_iterator it = textureGens.begin();
             it != textureGens.end(); ++it) {
            genTextureInternal(it->first, it->second);
        }

        for (TextureUnloads::const_iterator it = textureUnloads.begin();
             it != textureUnloads.end(); ++it) {
            GLuint id = *it;

            LOG4CPLUS_DEBUG(logger(), "Unload texture " << id);

            ogl.DeleteTextures(1, &id);
        }

        if (stencilDirty_) {
            ogl.Clear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            stencilDirty_ = false;
        } else {
            ogl.Clear(GL_COLOR_BUFFER_BIT);
        }

        for (RenderBatchList::const_iterator it = activeQueue_->batches().begin();
             it != activeQueue_->batches().end();
             ++it) {
            const RenderBatch* batch = *it;

            if (batch->orthoDirty) {
                memcpy(ortho_, batch->ortho, sizeof(ortho_));
                def_[0].orthoDirty = true;
                def_[1].orthoDirty = true;
                color_[0].orthoDirty = true;
                color_[1].orthoDirty = true;
                damage_[0].orthoDirty = true;
                damage_[1].orthoDirty = true;
                light_[0][0].orthoDirty = true;
                light_[1][0].orthoDirty = true;
                light_[0][1].orthoDirty = true;
                light_[1][1].orthoDirty = true;
                mask_.orthoDirty = true;
                projTex_[0].orthoDirty = true;
                projTex_[1].orthoDirty = true;
                flash_[0].orthoDirty = true;
                flash_[1].orthoDirty = true;
            }

            if ((batch->blendSfactor != blendSfactor_) ||
                (batch->blendDfactor != blendDfactor_)) {
                blendSfactor_ = batch->blendSfactor;
                blendDfactor_ = batch->blendDfactor;
                ogl.BlendFunc(blendSfactor_, blendDfactor_);
            }

            if (batch->stencilSet && batch->stencilUse) {
                ogl.Enable(GL_STENCIL_TEST);
                ogl.StencilFunc(batch->stencilFunc, batch->stencilRef, 0xFF);
                ogl.StencilOp(batch->stencilFail, batch->stencilFail, batch->stencilPass);
                stencilDirty_ = true;
            } else if (batch->stencilSet) {
                ogl.Enable(GL_STENCIL_TEST);
                ogl.ColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
                ogl.StencilFunc(GL_ALWAYS, 1, 0xFF);
                ogl.StencilOp(batch->stencilFail, batch->stencilFail, batch->stencilPass);
                stencilDirty_ = true;
            } else if (batch->stencilUse) {
                ogl.Enable(GL_STENCIL_TEST);
                ogl.StencilFunc(batch->stencilFunc, batch->stencilRef, 0xFF);
                ogl.StencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
            }

            if (batch->scissor) {
                ogl.Enable(GL_SCISSOR_TEST);
                ogl.Scissor(batch->scissorX, batch->scissorY, batch->scissorW, batch->scissorH);
            }

            switch (batch->programId) {
            case RenderBatch::ProgramIdDef:
                processDefBatch(batch);
                break;
            case RenderBatch::ProgramIdColor:
                processColorBatch(batch);
                break;
            case RenderBatch::ProgramIdDamage:
                processDamageBatch(batch);
                break;
            case RenderBatch::ProgramIdLight:
            case RenderBatch::ProgramIdLightGamma:
                processLightBatch(batch);
                break;
            case RenderBatch::ProgramIdShadow:
                processShadowBatch(batch);
                break;
            case RenderBatch::ProgramIdDiffuse:
                processDiffuseBatch(batch);
                break;
            case RenderBatch::ProgramIdBlur:
                processBlurBatch(batch);
                break;
            case RenderBatch::ProgramIdMask:
                processMaskBatch(batch);
                break;
            case RenderBatch::ProgramIdProjTex:
                processProjTexBatch(batch);
                break;
            case RenderBatch::ProgramIdFlash:
                processFlashBatch(batch);
                break;
            default:
                assert(false);
                break;
            }

            if (batch->scissor) {
                ogl.Disable(GL_SCISSOR_TEST);
            }

            if (batch->stencilSet && batch->stencilUse) {
                ogl.Disable(GL_STENCIL_TEST);
            } else if (batch->stencilSet) {
                ogl.ColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                ogl.Disable(GL_STENCIL_TEST);
            } else if (batch->stencilUse) {
                ogl.Disable(GL_STENCIL_TEST);
            }
        }

        {
            boost::mutex::scoped_lock lock(m_);

            renderQueuePool.release(activeQueue_);
            activeQueue_ = NULL;
        }

        c_.notify_one();

        return true;
    }

    bool Renderer::reload()
    {
        LOG4CPLUS_INFO(logger(), "OpenGL vendor: " << ogl.GetString(GL_VENDOR));
        LOG4CPLUS_INFO(logger(), "OpenGL renderer: " << ogl.GetString(GL_RENDERER));
        LOG4CPLUS_INFO(logger(), "OpenGL version: " << ogl.GetString(GL_VERSION));

        ogl.Viewport(settings.viewX, settings.viewY, settings.viewWidth, settings.viewHeight);
        ogl.ClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        ogl.Enable(GL_BLEND);
        ogl.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        ogl.Disable(GL_DEPTH_TEST);
        ogl.Disable(GL_CULL_FACE);

        if (!initDefProgram(false)) {
            return false;
        }

        if (!initDefProgram(true)) {
            return false;
        }

        if (!initColorProgram(false)) {
            return false;
        }

        if (!initColorProgram(true)) {
            return false;
        }

        if (!initDamageProgram(false)) {
            return false;
        }

        if (!initDamageProgram(true)) {
            return false;
        }

        if (!initLightProgram(false, false)) {
            return false;
        }

        if (!initLightProgram(true, false)) {
            return false;
        }

        if (!initLightProgram(false, true)) {
            return false;
        }

        if (!initLightProgram(true, true)) {
            return false;
        }

        if (!initShadowProgram()) {
            return false;
        }

        if (!initDiffuseProgram()) {
            return false;
        }

        UInt32 lightMapWidth = settings.viewWidth / settings.light.lightmapScale;
        UInt32 lightMapHeight = settings.viewHeight / settings.light.lightmapScale;

        if (!initBlurProgram(lightMapWidth, lightMapHeight, false)) {
            return false;
        }

        if (!initBlurProgram(lightMapWidth, lightMapHeight, true)) {
            return false;
        }

        if (!initMaskProgram()) {
            return false;
        }

        if (!initProjTexProgram(false)) {
            return false;
        }

        if (!initProjTexProgram(true)) {
            return false;
        }

        if (!initFlashProgram(false)) {
            return false;
        }

        if (!initFlashProgram(true)) {
            return false;
        }

        if (!initLightmap(lightMapWidth, lightMapHeight)) {
            return false;
        }

        GLint sampleBuffers = 0;
        GLint samples = 0;

        ogl.GetIntegerv(GL_SAMPLE_BUFFERS, &sampleBuffers);
        ogl.GetIntegerv(GL_SAMPLES, &samples);

        lastProgram_ = 0;
        unuseLastProgram_ = &Renderer::unuseNoneProgram;
        lastTexture_ = 0;
        blendSfactor_ = GL_SRC_ALPHA;
        blendDfactor_ = GL_ONE_MINUS_SRC_ALPHA;
        stencilDirty_ = true;

        LOG4CPLUS_INFO(logger(), "sample_buffers = " << sampleBuffers << ", samples = " << samples);
        LOG4CPLUS_INFO(logger(), "texture filter: " << (settings.trilinearFilter ? "trilinear" : "bilinear"));

        return true;
    }

    UInt32 Renderer::currentGeneration() const
    {
        return generation_;
    }

    void Renderer::advanceGeneration()
    {
        ++generation_;
    }

    GLuint Renderer::createShader(const std::string& source,
                                  GLenum type)
    {
        GLuint shader = ogl.CreateShader(type);

        if (!shader) {
            LOG4CPLUS_ERROR(logger(), "Unable to create shader type = " << type);
            return 0;
        }

        const char* str = source.c_str();

        ogl.ShaderSource(shader, 1, &str, NULL);
        ogl.CompileShader(shader);

        GLint tmp = 0;

        ogl.GetShaderiv(shader, GL_COMPILE_STATUS, &tmp);

        if (!tmp) {
            tmp = 0;
            ogl.GetShaderiv(shader, GL_INFO_LOG_LENGTH, &tmp);

            std::string buff(tmp, 0);

            ogl.GetShaderInfoLog(shader, buff.size(), NULL, &buff[0]);

            LOG4CPLUS_ERROR(logger(), "Unable to compile shader (type = " << type << ") - " << buff);

            ogl.DeleteShader(shader);

            return 0;
        }

        return shader;
    }

    GLuint Renderer::createProgram(GLuint vertexShaderId,
                                   GLuint fragmentShaderId)
    {
        GLuint program = ogl.CreateProgram();

        if (!program) {
            LOG4CPLUS_ERROR(logger(), "Unable to create program");
            return 0;
        }

        ogl.AttachShader(program, vertexShaderId);
        ogl.AttachShader(program, fragmentShaderId);
        ogl.LinkProgram(program);

        GLint tmp = 0;

        ogl.GetProgramiv(program, GL_LINK_STATUS, &tmp);

        if (!tmp) {
            tmp = 0;
            ogl.GetProgramiv(program, GL_INFO_LOG_LENGTH, &tmp);

            std::string buff(tmp, 0);

            ogl.GetProgramInfoLog(program, buff.size(), NULL, &buff[0]);

            LOG4CPLUS_ERROR(logger(), "Unable to link program - " << buff);

            ogl.DeleteProgram(program);

            return 0;
        }

        return program;
    }

    void Renderer::createOrtho(GLfloat left, GLfloat right,
                               GLfloat bottom, GLfloat top,
                               GLfloat nearf, GLfloat farf,
                               float ortho[16])
    {
        float a = 2.0f / (right - left);
        float b = 2.0f / (top - bottom);
        float c = -2.0f / (farf - nearf);

        float tx = -(right + left) / (right - left);
        float ty = -(top + bottom) / (top - bottom);
        float tz = -(farf + nearf) / (farf - nearf);

        memset(ortho, 0, sizeof(float[16]));

        ortho[0] = a;
        ortho[5] = b;
        ortho[10] = c;
        ortho[12] = tx;
        ortho[13] = ty;
        ortho[14] = tz;
        ortho[15] = 1.0f;
    }

    void Renderer::rotateOrtho(const float ortho[16],
                               GLfloat x, GLfloat y, GLfloat angle,
                               float res[16])
    {
        float cosa = cosf(angle);
        float sina = sinf(angle);

        float rot[16] = {
            cosa, sina, 0, 0,
            -sina, cosa, 0, 0,
            0, 0, 1.0f, 0,
            x * (1.0f - cosa) + y * sina, y * (1.0f - cosa) - x * sina, 0, 1.0f
        };

        int i, j, k;

        for (i = 0; i < 4; ++i) {
            for (j = 0; j < 4; ++j) {
                res[i * 4 + j] = 0.0f;
                for (k = 0; k < 4; ++k) {
                    res[i * 4 + j] += ortho[k * 4 + j] * rot[i * 4 + k];
                }
            }
        }
    }

    bool Renderer::initDefProgram(bool saturated)
    {
        int i = saturated ? 1 : 0;

        def_[i].vsId = createShader(defVsSource, GL_VERTEX_SHADER);

        if (!def_[i].vsId) {
            return false;
        }

        def_[i].fsId = createShader((saturated ? defSaturatedFsSource : defFsSource), GL_FRAGMENT_SHADER);

        if (!def_[i].fsId) {
            return false;
        }

        def_[i].programId = createProgram(def_[i].vsId, def_[i].fsId);

        if (!def_[i].programId) {
            return false;
        }

        def_[i].projLocation = ogl.GetUniformLocation(def_[i].programId, "proj");
        assert(def_[i].projLocation >= 0);
        def_[i].posLocation = ogl.GetAttribLocation(def_[i].programId, "pos");
        assert(def_[i].posLocation >= 0);
        def_[i].texCoordLocation = ogl.GetAttribLocation(def_[i].programId, "texCoord");
        assert(def_[i].texCoordLocation >= 0);
        def_[i].colorLocation = ogl.GetAttribLocation(def_[i].programId, "color");
        assert(def_[i].colorLocation >= 0);

        if (saturated) {
            def_[i].saturationLocation = ogl.GetUniformLocation(def_[i].programId, "saturation");
            assert(def_[i].saturationLocation >= 0);
        }

        def_[i].orthoDirty = true;

        return true;
    }

    bool Renderer::initColorProgram(bool saturated)
    {
        int i = saturated ? 1 : 0;

        color_[i].vsId = createShader((saturated ? colorSaturatedVsSource : colorVsSource), GL_VERTEX_SHADER);

        if (!color_[i].vsId) {
            return false;
        }

        color_[i].fsId = createShader(colorFsSource, GL_FRAGMENT_SHADER);

        if (!color_[i].fsId) {
            return false;
        }

        color_[i].programId = createProgram(color_[i].vsId, color_[i].fsId);

        if (!color_[i].programId) {
            return false;
        }

        color_[i].projLocation = ogl.GetUniformLocation(color_[i].programId, "proj");
        assert(color_[i].projLocation >= 0);
        color_[i].posLocation = ogl.GetAttribLocation(color_[i].programId, "pos");
        assert(color_[i].posLocation >= 0);
        color_[i].colorLocation = ogl.GetAttribLocation(color_[i].programId, "color");
        assert(color_[i].colorLocation >= 0);

        if (saturated) {
            color_[i].saturationLocation = ogl.GetUniformLocation(color_[i].programId, "saturation");
            assert(color_[i].saturationLocation >= 0);
        }

        color_[i].orthoDirty = true;

        return true;
    }

    bool Renderer::initDamageProgram(bool saturated)
    {
        int i = saturated ? 1 : 0;

        damage_[i].vsId = createShader((saturated ? damageSaturatedVsSource : damageVsSource), GL_VERTEX_SHADER);

        if (!damage_[i].vsId) {
            return false;
        }

        damage_[i].fsId = createShader(damageFsSource, GL_FRAGMENT_SHADER);

        if (!damage_[i].fsId) {
            return false;
        }

        damage_[i].programId = createProgram(damage_[i].vsId, damage_[i].fsId);

        if (!damage_[i].programId) {
            return false;
        }

        damage_[i].projLocation = ogl.GetUniformLocation(damage_[i].programId, "proj");
        assert(damage_[i].projLocation >= 0);
        damage_[i].posLocation = ogl.GetAttribLocation(damage_[i].programId, "pos");
        assert(damage_[i].posLocation >= 0);
        damage_[i].texCoordLocation = ogl.GetAttribLocation(damage_[i].programId, "texCoord");
        assert(damage_[i].texCoordLocation >= 0);
        damage_[i].colorLocation = ogl.GetAttribLocation(damage_[i].programId, "color");
        assert(damage_[i].colorLocation >= 0);

        if (saturated) {
            damage_[i].saturationLocation = ogl.GetUniformLocation(damage_[i].programId, "saturation");
            assert(damage_[i].saturationLocation >= 0);
        }

        damage_[i].orthoDirty = true;

        return true;
    }

    bool Renderer::initLightProgram(bool gammaCorrection, bool saturated)
    {
        int i, j = (saturated ? 1 : 0);
        std::string fsSource;

        if (gammaCorrection) {
            i = 1;
            fsSource = (boost::format(lightFsSource) % "sqrt").str();
        } else {
            i = 0;
            fsSource = (boost::format(lightFsSource) % "").str();
        }

        light_[i][j].vsId = createShader((saturated ? lightSaturatedVsSource : lightVsSource), GL_VERTEX_SHADER);

        if (!light_[i][j].vsId) {
            return false;
        }

        light_[i][j].fsId = createShader(fsSource, GL_FRAGMENT_SHADER);

        if (!light_[i][j].fsId) {
            return false;
        }

        light_[i][j].programId = createProgram(light_[i][j].vsId, light_[i][j].fsId);

        if (!light_[i][j].programId) {
            return false;
        }

        light_[i][j].projLocation = ogl.GetUniformLocation(light_[i][j].programId, "proj");
        assert(light_[i][j].projLocation >= 0);
        light_[i][j].posLocation = ogl.GetAttribLocation(light_[i][j].programId, "pos");
        assert(light_[i][j].posLocation >= 0);
        light_[i][j].colorLocation = ogl.GetAttribLocation(light_[i][j].programId, "color");
        assert(light_[i][j].colorLocation >= 0);
        light_[i][j].sLocation = ogl.GetAttribLocation(light_[i][j].programId, "s");
        assert(light_[i][j].sLocation >= 0);

        if (saturated) {
            light_[i][j].saturationLocation = ogl.GetUniformLocation(light_[i][j].programId, "saturation");
            assert(light_[i][j].saturationLocation >= 0);
        }

        light_[i][j].orthoDirty = true;

        return true;
    }

    bool Renderer::initShadowProgram()
    {
        shadow_.vsId = createShader(shadowVsSource, GL_VERTEX_SHADER);

        if (!shadow_.vsId) {
            return false;
        }

        shadow_.fsId = createShader(shadowFsSource, GL_FRAGMENT_SHADER);

        if (!shadow_.fsId) {
            return false;
        }

        shadow_.programId = createProgram(shadow_.vsId, shadow_.fsId);

        if (!shadow_.programId) {
            return false;
        }

        shadow_.ambientLocation = ogl.GetUniformLocation(shadow_.programId, "ambient");
        assert(shadow_.ambientLocation >= 0);
        shadow_.posLocation = ogl.GetAttribLocation(shadow_.programId, "pos");
        assert(shadow_.posLocation >= 0);
        shadow_.texCoordLocation = ogl.GetAttribLocation(shadow_.programId, "texCoord");
        assert(shadow_.texCoordLocation >= 0);

        return true;
    }

    bool Renderer::initDiffuseProgram()
    {
        diffuse_.vsId = createShader(diffuseVsSource, GL_VERTEX_SHADER);

        if (!diffuse_.vsId) {
            return false;
        }

        diffuse_.fsId = createShader(diffuseFsSource, GL_FRAGMENT_SHADER);

        if (!diffuse_.fsId) {
            return false;
        }

        diffuse_.programId = createProgram(diffuse_.vsId, diffuse_.fsId);

        if (!diffuse_.programId) {
            return false;
        }

        diffuse_.ambientLocation = ogl.GetUniformLocation(diffuse_.programId, "ambient");
        assert(diffuse_.ambientLocation >= 0);
        diffuse_.gammaLocation = ogl.GetUniformLocation(diffuse_.programId, "gamma");
        assert(diffuse_.gammaLocation >= 0);
        diffuse_.posLocation = ogl.GetAttribLocation(diffuse_.programId, "pos");
        assert(diffuse_.posLocation >= 0);
        diffuse_.texCoordLocation = ogl.GetAttribLocation(diffuse_.programId, "texCoord");
        assert(diffuse_.texCoordLocation >= 0);

        return true;
    }

    bool Renderer::initBlurProgram(UInt32 width, UInt32 height, bool diffuse)
    {
        int i;
        std::string fsSource;
        std::string vsSource = (boost::format(blurVsSource) % width % height).str();

        if (diffuse) {
            i = 1;
            fsSource = (boost::format(blurFsSource) % ".rgb").str();
        } else {
            i = 0;
            fsSource = (boost::format(blurFsSource) % "").str();
        }

        blur_[i].vsId = createShader(vsSource, GL_VERTEX_SHADER);

        if (!blur_[i].vsId) {
            return false;
        }

        blur_[i].fsId = createShader(fsSource, GL_FRAGMENT_SHADER);

        if (!blur_[i].fsId) {
            return false;
        }

        blur_[i].programId = createProgram(blur_[i].vsId, blur_[i].fsId);

        if (!blur_[i].programId) {
            return false;
        }

        blur_[i].dirLocation = ogl.GetUniformLocation(blur_[i].programId, "dir");
        assert(blur_[i].dirLocation >= 0);
        blur_[i].posLocation = ogl.GetAttribLocation(blur_[i].programId, "pos");
        assert(blur_[i].posLocation >= 0);
        blur_[i].texCoordLocation = ogl.GetAttribLocation(blur_[i].programId, "texCoord");
        assert(blur_[i].texCoordLocation >= 0);

        return true;
    }

    bool Renderer::initMaskProgram()
    {
        mask_.vsId = createShader(maskVsSource, GL_VERTEX_SHADER);

        if (!mask_.vsId) {
            return false;
        }

        mask_.fsId = createShader(maskFsSource, GL_FRAGMENT_SHADER);

        if (!mask_.fsId) {
            return false;
        }

        mask_.programId = createProgram(mask_.vsId, mask_.fsId);

        if (!mask_.programId) {
            return false;
        }

        mask_.projLocation = ogl.GetUniformLocation(mask_.programId, "proj");
        assert(mask_.projLocation >= 0);
        mask_.posLocation = ogl.GetAttribLocation(mask_.programId, "pos");
        assert(mask_.posLocation >= 0);
        mask_.texCoordLocation = ogl.GetAttribLocation(mask_.programId, "texCoord");
        assert(mask_.texCoordLocation >= 0);

        mask_.orthoDirty = true;

        return true;
    }

    bool Renderer::initProjTexProgram(bool saturated)
    {
        int i = saturated ? 1 : 0;

        projTex_[i].vsId = createShader(projTexVsSource, GL_VERTEX_SHADER);

        if (!projTex_[i].vsId) {
            return false;
        }

        projTex_[i].fsId = createShader((saturated ? projTexSaturatedFsSource : projTexFsSource), GL_FRAGMENT_SHADER);

        if (!projTex_[i].fsId) {
            return false;
        }

        projTex_[i].programId = createProgram(projTex_[i].vsId, projTex_[i].fsId);

        if (!projTex_[i].programId) {
            return false;
        }

        projTex_[i].projLocation = ogl.GetUniformLocation(projTex_[i].programId, "proj");
        assert(projTex_[i].projLocation >= 0);
        projTex_[i].posLocation = ogl.GetAttribLocation(projTex_[i].programId, "pos");
        assert(projTex_[i].posLocation >= 0);
        projTex_[i].texCoordLocation = ogl.GetAttribLocation(projTex_[i].programId, "texCoord");
        assert(projTex_[i].texCoordLocation >= 0);
        projTex_[i].colorLocation = ogl.GetAttribLocation(projTex_[i].programId, "color");
        assert(projTex_[i].colorLocation >= 0);

        if (saturated) {
            projTex_[i].saturationLocation = ogl.GetUniformLocation(projTex_[i].programId, "saturation");
            assert(projTex_[i].saturationLocation >= 0);
        }

        projTex_[i].orthoDirty = true;

        return true;
    }

    bool Renderer::initFlashProgram(bool saturated)
    {
        int i = saturated ? 1 : 0;

        flash_[i].vsId = createShader(flashVsSource, GL_VERTEX_SHADER);

        if (!flash_[i].vsId) {
            return false;
        }

        flash_[i].fsId = createShader((saturated ? flashSaturatedFsSource : flashFsSource), GL_FRAGMENT_SHADER);

        if (!flash_[i].fsId) {
            return false;
        }

        flash_[i].programId = createProgram(flash_[i].vsId, flash_[i].fsId);

        if (!flash_[i].programId) {
            return false;
        }

        flash_[i].projLocation = ogl.GetUniformLocation(flash_[i].programId, "proj");
        assert(flash_[i].projLocation >= 0);
        flash_[i].posLocation = ogl.GetAttribLocation(flash_[i].programId, "pos");
        assert(flash_[i].posLocation >= 0);
        flash_[i].texCoordLocation = ogl.GetAttribLocation(flash_[i].programId, "texCoord");
        assert(flash_[i].texCoordLocation >= 0);
        flash_[i].colorLocation = ogl.GetAttribLocation(flash_[i].programId, "color");
        assert(flash_[i].colorLocation >= 0);
        flash_[i].flashColorLocation = ogl.GetAttribLocation(flash_[i].programId, "flashColor");
        assert(flash_[i].flashColorLocation >= 0);

        if (saturated) {
            flash_[i].saturationLocation = ogl.GetUniformLocation(flash_[i].programId, "saturation");
            assert(flash_[i].saturationLocation >= 0);
        }

        flash_[i].orthoDirty = true;

        return true;
    }

    bool Renderer::initLightmap(UInt32 width, UInt32 height)
    {
        lightmap_.width = width;
        lightmap_.height = height;

        ogl.GenTextures(2, &lightmap_.texId[0]);
        for (int i = 0; i < 2; ++i) {
            ogl.BindTexture(GL_TEXTURE_2D, lightmap_.texId[i]);
            ogl.TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                GL_UNSIGNED_BYTE, NULL);
            ogl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            ogl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            ogl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            ogl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        ogl.GenTextures(1, &lightmap_.blurTexId);
        ogl.BindTexture(GL_TEXTURE_2D, lightmap_.blurTexId);
        ogl.TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
            GL_UNSIGNED_BYTE, NULL);
        ogl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        ogl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        ogl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        ogl.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        ogl.BindTexture(GL_TEXTURE_2D, 0);

        GLuint curFb = 0;

        ogl.GetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&curFb);

        ogl.GenFramebuffers(2, &lightmap_.fbId[0]);
        for (int i = 0; i < 2; ++i) {
            ogl.BindFramebuffer(GL_FRAMEBUFFER, lightmap_.fbId[i]);
            ogl.FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_2D, lightmap_.texId[i], 0);
            GLenum status = ogl.CheckFramebufferStatus(GL_FRAMEBUFFER);

            if (status != GL_FRAMEBUFFER_COMPLETE) {
                LOG4CPLUS_ERROR(logger(), "Unable to create lightmap framebuffer, status = " << status);
                ogl.BindFramebuffer(GL_FRAMEBUFFER, curFb);
                return false;
            }
        }

        ogl.GenFramebuffers(1, &lightmap_.blurFbId);
        ogl.BindFramebuffer(GL_FRAMEBUFFER, lightmap_.blurFbId);
        ogl.FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D, lightmap_.blurTexId, 0);
        GLenum status = ogl.CheckFramebufferStatus(GL_FRAMEBUFFER);
        ogl.BindFramebuffer(GL_FRAMEBUFFER, curFb);

        if (status != GL_FRAMEBUFFER_COMPLETE) {
            LOG4CPLUS_ERROR(logger(), "Unable to create lightmap blur framebuffer, status = " << status);
            return false;
        }

        return true;
    }

    void Renderer::flush()
    {
        if (!batch_->empty()) {
            GLenum sfactor = batch_->blendSfactor;
            GLenum dfactor = batch_->blendDfactor;
            float saturation = batch_->saturation;

            queue_->addBatch(batch_);
            batch_ = renderBatchPool.allocate();

            batch_->blendSfactor = sfactor;
            batch_->blendDfactor = dfactor;
            batch_->saturation = saturation;
        }
    }

    void Renderer::processDefBatch(const RenderBatch* batch)
    {
        int i = (batch->saturation != 1.0f) ? 1 : 0;

        if (lastProgram_ != def_[i].programId) {
            (this->*unuseLastProgram_)(unuseLastProgramArg_);
            unuseLastProgram_ = &Renderer::unuseDefProgram;
            unuseLastProgramArg_ = i;
            ogl.UseProgram(def_[i].programId);
            lastProgram_ = def_[i].programId;
            ogl.EnableVertexAttribArray(def_[i].posLocation);
            ogl.EnableVertexAttribArray(def_[i].texCoordLocation);
            ogl.EnableVertexAttribArray(def_[i].colorLocation);
            if (i == 1) {
                ogl.EnableVertexAttribArray(def_[i].saturationLocation);
            }
        }

        ogl.VertexAttribPointer(def_[i].posLocation, 2, GL_FLOAT, GL_FALSE, 0, &batch->vertices[0]);
        ogl.VertexAttribPointer(def_[i].texCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, &batch->texCoords[0]);
        ogl.VertexAttribPointer(def_[i].colorLocation, 4, GL_FLOAT, GL_FALSE, 0, &batch->colors[0]);
        if (i == 1) {
            ogl.Uniform1f(def_[i].saturationLocation, batch->saturation);
        }

        if (def_[i].orthoDirty) {
            ogl.UniformMatrix4fv(def_[i].projLocation, 1, GL_FALSE, &ortho_[0]);
            def_[i].orthoDirty = false;
        }

        if (lastTexture_ != batch->texture->id()) {
            ogl.BindTexture(GL_TEXTURE_2D, batch->texture->id());
            lastTexture_ = batch->texture->id();
        }

        ogl.DrawArrays(GL_TRIANGLES, 0, batch->vertices.size() / 2);
    }

    void Renderer::processColorBatch(const RenderBatch* batch)
    {
        int i = (batch->saturation != 1.0f) ? 1 : 0;

        if (lastProgram_ != color_[i].programId) {
            (this->*unuseLastProgram_)(unuseLastProgramArg_);
            unuseLastProgram_ = &Renderer::unuseColorProgram;
            unuseLastProgramArg_ = i;
            ogl.UseProgram(color_[i].programId);
            lastProgram_ = color_[i].programId;
            ogl.EnableVertexAttribArray(color_[i].posLocation);
            ogl.EnableVertexAttribArray(color_[i].colorLocation);
            if (i == 1) {
                ogl.EnableVertexAttribArray(color_[i].saturationLocation);
            }
        }

        ogl.VertexAttribPointer(color_[i].posLocation, 2, GL_FLOAT, GL_FALSE, 0, &batch->vertices[0]);
        ogl.VertexAttribPointer(color_[i].colorLocation, 4, GL_FLOAT, GL_FALSE, 0, &batch->colors[0]);
        if (i == 1) {
            ogl.Uniform1f(color_[i].saturationLocation, batch->saturation);
        }

        if (color_[i].orthoDirty) {
            ogl.UniformMatrix4fv(color_[i].projLocation, 1, GL_FALSE, &ortho_[0]);
            color_[i].orthoDirty = false;
        }

        switch (batch->mode) {
        case GL_POINTS:
            if (ogl.PointSize) {
                /*
                 * We need this because GLESv2 doesn't support glPointSize.
                 * TODO: reimplement in shader using gl_PointSize.
                 */
                ogl.PointSize(batch->pointSize);
            }
            ogl.DrawArrays(GL_POINTS, 0, batch->vertices.size() / 2);
            break;
        case GL_LINES:
            ogl.LineWidth(batch->pointSize);
            ogl.DrawArrays(GL_LINES, 0, batch->vertices.size() / 2);
            break;
        default:
            ogl.DrawArrays(GL_TRIANGLES, 0, batch->vertices.size() / 2);
            break;
        }
    }

    void Renderer::processDamageBatch(const RenderBatch* batch)
    {
        int i = (batch->saturation != 1.0f) ? 1 : 0;

        if (lastProgram_ != damage_[i].programId) {
            (this->*unuseLastProgram_)(unuseLastProgramArg_);
            unuseLastProgram_ = &Renderer::unuseDamageProgram;
            unuseLastProgramArg_ = i;
            ogl.UseProgram(damage_[i].programId);
            lastProgram_ = damage_[i].programId;
            ogl.EnableVertexAttribArray(damage_[i].posLocation);
            ogl.EnableVertexAttribArray(damage_[i].texCoordLocation);
            ogl.EnableVertexAttribArray(damage_[i].colorLocation);
            if (i == 1) {
                ogl.EnableVertexAttribArray(damage_[i].saturationLocation);
            }
        }

        ogl.VertexAttribPointer(damage_[i].posLocation, 2, GL_FLOAT, GL_FALSE, 0, &batch->vertices[0]);
        ogl.VertexAttribPointer(damage_[i].texCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, &batch->texCoords[0]);
        ogl.VertexAttribPointer(damage_[i].colorLocation, 4, GL_FLOAT, GL_FALSE, 0, &batch->colors[0]);
        if (i == 1) {
            ogl.Uniform1f(damage_[i].saturationLocation, batch->saturation);
        }

        if (damage_[i].orthoDirty) {
            ogl.UniformMatrix4fv(damage_[i].projLocation, 1, GL_FALSE, &ortho_[0]);
            damage_[i].orthoDirty = false;
        }

        ogl.DrawArrays(GL_TRIANGLES, 0, batch->vertices.size() / 2);
    }

    void Renderer::processLightBatch(const RenderBatch* batch)
    {
        int i = (batch->programId == RenderBatch::ProgramIdLightGamma) ? 1 : 0;
        int j = batch->diffuse ? 1 : 0;
        int k = (batch->saturation != 1.0f) ? 1 : 0;

        if (lastProgram_ != light_[i][k].programId) {
            (this->*unuseLastProgram_)(unuseLastProgramArg_);
            unuseLastProgram_ = &Renderer::unuseLightProgram;
            unuseLastProgramArg_ = i | (k << 1);
            ogl.UseProgram(light_[i][k].programId);
            lastProgram_ = light_[i][k].programId;
            ogl.EnableVertexAttribArray(light_[i][k].posLocation);
            ogl.EnableVertexAttribArray(light_[i][k].colorLocation);
            ogl.EnableVertexAttribArray(light_[i][k].sLocation);
            if (k == 1) {
                ogl.EnableVertexAttribArray(light_[i][k].saturationLocation);
            }
        }

        GLuint curFb = 0;

        ogl.GetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&curFb);
        ogl.BindFramebuffer(GL_FRAMEBUFFER, lightmap_.fbId[j]);
        ogl.Viewport(0, 0, lightmap_.width, lightmap_.height);
        ogl.ClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        ogl.Clear(GL_COLOR_BUFFER_BIT);

        if (!batch->vertices.empty()) {
            ogl.VertexAttribPointer(light_[i][k].posLocation, 2, GL_FLOAT, GL_FALSE, 0, &batch->vertices[0]);
            ogl.VertexAttribPointer(light_[i][k].colorLocation, 4, GL_FLOAT, GL_FALSE, 0, &batch->colors[0]);
            ogl.VertexAttribPointer(light_[i][k].sLocation, 1, GL_FLOAT, GL_FALSE, 0, &batch->generic1[0]);
            if (k == 1) {
                ogl.Uniform1f(light_[i][k].saturationLocation, batch->saturation);
            }

            if (light_[i][k].orthoDirty) {
                ogl.UniformMatrix4fv(light_[i][k].projLocation, 1, GL_FALSE, &ortho_[0]);
                light_[i][k].orthoDirty = false;
            }

            ogl.DrawArrays(GL_TRIANGLES, 0, batch->vertices.size() / 2);
        }

        ogl.BindFramebuffer(GL_FRAMEBUFFER, curFb);
        ogl.Viewport(settings.viewX, settings.viewY, settings.viewWidth, settings.viewHeight);
        ogl.ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }

    void Renderer::processShadowBatch(const RenderBatch* batch)
    {
        if (lastProgram_ != shadow_.programId) {
            (this->*unuseLastProgram_)(unuseLastProgramArg_);
            unuseLastProgram_ = &Renderer::unuseShadowProgram;
            ogl.UseProgram(shadow_.programId);
            lastProgram_ = shadow_.programId;
            ogl.EnableVertexAttribArray(shadow_.posLocation);
            ogl.EnableVertexAttribArray(shadow_.texCoordLocation);
        }

        ogl.VertexAttribPointer(shadow_.posLocation, 2, GL_FLOAT, GL_FALSE, 0, &batch->vertices[0]);
        ogl.VertexAttribPointer(shadow_.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, &batch->texCoords[0]);

        ogl.Uniform4fv(shadow_.ambientLocation, 1, &batch->ambient[0]);

        if (lastTexture_ != lightmap_.texId[0]) {
            ogl.BindTexture(GL_TEXTURE_2D, lightmap_.texId[0]);
            lastTexture_ = lightmap_.texId[0];
        }

        ogl.DrawArrays(GL_TRIANGLES, 0, batch->vertices.size() / 2);
    }

    void Renderer::processDiffuseBatch(const RenderBatch* batch)
    {
        if (lastProgram_ != diffuse_.programId) {
            (this->*unuseLastProgram_)(unuseLastProgramArg_);
            unuseLastProgram_ = &Renderer::unuseDiffuseProgram;
            ogl.UseProgram(diffuse_.programId);
            lastProgram_ = diffuse_.programId;
            ogl.EnableVertexAttribArray(diffuse_.posLocation);
            ogl.EnableVertexAttribArray(diffuse_.texCoordLocation);
        }

        ogl.VertexAttribPointer(diffuse_.posLocation, 2, GL_FLOAT, GL_FALSE, 0, &batch->vertices[0]);
        ogl.VertexAttribPointer(diffuse_.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, &batch->texCoords[0]);

        ogl.Uniform4fv(diffuse_.ambientLocation, 1, &batch->ambient[0]);
        ogl.Uniform1f(diffuse_.gammaLocation, settings.gamma);

        if (lastTexture_ != lightmap_.texId[1]) {
            ogl.BindTexture(GL_TEXTURE_2D, lightmap_.texId[1]);
            lastTexture_ = lightmap_.texId[1];
        }

        ogl.DrawArrays(GL_TRIANGLES, 0, batch->vertices.size() / 2);
    }

    void Renderer::processBlurBatch(const RenderBatch* batch)
    {
        ogl.Disable(GL_BLEND);

        GLuint curFb = 0;

        ogl.GetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&curFb);

        for (int i = 0; i < 2; ++i) {
            if (lastProgram_ != blur_[i].programId) {
                (this->*unuseLastProgram_)(unuseLastProgramArg_);
                unuseLastProgram_ = &Renderer::unuseBlurProgram;
                unuseLastProgramArg_ = i;
                ogl.UseProgram(blur_[i].programId);
                lastProgram_ = blur_[i].programId;
                ogl.EnableVertexAttribArray(blur_[i].posLocation);
                ogl.EnableVertexAttribArray(blur_[i].texCoordLocation);
            }

            ogl.VertexAttribPointer(blur_[i].posLocation, 2, GL_FLOAT, GL_FALSE, 0, &batch->vertices[0]);
            ogl.VertexAttribPointer(blur_[i].texCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, &batch->texCoords[0]);

            for (UInt32 j = 0; j < batch->numBlur; ++j) {
                ogl.BindFramebuffer(GL_FRAMEBUFFER, lightmap_.blurFbId);
                ogl.Viewport(0, 0, lightmap_.width, lightmap_.height);

                if (lastTexture_ != lightmap_.texId[i]) {
                    ogl.BindTexture(GL_TEXTURE_2D, lightmap_.texId[i]);
                    lastTexture_ = lightmap_.texId[i];
                }

                ogl.Uniform2f(blur_[i].dirLocation, 1.0f, 0.0f);

                ogl.DrawArrays(GL_TRIANGLES, 0, batch->vertices.size() / 2);

                ogl.BindFramebuffer(GL_FRAMEBUFFER, lightmap_.fbId[i]);
                ogl.Viewport(0, 0, lightmap_.width, lightmap_.height);

                if (lastTexture_ != lightmap_.blurTexId) {
                    ogl.BindTexture(GL_TEXTURE_2D, lightmap_.blurTexId);
                    lastTexture_ = lightmap_.blurTexId;
                }

                ogl.Uniform2f(blur_[i].dirLocation, 0.0f, 1.0f);

                ogl.DrawArrays(GL_TRIANGLES, 0, batch->vertices.size() / 2);
            }
        }

        ogl.BindFramebuffer(GL_FRAMEBUFFER, curFb);
        ogl.Viewport(settings.viewX, settings.viewY, settings.viewWidth, settings.viewHeight);

        ogl.Enable(GL_BLEND);
    }

    void Renderer::processMaskBatch(const RenderBatch* batch)
    {
        if (lastProgram_ != mask_.programId) {
            (this->*unuseLastProgram_)(unuseLastProgramArg_);
            unuseLastProgram_ = &Renderer::unuseMaskProgram;
            ogl.UseProgram(mask_.programId);
            lastProgram_ = mask_.programId;
            ogl.EnableVertexAttribArray(mask_.posLocation);
            ogl.EnableVertexAttribArray(mask_.texCoordLocation);
        }

        ogl.VertexAttribPointer(mask_.posLocation, 2, GL_FLOAT, GL_FALSE, 0, &batch->vertices[0]);
        ogl.VertexAttribPointer(mask_.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, &batch->texCoords[0]);

        if (mask_.orthoDirty) {
            ogl.UniformMatrix4fv(mask_.projLocation, 1, GL_FALSE, &ortho_[0]);
            mask_.orthoDirty = false;
        }

        if (lastTexture_ != batch->texture->id()) {
            ogl.BindTexture(GL_TEXTURE_2D, batch->texture->id());
            lastTexture_ = batch->texture->id();
        }

        ogl.DrawArrays(GL_TRIANGLES, 0, batch->vertices.size() / 2);
    }

    void Renderer::processProjTexBatch(const RenderBatch* batch)
    {
        int i = (batch->saturation != 1.0f) ? 1 : 0;

        if (lastProgram_ != projTex_[i].programId) {
            (this->*unuseLastProgram_)(unuseLastProgramArg_);
            unuseLastProgram_ = &Renderer::unuseProjTexProgram;
            unuseLastProgramArg_ = i;
            ogl.UseProgram(projTex_[i].programId);
            lastProgram_ = projTex_[i].programId;
            ogl.EnableVertexAttribArray(projTex_[i].posLocation);
            ogl.EnableVertexAttribArray(projTex_[i].texCoordLocation);
            ogl.EnableVertexAttribArray(projTex_[i].colorLocation);
            if (i == 1) {
                ogl.EnableVertexAttribArray(projTex_[i].saturationLocation);
            }
        }

        ogl.VertexAttribPointer(projTex_[i].posLocation, 2, GL_FLOAT, GL_FALSE, 0, &batch->vertices[0]);
        ogl.VertexAttribPointer(projTex_[i].texCoordLocation, 4, GL_FLOAT, GL_FALSE, 0, &batch->texCoords[0]);
        ogl.VertexAttribPointer(projTex_[i].colorLocation, 4, GL_FLOAT, GL_FALSE, 0, &batch->colors[0]);
        if (i == 1) {
            ogl.Uniform1f(projTex_[i].saturationLocation, batch->saturation);
        }

        if (projTex_[i].orthoDirty) {
            ogl.UniformMatrix4fv(projTex_[i].projLocation, 1, GL_FALSE, &ortho_[0]);
            projTex_[i].orthoDirty = false;
        }

        if (lastTexture_ != batch->texture->id()) {
            ogl.BindTexture(GL_TEXTURE_2D, batch->texture->id());
            lastTexture_ = batch->texture->id();
        }

        ogl.DrawArrays(GL_TRIANGLES, 0, batch->vertices.size() / 2);
    }

    void Renderer::processFlashBatch(const RenderBatch* batch)
    {
        int i = (batch->saturation != 1.0f) ? 1 : 0;

        if (lastProgram_ != flash_[i].programId) {
            (this->*unuseLastProgram_)(unuseLastProgramArg_);
            unuseLastProgram_ = &Renderer::unuseFlashProgram;
            unuseLastProgramArg_ = i;
            ogl.UseProgram(flash_[i].programId);
            lastProgram_ = flash_[i].programId;
            ogl.EnableVertexAttribArray(flash_[i].posLocation);
            ogl.EnableVertexAttribArray(flash_[i].texCoordLocation);
            ogl.EnableVertexAttribArray(flash_[i].colorLocation);
            ogl.EnableVertexAttribArray(flash_[i].flashColorLocation);
            if (i == 1) {
                ogl.EnableVertexAttribArray(flash_[i].saturationLocation);
            }
        }

        ogl.VertexAttribPointer(flash_[i].posLocation, 2, GL_FLOAT, GL_FALSE, 0, &batch->vertices[0]);
        ogl.VertexAttribPointer(flash_[i].texCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, &batch->texCoords[0]);
        ogl.VertexAttribPointer(flash_[i].colorLocation, 4, GL_FLOAT, GL_FALSE, 0, &batch->colors[0]);
        ogl.VertexAttribPointer(flash_[i].flashColorLocation, 4, GL_FLOAT, GL_FALSE, 0, &batch->generic1[0]);
        if (i == 1) {
            ogl.Uniform1f(flash_[i].saturationLocation, batch->saturation);
        }

        if (flash_[i].orthoDirty) {
            ogl.UniformMatrix4fv(flash_[i].projLocation, 1, GL_FALSE, &ortho_[0]);
            flash_[i].orthoDirty = false;
        }

        if (lastTexture_ != batch->texture->id()) {
            ogl.BindTexture(GL_TEXTURE_2D, batch->texture->id());
            lastTexture_ = batch->texture->id();
        }

        ogl.DrawArrays(GL_TRIANGLES, 0, batch->vertices.size() / 2);
    }

    void Renderer::unuseNoneProgram(int arg)
    {
    }

    void Renderer::unuseDefProgram(int arg)
    {
        ogl.DisableVertexAttribArray(def_[arg].posLocation);
        ogl.DisableVertexAttribArray(def_[arg].texCoordLocation);
        ogl.DisableVertexAttribArray(def_[arg].colorLocation);
        if (arg == 1) {
            ogl.DisableVertexAttribArray(def_[arg].saturationLocation);
        }
    }

    void Renderer::unuseColorProgram(int arg)
    {
        ogl.DisableVertexAttribArray(color_[arg].posLocation);
        ogl.DisableVertexAttribArray(color_[arg].colorLocation);
        if (arg == 1) {
            ogl.DisableVertexAttribArray(color_[arg].saturationLocation);
        }
    }

    void Renderer::unuseDamageProgram(int arg)
    {
        ogl.DisableVertexAttribArray(damage_[arg].posLocation);
        ogl.DisableVertexAttribArray(damage_[arg].texCoordLocation);
        ogl.DisableVertexAttribArray(damage_[arg].colorLocation);
        if (arg == 1) {
            ogl.DisableVertexAttribArray(damage_[arg].saturationLocation);
        }
    }

    void Renderer::unuseLightProgram(int arg)
    {
        int i = arg & 1;
        int j = (arg >> 1);
        ogl.DisableVertexAttribArray(light_[i][j].posLocation);
        ogl.DisableVertexAttribArray(light_[i][j].colorLocation);
        ogl.DisableVertexAttribArray(light_[i][j].sLocation);
        if (j == 1) {
            ogl.DisableVertexAttribArray(light_[i][j].saturationLocation);
        }
    }

    void Renderer::unuseShadowProgram(int arg)
    {
        ogl.DisableVertexAttribArray(shadow_.posLocation);
        ogl.DisableVertexAttribArray(shadow_.texCoordLocation);
    }

    void Renderer::unuseDiffuseProgram(int arg)
    {
        ogl.DisableVertexAttribArray(diffuse_.posLocation);
        ogl.DisableVertexAttribArray(diffuse_.texCoordLocation);
    }

    void Renderer::unuseBlurProgram(int arg)
    {
        ogl.DisableVertexAttribArray(blur_[arg].posLocation);
        ogl.DisableVertexAttribArray(blur_[arg].texCoordLocation);
    }

    void Renderer::unuseMaskProgram(int arg)
    {
        ogl.DisableVertexAttribArray(mask_.posLocation);
        ogl.DisableVertexAttribArray(mask_.texCoordLocation);
    }

    void Renderer::unuseProjTexProgram(int arg)
    {
        ogl.DisableVertexAttribArray(projTex_[arg].posLocation);
        ogl.DisableVertexAttribArray(projTex_[arg].texCoordLocation);
        ogl.DisableVertexAttribArray(projTex_[arg].colorLocation);
        if (arg == 1) {
            ogl.DisableVertexAttribArray(projTex_[arg].saturationLocation);
        }
    }

    void Renderer::unuseFlashProgram(int arg)
    {
        ogl.DisableVertexAttribArray(flash_[arg].posLocation);
        ogl.DisableVertexAttribArray(flash_[arg].texCoordLocation);
        ogl.DisableVertexAttribArray(flash_[arg].colorLocation);
        ogl.DisableVertexAttribArray(flash_[arg].flashColorLocation);
        if (arg == 1) {
            ogl.DisableVertexAttribArray(flash_[arg].saturationLocation);
        }
    }
}
