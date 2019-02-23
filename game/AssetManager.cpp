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

#include "AssetManager.h"
#include "TextureManager.h"
#include "Logger.h"
#include "Platform.h"
#include "Settings.h"
#include "af/Utils.h"
#include <boost/make_shared.hpp>
#include "ft2build.h"
#include FT_FREETYPE_H

namespace af
{
    AssetManager assetManager;

    template <>
    Single<AssetManager>* Single<AssetManager>::single = NULL;

    static std::set<std::string> assetDump;

    AssetManager::AssetManager()
    {
    }

    AssetManager::~AssetManager()
    {
    }

    bool AssetManager::init()
    {
        return true;
    }

    void AssetManager::shutdown()
    {
        if (settings.dumpAssetList) {
            for (std::set<std::string>::const_iterator it = assetDump.begin();
                it != assetDump.end(); ++it) {
                std::cerr << *it << "\n";
            }
        }

        tpsMap_.clear();
        worldMap_.clear();
        gdxpMap_.clear();
        charMap_.clear();
        animationMap_.clear();
    }

    void AssetManager::reload()
    {
        charMap_.clear();
        charMap();
    }

    Image AssetManager::getBadImage()
    {
        return Image(textureManager.loadTexture("bad.png",
                                                Texture::WrapModeRepeat,
                                                Texture::WrapModeRepeat),
                     0, 0, 256, 256);
    }

    Image AssetManager::getImage(const std::string& name,
                                 Texture::WrapMode wrapX,
                                 Texture::WrapMode wrapY)
    {
        assetTouched(name);

        std::string::size_type pos = name.find('/');

        if (pos == std::string::npos) {
            TexturePtr texture = textureManager.loadTexture(name,
                                                            wrapX, wrapY);

            if (!texture) {
                return getBadImage();
            }

            return Image(texture, 0, 0, texture->width(), texture->height());
        } else {
            std::string tpsName = name.substr(0, pos) + ".json";
            std::string fileName = name.substr(pos + 1);

            TPSMap::const_iterator it = tpsMap_.find(tpsName);

            if (it == tpsMap_.end()) {
                PlatformIFStream is(tpsName);

                if (!is) {
                    TexturePtr texture = textureManager.loadTexture(name,
                                                                    wrapX, wrapY);

                    if (!texture) {
                        return getBadImage();
                    }

                    it = tpsMap_.insert(std::make_pair(tpsName, TPSPtr())).first;
                } else {
                    TPSPtr tps = TPS::fromStream(tpsName, is);

                    if (!tps) {
                        return getBadImage();
                    }

                    it = tpsMap_.insert(std::make_pair(tpsName, tps)).first;
                }
            }

            if (!it->second) {
                TexturePtr texture = textureManager.loadTexture(name,
                                                                wrapX, wrapY);

                if (!texture) {
                    return getBadImage();
                }

                return Image(texture, 0, 0, texture->width(), texture->height());
            }

            TexturePtr texture = textureManager.loadTexture(it->second->imageFileName(),
                                                            wrapX, wrapY);

            if (!texture) {
                return getBadImage();
            }

            TPSEntry entry = it->second->entry(fileName);

            if (entry.valid()) {
                return Image(texture, entry.x, entry.y, entry.width, entry.height);
            } else {
                return getBadImage();
            }
        }
    }

    DrawablePtr AssetManager::getBadDrawable()
    {
        return boost::make_shared<Drawable>(getBadImage());
    }

    DrawablePtr AssetManager::getDrawable(const std::string& name,
                                          Texture::WrapMode wrapX,
                                          Texture::WrapMode wrapY)
    {
        return boost::make_shared<Drawable>(getImage(name, wrapX, wrapY));
    }

    SceneObjectAssetPtr AssetManager::getSceneObjectAsset(const std::string& name)
    {
        assetTouched(name);

        WorldMap::const_iterator it = worldMap_.find(name);

        if (it == worldMap_.end()) {
            PlatformIFStream is(name);

            RUBEWorldPtr world = RUBEWorld::fromStream(name, is);

            if (!world) {
                return SceneObjectAssetPtr();
            }

            it = worldMap_.insert(std::make_pair(name, world)).first;
        }

        return boost::make_shared<SceneObjectAsset>(it->second);
    }

