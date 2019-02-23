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

#ifndef _AF_APPCONFIG_H_
#define _AF_APPCONFIG_H_

#include "af/Types.h"
#include <boost/shared_ptr.hpp>
#include <vector>

namespace af
{
    class AppConfig
    {
    public:
        AppConfig() {}
        virtual ~AppConfig() {}

        /*
         * Returns log4cplus config as a string. The string can be
         * fed right away to log4cplus::PropertyConfigurator.
         */
        virtual std::string getLoggerConfig() const = 0;

        virtual bool haveKey(const std::string& key) const = 0;

        /*
         * Gets first level subkeys of a 'key'. For example, if we have config
         * like this:
         *
         * global_key.1=global_value1
         * global_key.2=global_value2
         * global_key3=global_value3
         * [section1]
         * key1.1=a1
         * key1.2=b1
         * key1.3=c1
         * key2.1=a2
         * key2.2=b2
         * key2.3=c2
         * [section2]
         * k2=v2
         *
         * Then getSubKeys("section1") will yield "key1" and "key2".
         * getSubKeys("section1.key1") will yield "1", "2" and "3".
         * getSubKeys() will yield "section1" and "section2".
         * getSubKeys(".") will yield "global_key" and "global_key3".
         * getSubKeys(".global_key") will yield "1" and "2".
         */
        virtual std::vector<std::string> getSubKeys(const std::string& key = "") const = 0;

        virtual std::string getString(const std::string& key) const = 0;

        virtual int getStringIndex(const std::string& key,
                                   const std::vector<std::string>& allowed) const = 0;

        virtual int getInt(const std::string& key) const = 0;

        virtual float getFloat(const std::string& key) const = 0;

        /*
         * true values are - "yes", "true", "1", "y".
         * false values are - "no", "false", "0", "n".
         */
        virtual bool getBool(const std::string& key) const = 0;

        /*
         * value is in form "1.0, 0.5, 0.8, 1.0"
         */
        virtual Color getColor(const std::string& key) const = 0;

        /*
         * value is in form "1.0, 0.5"
         */
        virtual b2Vec2 getVec2(const std::string& key) const = 0;
    };

    typedef boost::shared_ptr<AppConfig> AppConfigPtr;
}

#endif
