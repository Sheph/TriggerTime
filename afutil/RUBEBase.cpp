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

#include "af/RUBEBase.h"
#include "Logger.h"
#include <json/json.h>

namespace af
{
    RUBEBase::RUBEBase(const std::string& name)
    : name_(name)
    {
    }

    RUBEBase::~RUBEBase()
    {
    }

    void RUBEBase::setStringProp(const std::string& key, const std::string& value)
    {
        props_.insert(std::make_pair(key, value));
    }

    void RUBEBase::setFloatProp(const std::string& key, float value)
    {
        props_.insert(std::make_pair(key, value));
    }

    void RUBEBase::setIntProp(const std::string& key, int value)
    {
        props_.insert(std::make_pair(key, value));
    }

    void RUBEBase::setVec2Prop(const std::string& key, const b2Vec2& value)
    {
        props_.insert(std::make_pair(key, value));
    }

    void RUBEBase::setBoolProp(const std::string& key, bool value)
    {
        props_.insert(std::make_pair(key, value));
    }

    void RUBEBase::setColorProp(const std::string& key, const Color& value)
    {
        props_.insert(std::make_pair(key, value));
    }

    bool RUBEBase::haveProp(const std::string& key) const
    {
        PropMap::const_iterator it = props_.find(key);

        return it != props_.end();
    }

    std::string RUBEBase::stringProp(const std::string& key) const
    {
        PropMap::const_iterator it = props_.find(key);

        if (it == props_.end()) {
            return "";
        }

        try {
            return boost::any_cast<std::string>(it->second);
        } catch(const boost::bad_any_cast&) {
            LOG4CPLUS_WARN(afutil::logger(),
                           "RUBE property \"" << key << "\" is not a string");
            return "";
        }
    }

    float RUBEBase::floatProp(const std::string& key, float def) const
    {
        PropMap::const_iterator it = props_.find(key);

        if (it == props_.end()) {
            return def;
        }

        try {
            return boost::any_cast<float>(it->second);
        } catch(const boost::bad_any_cast&) {
            LOG4CPLUS_WARN(afutil::logger(),
                           "RUBE property \"" << key << "\" is not a float");
            return def;
        }
    }

    int RUBEBase::intProp(const std::string& key, int def) const
    {
        PropMap::const_iterator it = props_.find(key);

        if (it == props_.end()) {
            return def;
        }

        try {
            return boost::any_cast<int>(it->second);
        } catch(const boost::bad_any_cast&) {
            LOG4CPLUS_WARN(afutil::logger(),
                           "RUBE property \"" << key << "\" is not an int");
            return def;
        }
    }

    b2Vec2 RUBEBase::vec2Prop(const std::string& key) const
    {
        PropMap::const_iterator it = props_.find(key);

        if (it == props_.end()) {
            return b2Vec2(0.0f, 0.0f);
        }

        try {
            return boost::any_cast<b2Vec2>(it->second);
        } catch(const boost::bad_any_cast&) {
            LOG4CPLUS_WARN(afutil::logger(),
                           "RUBE property \"" << key << "\" is not a vec2");
            return b2Vec2(0.0f, 0.0f);
        }
    }

    bool RUBEBase::boolProp(const std::string& key, bool def) const
    {
        PropMap::const_iterator it = props_.find(key);

        if (it == props_.end()) {
            return def;
        }

        try {
            return boost::any_cast<bool>(it->second);
        } catch(const boost::bad_any_cast&) {
            LOG4CPLUS_WARN(afutil::logger(),
                           "RUBE property \"" << key << "\" is not a bool");
            return def;
        }
    }

    Color RUBEBase::colorProp(const std::string& key, const Color& def) const
    {
        PropMap::const_iterator it = props_.find(key);

        if (it == props_.end()) {
            return def;
        }

        try {
            return boost::any_cast<Color>(it->second);
        } catch(const boost::bad_any_cast&) {
            LOG4CPLUS_WARN(afutil::logger(),
                           "RUBE property \"" << key << "\" is not a color");
            return def;
        }
    }

    float RUBEBase::hex2float(const std::string& str)
    {
        int strLen = 8;
        unsigned char bytes[4];
        int bptr = (strLen / 2) - 1;

        for (int i = 0; i < strLen; i++) {
            unsigned char c = str[i];
            if (c > '9') {
                c -= 7;
            }

            c <<= 4;
            bytes[bptr] = c;
            ++i;

            c = str[i];
            if (c > '9') {
                c -= 7;
            }

            c -= '0';
            bytes[bptr] |= c;
            --bptr;
        }

        float* f = reinterpret_cast<float*>(bytes);

        return *f;
    }