    SceneAssetPtr AssetManager::getSceneAsset(const std::string& name)
    {
        assetTouched(name);

        WorldMap::const_iterator it = worldMap_.find(name);

        if (it == worldMap_.end()) {
            PlatformIFStream is(name);

            RUBEWorldPtr world = RUBEWorld::fromStream(name, is);

            if (!world) {
                return SceneAssetPtr();
            }

            it = worldMap_.insert(std::make_pair(name, world)).first;
        }

        return boost::make_shared<SceneAsset>(it->second);
    }

    ParticleEffectComponentPtr AssetManager::getParticleEffect(const std::string& name,
                                                               const b2Vec2& pos,
                                                               float angle,
                                                               bool fixedPos)
    {
        assetTouched(name);

        GdxPMap::const_iterator it = gdxpMap_.find(name);

        if (it == gdxpMap_.end()) {
            PlatformIFStream is(name);

            GdxPPtr gdxp = GdxP::fromStream(name, is);

            if (!gdxp) {
                return ParticleEffectComponentPtr();
            }

            it = gdxpMap_.insert(std::make_pair(name, gdxp)).first;
        }

        ParticleEffectComponentPtr component =
            boost::make_shared<ParticleEffectComponent>();

        for (int i = 0; i < it->second->numEntries(); ++i) {
            ParticleEmmiterPtr emmiter =
                boost::make_shared<ParticleEmmiter>(pos, angle,
                                                    it->second->entry(i),
                                                    fixedPos);
            component->addEmmiter(emmiter);
        }

        return component;
    }

    Animation AssetManager::getAnimation(const std::string& name, float factor)
    {
        AnimationMap::const_iterator it = animationMap_.find(name);

        if (it == animationMap_.end()) {
            std::string fileName = std::string("anim/") + name + ".json";

            PlatformIFStream is(fileName);

            Animation anim = Animation::fromStream(fileName, is);

            it = animationMap_.insert(std::make_pair(name, anim)).first;
        }

        if (factor == 1.0f) {
            return it->second;
        } else {
            return it->second.clone(factor);
        }
    }

