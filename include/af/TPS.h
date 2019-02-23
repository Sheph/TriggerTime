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

#ifndef _AF_TPS_H_
#define _AF_TPS_H_

#include "af/Types.h"
#include <json/json-forwards.h>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <map>
#include <iostream>

namespace af
{
    class TPS;
    typedef boost::shared_ptr<TPS> TPSPtr;

    struct TPSEntry
    {
        TPSEntry()
        : x(0),
          y(0),
          width(0),
          height(0)
        {
        }

        TPSEntry(UInt32 x,
                 UInt32 y,
                 UInt32 width,
                 UInt32 height)
        : x(x),
          y(y),
          width(width),
          height(height)
        {
        }

        inline bool valid() const { return (width > 0) && (height > 0); }

        UInt32 x;
        UInt32 y;
        UInt32 width;
        UInt32 height;
    };

    class TPS : boost::noncopyable
    {
    public:
        TPS(const std::string& imageFileName,
            UInt32 width,
            UInt32 height);
        ~TPS();

        static TPSPtr fromStream(const std::string& fileName, std::istream& is);

        static TPSPtr fromString(const std::string& fileName, const std::string& json);

        static TPSPtr fromJsonValue(const Json::Value& jsonValue);

        inline const std::string& imageFileName() const { return imageFileName_; }
        inline UInt32 width() const { return width_; }
        inline UInt32 height() const { return height_; }

        void addEntry(const std::string& fileName, const TPSEntry& entry);

        const TPSEntry& entry(const std::string& fileName) const;

    private:
        typedef std::map<std::string, TPSEntry> Entries;

        std::string imageFileName_;
        UInt32 width_;
        UInt32 height_;
        Entries entries_;
    };
}

#endif