    float RUBEBase::json2float(const Json::Value& jsonValue, int index, float defValue)
    {
        if (index > -1) {
            if (jsonValue[index].isNull()) {
                return defValue;
            } else if (jsonValue[index].isInt()) {
                return static_cast<float>(jsonValue[index].asInt());
            } else if (jsonValue[index].isString()) {
                return hex2float(jsonValue[index].asString());
            } else {
                return jsonValue[index].asFloat();
            }
        } else {
            if (jsonValue.isNull()) {
                return defValue;
            } else if (jsonValue.isInt()) {
                return static_cast<float>(jsonValue.asInt());
            } else if (jsonValue.isString()) {
                return hex2float(jsonValue.asString());
            } else {
                return jsonValue.asFloat();
            }
        }
    }

    b2Vec2 RUBEBase::json2vec2(const Json::Value& jsonValue, int index)
    {
        b2Vec2 vec = b2Vec2(0.0f, 0.0f);

        if (index > -1) {
            if (jsonValue["x"][index].isInt()) {
                vec.x = static_cast<float>(jsonValue["x"][index].asInt());
            } else if (jsonValue["x"][index].isString()) {
                vec.x = hex2float(jsonValue["x"][index].asString());
            } else {
                vec.x = jsonValue["x"][index].asFloat();
            }

            if (jsonValue["y"][index].isInt()) {
                vec.y = static_cast<float>(jsonValue["y"][index].asInt());
            } else if (jsonValue["y"][index].isString()) {
                vec.y = hex2float(jsonValue["y"][index].asString());
            } else {
                vec.y = jsonValue["y"][index].asFloat();
            }
        } else {
            if (jsonValue.isInt()) {
                vec.Set(0.0f, 0.0f);
            } else {
                vec.x = json2float(jsonValue["x"]);
                vec.y = json2float(jsonValue["y"]);
            }
        }

        return vec;
    }

    void RUBEBase::setProps(const Json::Value& jsonValue)
    {
        if (!jsonValue.isMember("customProperties")) {
            return;
        }

        int i = 0;
        Json::Value propValue = jsonValue["customProperties"][i];

        while (!propValue.isNull()) {
            i++;
            std::string key = propValue.get("name", "").asString();

            if (propValue.isMember("string")) {
                std::string value = propValue.get("string", 0).asString();
                setStringProp(key, value);
            } else if (propValue.isMember("float")) {
                float value;
                if (propValue.get("float", 0).isString()) {
                    value = hex2float(propValue.get("float", 0).asString());
                } else {
                    value = propValue.get("float", 0).asFloat();
                }
                setFloatProp(key, value);
            } else if (propValue.isMember("int")) {
                int value = propValue.get("int", 0).asInt();
                setIntProp(key, value);
            } else if (propValue.isMember("vec2")) {
                b2Vec2 value = json2vec2(propValue.get("vec2", 0));
                setVec2Prop(key, value);
            } else if (propValue.isMember("bool")) {
                bool value = propValue.get("bool", 0).asBool();
                setBoolProp(key, value);
            } else if (propValue.isMember("color")) {
                Json::Value c = propValue.get("color", 0);
                Color value(1.0f, 1.0f, 1.0f, 1.0f);
                for (int i = 0; i < 4; i++) {
                    if (c[i].isInt()) {
                        value.rgba[i] = static_cast<float>(c[i].asInt()) / 255.0f;
                    }
                }
                setColorProp(key, value);
            }

            propValue = jsonValue["customProperties"][i];
        }
    }

    bool RUBEBase::json2enumInternal(const Json::Value& jsonValue,
                                     const int* intValues,
                                     const char** strValues,
                                     int numValues,
                                     int& ret)
    {
        if (jsonValue.isInt()) {
            ret = jsonValue.asInt();

            for (int i = 0; i < numValues; ++i) {
                if (ret == intValues[i]) {
                    return true;
                }
            }
        } else {
            std::string str = jsonValue.asString();

            ret = 0;

            for (int i = 0; i < numValues; ++i) {
                if (str == strValues[i]) {
                    ret = intValues[i];
                    return true;
                }
            }
        }

        LOG4CPLUS_WARN(afutil::logger(), "enum field expected");

        return false;
    }

    bool RUBEBase::enumPropInternal(const std::string& key,
                                    const int* intValues,
                                    const char** strValues,
                                    int numValues,
                                    int& ret) const
    {
        PropMap::const_iterator it = props_.find(key);

        if (it == props_.end()) {
            return false;
        }

        if (it->second.type() == typeid(int)) {
            ret = boost::any_cast<int>(it->second);

            for (int i = 0; i < numValues; ++i) {
                if (ret == intValues[i]) {
                    return true;
                }
            }
        } else if (it->second.type() == typeid(std::string)) {
            std::string str = boost::any_cast<std::string>(it->second);

            ret = 0;

            for (int i = 0; i < numValues; ++i) {
                if (str == strValues[i]) {
                    ret = intValues[i];
                    return true;
                }
            }
        } else {
            LOG4CPLUS_WARN(afutil::logger(),
                           "RUBE property \"" << key << "\" is not an enum");
            return false;
        }

        LOG4CPLUS_WARN(afutil::logger(),
                       "RUBE property \"" << key << "\" has bad enum value");

        return false;
    }
}
