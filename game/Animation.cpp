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

#include "Animation.h"
#include "AssetManager.h"
#include "Logger.h"
#include "af/Utils.h"
#include <json/json.h>
#include <log4cplus/ndc.h>
#include <cmath>

namespace af
{
    Animation::Animation(bool loop)
    : loop_(loop),
      duration_(0.0f)
    {
    }

    Animation::~Animation()
    {
    }

    Animation Animation::fromStream(const std::string& fileName, std::istream& is)
    {
        Animation anim;

        std::string json;

        if (!is) {
            log4cplus::NDCContextCreator ndc(fileName);

            LOG4CPLUS_ERROR(logger(), "Cannot open file");

            return anim;
        }

        if (!readStream(is, json)) {
            log4cplus::NDCContextCreator ndc(fileName);

            LOG4CPLUS_ERROR(logger(), "Error reading file");

            return anim;
        }

        Json::Value jsonValue;
        Json::Reader reader;

        if (!reader.parse(json, jsonValue)) {
            log4cplus::NDCContextCreator ndc(fileName);

            LOG4CPLUS_ERROR(logger(),
                "Failed to parse JSON: " << reader.getFormattedErrorMessages());
            return anim;
        }

        anim.setLoop(jsonValue["loop"].asBool());

        Texture::WrapMode wrapX = jsonValue["clampX"].asBool() ? Texture::WrapModeClamp : Texture::WrapModeRepeat;
        Texture::WrapMode wrapY = jsonValue["clampY"].asBool() ? Texture::WrapModeClamp : Texture::WrapModeRepeat;

        const Json::Value& frames = jsonValue["frames"];

        std::istringstream iss;
        std::ostringstream oss;

        int i = 0;
        Json::Value frame = frames[i];
        while (!frame.isNull()) {
            std::string fn = frame[0].asString();
            float dur = frame[1].asFloat();

            if (dur != 0.0f) {
                dur = 1.0f / dur;
            }

            int index1 = 0;
            int index2 = 0;
            bool ranged = false;

            std::string::size_type pos,
                pos2 = std::string::npos, pos3 = std::string::npos;

            pos = fn.find_first_of('%');
            if (pos != std::string::npos) {
                pos2 = fn.find_first_of('-', pos + 1);
                if (pos2 != std::string::npos) {
                    std::string tmp = fn.substr(pos + 1, pos2 - pos - 1);

                    iss.str(tmp);
                    iss.clear();

                    if ((iss >> index1) && iss.eof()) {
                        pos3 = fn.find_first_of('%', pos2 + 1);
                        if (pos3 != std::string::npos) {
                            tmp = fn.substr(pos2 + 1, pos3 - pos2 - 1);

                            iss.str(tmp);
                            iss.clear();

                            if ((iss >> index2) && iss.eof()) {
                                ranged = true;
                            }
                        }
                    }
                }
            }

            if (ranged) {
                std::string p1 = fn.substr(0, pos);
                std::string p2 = fn.substr(pos3 + 1);
                if (index2 >= index1) {
                    for (int i = index1; i <= index2; ++i) {
                        oss.str("");
                        oss << p1 << i << p2;
                        anim.addFrame(assetManager.getImage(oss.str(), wrapX, wrapY), dur);
                    }
                } else {
                    for (int i = index1; i >= index2; --i) {
                        oss.str("");
                        oss << p1 << i << p2;
                        anim.addFrame(assetManager.getImage(oss.str(), wrapX, wrapY), dur);
                    }
                }
            } else {
                anim.addFrame(assetManager.getImage(fn, wrapX, wrapY), dur);
            }

            frame = frames[++i];
        }

        const Json::Value& specials = jsonValue["specials"];

        i = 0;
        Json::Value special = specials[i];
        while (!special.isNull()) {
            anim.addSpecialIndex(special.asInt());
            special = specials[++i];
        }

        return anim;
    }

    void Animation::addFrame(const Image& image, float duration)
    {
        frames_.push_back(std::make_pair(image, duration));
        duration_ += duration;
    }

    Image Animation::getFrame(float timeVal) const
    {
        int i = getFrameIndex(timeVal);

        if (i < 0) {
            return assetManager.getBadImage();
        }

        return frames_[i].first;
    }

    int Animation::getFrameIndex(float timeVal) const
    {
        if (frames_.empty()) {
            return -1;
        }

        if (loop_ && (duration_ > 0.0f)) {
            timeVal = std::fmod(timeVal, duration_);
        }

        if (timeVal < duration_) {
            float tmp = 0.0f;
            for (size_t i = 0; i < frames_.size(); ++i) {
                tmp += frames_[i].second;
                if (tmp > timeVal) {
                    return i;
                }
            }
        }

        return frames_.size() - 1;
    }

    bool Animation::finished(float timeVal) const
    {
        if (frames_.empty()) {
            return true;
        } else {
            return (!loop_ && (timeVal >= duration_));
        }
    }

    void Animation::addSpecialIndex(int index)
    {
        specials_.push_back(index);
    }

    int Animation::getSpecialIndex(int i) const
    {
        if ((i < 0) || (i >= static_cast<int>(specials_.size()))) {
            LOG4CPLUS_ERROR(logger(), "special index " << i << " doesn't exist");
            return 0;
        }

        return specials_[i];
    }

    Animation Animation::clone(float factor) const
    {
        Animation anim(loop_);

        for (size_t i = 0; i < frames_.size(); ++i) {
            anim.addFrame(frames_[i].first, frames_[i].second * factor);
        }

        return anim;
    }
}