    const CharMap& AssetManager::charMap()
    {
        if (!charMap_.empty()) {
            return charMap_;
        }

        LOG4CPLUS_INFO(logger(), "Creating charmap...");

        typedef std::vector<std::pair<UInt16, UInt16> > CharRanges;

        CharRanges charRanges;

        charRanges.push_back(std::make_pair(0, 0xff)); // en-us
        charRanges.push_back(std::make_pair(0x400, 0x4ff)); // cyrillic
        charRanges.push_back(std::make_pair(0x221e, 0x221e)); // infinity

        PlatformIFStream is("Days.otf");

        if (!is) {
            LOG4CPLUS_ERROR(logger(), "Unable to open game font");
            return charMap_;
        }

        std::string fontData;

        if (!readStream(is, fontData)) {
            LOG4CPLUS_ERROR(logger(), "Unable to read game font");
            return charMap_;
        }

        FT_Library library = NULL;

        if (FT_Init_FreeType(&library) != 0) {
            LOG4CPLUS_ERROR(logger(), "Unable to init freetype");
            return charMap_;
        }

        FT_Face face = NULL;

        if (FT_New_Memory_Face(library, (const FT_Byte*)&fontData[0], fontData.size(), 0, &face) != 0) {
            LOG4CPLUS_ERROR(logger(), "Unable to init game font");
            FT_Done_FreeType(library);
            return charMap_;
        }

        FT_Set_Char_Size(face, 0, 50 << 6, 0, 0);

        int texSize = 1024;

        std::vector<Byte> data(texSize * texSize * 4);

        int yOffset = 0;
        int glyphMaxHeight = 0;

        for (CharRanges::const_iterator it = charRanges.begin(); it != charRanges.end(); ++ it) {
            for (UInt16 c = it->first; c <= it->second; ++c) {
                int index = FT_Get_Char_Index(face, c);
                if (index == 0) {
                    continue;
                }

                if (FT_Load_Glyph(face, index, 0) != 0) {
                    LOG4CPLUS_WARN(logger(), "Unable to load glyph for character " << c);
                    continue;
                }

                FT_GlyphSlot glyph = face->glyph;

                int height = (glyph->metrics.horiBearingY >> 6);
                if (height > glyphMaxHeight) {
                    glyphMaxHeight = height;
                }

                int offset = (glyph->metrics.height >> 6) - height;
                if (offset > yOffset) {
                    yOffset = offset;
                }
            }
        }

        glyphMaxHeight += yOffset;

        int margin = 0;
        int spacing = 4;

        glyphMaxHeight += 2 * margin;

        int x = spacing;
        int y = spacing;

        for (CharRanges::const_iterator it = charRanges.begin(); it != charRanges.end(); ++ it) {
            for (UInt16 c = it->first; c <= it->second; ++c) {
                int index = FT_Get_Char_Index(face, c);
                if (index == 0) {
                    continue;
                }

                if (FT_Load_Glyph(face, index, 0) != 0) {
                    LOG4CPLUS_WARN(logger(), "Unable to load glyph for character " << c);
                    continue;
                }

                FT_GlyphSlot glyph = face->glyph;

                if (FT_Render_Glyph(glyph, FT_RENDER_MODE_NORMAL) != 0) {
                    LOG4CPLUS_WARN(logger(), "Unable to render glyph for character " << c);
                    continue;
                }

                if (glyph->bitmap.pixel_mode != FT_PIXEL_MODE_GRAY) {
                    LOG4CPLUS_WARN(logger(), "Bad glyph pixel mode for character " << c);
                    continue;
                }

                int glyphX = (glyph->metrics.horiBearingX >> 6);
                int glyphY = glyphMaxHeight - yOffset - (glyph->metrics.horiBearingY >> 6);
                int glyphWidth = glyph->metrics.width >> 6;
                int glyphHeight = glyph->metrics.height >> 6;
                int glyphAdvance = glyph->metrics.horiAdvance >> 6;

                if (glyphX < 0) {
                    glyphAdvance -= glyphX;
                    glyphX = 0;
                }

                if (glyphX + glyphWidth > glyphAdvance) {
                    glyphAdvance = glyphX + glyphWidth;
                }

                glyphX += margin;
                glyphY -= margin;
                glyphAdvance += 2 * margin;

                assert(glyphX >= 0);
                assert(glyphY >= 0);
                assert(glyphX + glyphWidth <= glyphAdvance);
                assert(glyphY + glyphHeight <= glyphMaxHeight);
                assert(static_cast<int>(glyph->bitmap.width) == glyphWidth);
                assert(static_cast<int>(glyph->bitmap.rows) == glyphHeight);

                if (x + glyphAdvance + spacing > texSize) {
                    if (y + glyphMaxHeight + spacing > texSize) {
                        LOG4CPLUS_ERROR(logger(), "Unable to fit glyph for character " << c);
                        break;
                    }
                    y += glyphMaxHeight + spacing;
                    x = spacing;
                }

                for (int i = 0; i < glyphHeight; ++i) {
                    for (int j = 0; j < glyphWidth; ++j) {
                        char color = glyph->bitmap.buffer[i * glyph->bitmap.pitch + j];
                        data[(y + glyphY + i) * texSize * 4 + (x + glyphX + j) * 4 + 0] = color;
                        data[(y + glyphY + i) * texSize * 4 + (x + glyphX + j) * 4 + 1] = color;
                        data[(y + glyphY + i) * texSize * 4 + (x + glyphX + j) * 4 + 2] = color;
                        data[(y + glyphY + i) * texSize * 4 + (x + glyphX + j) * 4 + 3] = color;
                    }
                }

                charMap_[c] = Image(TexturePtr(), x, y, glyphAdvance, glyphMaxHeight);

                x += glyphAdvance + spacing;
            }
        }

        FT_Done_Face(face);
        FT_Done_FreeType(library);

        TexturePtr tex = textureManager.genTexture(&data[0], texSize, texSize,
            Texture::WrapModeClamp, Texture::WrapModeClamp);

        for (CharMap::iterator it = charMap_.begin(); it != charMap_.end(); ++it) {
            it->second = Image(tex, it->second.x(), it->second.y(),
                it->second.width(), it->second.height());
        }

        LOG4CPLUS_INFO(logger(), "Charmap created");

        return charMap_;
    }

    void AssetManager::assetTouched(const std::string& name)
    {
        if (settings.dumpAssetList) {
            assetDump.insert(name);
        }
    }
}
