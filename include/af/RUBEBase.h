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

#ifndef _AF_RUBEBASE_H_
#define _AF_RUBEBASE_H_

#include "af/Types.h"
#include "af/EnumTraits.h"
#include <boost/noncopyable.hpp>
#include <boost/any.hpp>
#include <map>
#include <json/json-forwards.h>

namespace af
{
    class RUBEBase : boost::noncopyable
    {
    public:
        explicit RUBEBase(const std::string& name);
        virtual ~RUBEBase();

        inline const std::string& name() const { return name_; }

        void setStringProp(const std::string& key, const std::string& value);
        void setFloatProp(const std::string& key, float value);
        void setIntProp(const std::string& key, int value);
        void setVec2Prop(const std::string& key, const b2Vec2& value);
        void setBoolProp(const std::string& key, bool value);
        void setColorProp(const std::string& key, const Color& value);

        bool haveProp(const std::string& key) const;
        std::string stringProp(const std::string& key) const;
        float floatProp(const std::string& key, float def = 0.0f) const;
        int intProp(const std::string& key, int def = 0) const;
        b2Vec2 vec2Prop(const std::string& key) const;
        bool boolProp(const std::string& key, bool def = false) const;
        Color colorProp(const std::string& key, const Color& def = Color(1.0f, 1.0f, 1.0f, 1.0f)) const;

        template <class T>
        T enumProp(const std::string& key, T defValue) const
        {
            int tmp;

            if (!enumPropInternal(key,
                                  EnumTraits<T>::intValues,
                                  EnumTraits<T>::strValues,
                                  EnumTraits<T>::numValues,
                                  tmp)) {
                return defValue;
            }

            return static_cast<T>(tmp);
        }

    protected:
        static float hex2float(const std::string& str);

        static float json2float(const Json::Value& jsonValue, int index = -1, float defValue = 0.0f);

        static b2Vec2 json2vec2(const Json::Value& jsonValue, int index = -1);

        template <class T>
        static T json2enum(const Json::Value& jsonValue, T defValue)
        {
            int tmp;

            if (!json2enumInternal(jsonValue,
                                   EnumTraits<T>::intValues,
                                   EnumTraits<T>::strValues,
                                   EnumTraits<T>::numValues,
                                   tmp)) {
                return defValue;
            }

            return static_cast<T>(tmp);
        }

        void setProps(const Json::Value& jsonValue);

    private:
        typedef std::map<std::string, boost::any> PropMap;

        static bool json2enumInternal(const Json::Value& jsonValue,
                                      const int* intValues,
                                      const char** strValues,
                                      int numValues,
                                      int& ret);

        bool enumPropInternal(const std::string& key,
                              const int* intValues,
                              const char** strValues,
                              int numValues,
                              int& ret) const;

        std::string name_;
        PropMap props_;
    };
}

#endif
