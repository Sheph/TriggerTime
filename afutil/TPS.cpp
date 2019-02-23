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

#include "af/TPS.h"
#include "af/Utils.h"
#include "Logger.h"
#include <boost/make_shared.hpp>
#include <json/json.h>
#include <log4cplus/ndc.h>

namespace af
{
    static const TPSEntry dummyEntry;

    TPS::TPS(const std::string& imageFileName,
             UInt32 width,
             UInt32 height)
    : imageFileName_(imageFileName),
      width_(width),
      height_(height)
    {
    }

    TPS::~TPS()
    {
    }

    TPSPtr TPS::fromStream(const std::string& fileName, std::istream& is)
    {
        std::string json;

        {
            log4cplus::NDCContextCreator ndc(fileName);

            if (!is) {
                LOG4CPLUS_ERROR(afutil::logger(), "Cannot open file");

                return TPSPtr();
            }

            if (!readStream(is, json)) {
                LOG4CPLUS_ERROR(afutil::logger(), "Error reading file");

                return TPSPtr();
            }
        }

        return fromString(fileName, json);
    }

    TPSPtr TPS::fromString(const std::string& fileName, const std::string& json)
    {
        Json::Value jsonValue;
        Json::Reader reader;

        log4cplus::NDCContextCreator ndc(fileName);

        if (!reader.parse(json, jsonValue)) {
            LOG4CPLUS_ERROR(afutil::logger(),
                            "Failed to parse JSON: " << reader.getFormattedErrorMessages());
            return TPSPtr();
        }

        LOG4CPLUS_DEBUG(afutil::logger(), "Processing...");

        return fromJsonValue(jsonValue);
    }

    TPSPtr TPS::fromJsonValue(const Json::Value& jsonValue)
    {
        std::string imageFileName = jsonValue["meta"]["image"].asString();
        UInt32 width = jsonValue["meta"]["size"]["w"].asUInt();
        UInt32 height = jsonValue["meta"]["size"]["h"].asUInt();

        TPSPtr tps = boost::make_shared<TPS>(imageFileName, width, height);

        int i = 0;
        Json::Value tmp = jsonValue["frames"][i];
        while (!tmp.isNull()) {
            std::ostringstream os;
            os << "frame " << i++;
            log4cplus::NDCContextCreator ndc(os.str());

            std::string fileName = tmp["filename"].asString();
            UInt32 x = tmp["frame"]["x"].asUInt();
            UInt32 y = tmp["frame"]["y"].asUInt();
            UInt32 width = tmp["frame"]["w"].asUInt();
            UInt32 height = tmp["frame"]["h"].asUInt();

            TPSEntry entry(x, y, width, height);

            tps->addEntry(fileName, entry);

            tmp = jsonValue["frames"][i];
        }

        return tps;
    }

    void TPS::addEntry(const std::string& fileName, const TPSEntry& entry)
    {
        if (!entries_.insert(std::make_pair(fileName, entry)).second) {
            LOG4CPLUS_WARN(afutil::logger(),
                           "File \"" << fileName << "\" already in TPS \"" << imageFileName_ << "\"");
        }
    }

    const TPSEntry& TPS::entry(const std::string& fileName) const
    {
        Entries::const_iterator it = entries_.find(fileName);

        if (it == entries_.end()) {
            LOG4CPLUS_WARN(afutil::logger(),
                           "File \"" << fileName << "\" not found in TPS \"" << imageFileName_ << "\"");
            return dummyEntry;
        }

        return it->second;
    }
}
