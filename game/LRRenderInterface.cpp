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

#include "LRRenderInterface.h"
#include "TextureManager.h"
#include "AssetManager.h"
#include "Settings.h"
#include "GameShell.h"
#include <Rocket/Core/Log.h>
#include <boost/tokenizer.hpp>

namespace af
{
    LRRenderInterface::LRRenderInterface()
    : scissor_(false),
      scissorX_(0),
      scissorY_(0),
      scissorW_(settings.viewWidth),
      scissorH_(settings.viewHeight)
    {
    }

    LRRenderInterface::~LRRenderInterface()
    {
    }

    void LRRenderInterface::RenderGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f& translation)
    {
        Image* image = NULL;

        if (scissor_) {
            renderer.checkScissor(scissorX_, scissorY_, scissorW_, scissorH_);
        } else {
            renderer.endScissor();
        }

        if (texture) {
            image = reinterpret_cast<Image*>(texture);

            renderer.setProgramDef(image->texture());
        } else {
            renderer.setProgramColor();
        }

        if (scissor_) {
            renderer.setScissor(scissorX_, scissorY_, scissorW_, scissorH_);
        }

        RenderSimple rop = renderer.renderTriangles();

        float sx = (settings.gameHeight * settings.viewWidth) / (settings.viewHeight * settings.layoutWidth);
        float sy = settings.gameHeight / settings.layoutHeight;

        for (int i = 0; i < num_indices; ++i) {
            int idx = indices[i];

            Rocket::Core::Vector2f tmp = vertices[idx].position + translation;

            rop.addVertex(tmp.x * sx, settings.gameHeight - tmp.y * sy);

            rop.addColor(Color(vertices[idx].colour.red / 255.0f,
                vertices[idx].colour.green / 255.0f,
                vertices[idx].colour.blue / 255.0f,
                vertices[idx].colour.alpha / 255.0f));
        }

        if (image) {
            for (int i = 0; i < num_indices; ++i) {
                Rocket::Core::Vector2f tmp = vertices[indices[i]].tex_coord;
                rop.addTexCoord((tmp.x * image->width() + image->x()) / image->texture()->width(),
                    1.0f - (tmp.y * image->height() + image->y()) / image->texture()->height());
            }
        }
    }

    Rocket::Core::CompiledGeometryHandle LRRenderInterface::CompileGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rocket::Core::TextureHandle texture)
    {
        CompiledGeometry* geom = new CompiledGeometry();

        if (texture) {
            geom->image = *reinterpret_cast<Image*>(texture);
        }

        geom->translation = Rocket::Core::Vector2f(0.0f, 0.0f);

        float sx = (settings.gameHeight * settings.viewWidth) / (settings.viewHeight * settings.layoutWidth);
        float sy = settings.gameHeight / settings.layoutHeight;

        for (int i = 0; i < num_indices; ++i) {
            int idx = indices[i];

            Rocket::Core::Vector2f tmp = vertices[idx].position;

            geom->origVertices.push_back(tmp.x);
            geom->origVertices.push_back(tmp.y);

            geom->vertices.push_back(tmp.x * sx);
            geom->vertices.push_back(settings.gameHeight - tmp.y * sy);

            geom->colors.push_back(vertices[idx].colour.red / 255.0f);
            geom->colors.push_back(vertices[idx].colour.green / 255.0f);
            geom->colors.push_back(vertices[idx].colour.blue / 255.0f);
            geom->colors.push_back(vertices[idx].colour.alpha / 255.0f);
        }

        if (texture) {
            for (int i = 0; i < num_indices; ++i) {
                Rocket::Core::Vector2f tmp = vertices[indices[i]].tex_coord;

                geom->texCoords.push_back((tmp.x * geom->image.width() + geom->image.x()) / geom->image.texture()->width());
                geom->texCoords.push_back(1.0f - (tmp.y * geom->image.height() + geom->image.y()) / geom->image.texture()->height());
            }
        }

        return reinterpret_cast<Rocket::Core::CompiledGeometryHandle>(geom);
    }

    void LRRenderInterface::RenderCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry, const Rocket::Core::Vector2f& translation)
    {
        CompiledGeometry* geom = reinterpret_cast<CompiledGeometry*>(geometry);

        if (geom->translation != translation) {
            geom->translation = translation;

            float sx = (settings.gameHeight * settings.viewWidth) / (settings.viewHeight * settings.layoutWidth);
            float sy = settings.gameHeight / settings.layoutHeight;

            for (size_t i = 0; i < geom->vertices.size(); i += 2) {
                geom->vertices[i] = (geom->origVertices[i] + translation.x) * sx;
                geom->vertices[i + 1] = settings.gameHeight - (geom->origVertices[i + 1] + translation.y) * sy;
            }
        }

        if (scissor_) {
            renderer.checkScissor(scissorX_, scissorY_, scissorW_, scissorH_);
        } else {
            renderer.endScissor();
        }

        if (geom->image) {
            renderer.setProgramDef(geom->image.texture());
        } else {
            renderer.setProgramColor();
        }

        if (scissor_) {
            renderer.setScissor(scissorX_, scissorY_, scissorW_, scissorH_);
        }

        RenderSimple rop = renderer.renderTriangles();

        rop.addVertices(&geom->vertices[0], geom->vertices.size() / 2);
        rop.addColors(&geom->colors[0], geom->colors.size() / 4);

        if (geom->image) {
            rop.addTexCoords(&geom->texCoords[0], geom->texCoords.size() / 2);
        }
    }

    void LRRenderInterface::ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry)
    {
        CompiledGeometry* geom = reinterpret_cast<CompiledGeometry*>(geometry);

        delete geom;
    }

    void LRRenderInterface::EnableScissorRegion(bool enable)
    {
        scissor_ = enable;
    }

    void LRRenderInterface::SetScissorRegion(int x, int y, int width, int height)
    {
        scissorX_ = (x * static_cast<int>(settings.viewWidth)) / static_cast<int>(settings.layoutWidth);
        scissorY_ = ((static_cast<int>(settings.layoutHeight) - y - height) * static_cast<int>(settings.viewHeight)) / static_cast<int>(settings.layoutHeight);
        scissorW_ = (width * settings.viewWidth) / settings.layoutWidth;
        scissorH_ = (height * settings.viewHeight) / settings.layoutHeight;

        scissorX_ += settings.viewX;
        scissorY_ += settings.viewY;
    }

    bool LRRenderInterface::LoadTexture(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source)
    {
        const char sepStr[] = { '/', '\\', '\0' };

        boost::char_separator<char> seps(sepStr);

        std::string tmp = source.CString();

        if (tmp.compare(0, 8, "ach_icon") == 0) {
            std::string tmp2 = tmp.substr(8);

            std::istringstream is(tmp2);

            int ach = 0;

            if (!(is >> ach) || !is.eof() || (ach < 0) || (ach > AchievementMax)) {
                Rocket::Core::Log::Message(Rocket::Core::Log::LT_ERROR, "Cannot convert %s to achievement index", tmp.c_str());
            } else {
                Image* res = new Image(gameShell->getAchievementImage(static_cast<Achievement>(ach)));

                texture_dimensions.x = res->width();
                texture_dimensions.y = res->height();

                texture_handle = reinterpret_cast<Rocket::Core::TextureHandle>(res);

                return true;
            }
        }

        boost::tokenizer<boost::char_separator<char> > tokens(tmp, seps);

        std::vector<std::string> p;

        for (boost::tokenizer<boost::char_separator<char> >::iterator it =
             tokens.begin(); it != tokens.end(); ++it)
        {
            std::string part = *it;
            if (part == "..") {
                if (!p.empty()) {
                    p.pop_back();
                }
            } else {
                p.push_back(part);
            }
        }

        std::string::size_type pos = p.back().find_first_of('@');

        UInt32 dimx = 0, dimy = 0;

        if (pos != std::string::npos) {
            std::string::size_type pos2 = p.back().find_first_of('x', pos + 1);
            if (pos2 != std::string::npos) {
                std::string tmp = p.back().substr(pos + 1, pos2 - pos - 1);

                std::istringstream is(tmp);

                if (!(is >> dimx) || !is.eof()) {
                    Rocket::Core::Log::Message(Rocket::Core::Log::LT_ERROR, "Cannot convert %s to integer", tmp.c_str());
                    dimx = 0;
                }

                tmp = p.back().substr(pos2 + 1);

                is.str(tmp);
                is.clear();

                if (!(is >> dimy) || !is.eof()) {
                    Rocket::Core::Log::Message(Rocket::Core::Log::LT_ERROR, "Cannot convert %s to integer", tmp.c_str());
                    dimy = 0;
                }
            } else {
                std::string tmp = p.back().substr(pos + 1);

                std::istringstream is(tmp);

                if (!(is >> dimy) || !is.eof()) {
                    Rocket::Core::Log::Message(Rocket::Core::Log::LT_ERROR, "Cannot convert %s to integer", tmp.c_str());
                    dimy = 0;
                }
            }
            p.back() = p.back().substr(0, pos);
        }

        tmp.clear();

        for (std::vector<std::string>::const_iterator it =
              p.begin(); it != p.end(); ++it)
        {
            if (!tmp.empty()) {
                tmp += "/";
            }
            tmp += *it;
        }

        Image image = assetManager.getImage(tmp, Texture::WrapModeClamp, Texture::WrapModeClamp);

        Image* res = new Image(image);

        if ((dimy != 0) && (dimx == 0)) {
            dimx = image.aspect() * dimy;
        }

        if (dimx == 0) {
            texture_dimensions.x = image.width();
            texture_dimensions.y = image.height();
        } else {
            texture_dimensions.x = dimx;
            texture_dimensions.y = dimy;
        }

        texture_handle = reinterpret_cast<Rocket::Core::TextureHandle>(res);

        return true;
    }

    bool LRRenderInterface::GenerateTexture(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions)
    {
        TexturePtr tex = textureManager.genTexture(source, source_dimensions.x,
            source_dimensions.y, Texture::WrapModeClamp, Texture::WrapModeClamp);

        Image* res = new Image(tex, 0, 0, tex->width(), tex->height());

        texture_handle = reinterpret_cast<Rocket::Core::TextureHandle>(res);

        return true;
    }

    void LRRenderInterface::ReleaseTexture(Rocket::Core::TextureHandle texture)
    {
        Image* image = reinterpret_cast<Image*>(texture);
        delete image;
    }
}
