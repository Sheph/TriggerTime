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

#include "LRSystemInterface.h"
#include "Logger.h"
#include <log4cplus/ndc.h>
#include "luainc.h"

namespace af
{
    LRSystemInterface::LRSystemInterface()
    : L_(NULL),
      t_(0.0f)
    {
    }

    LRSystemInterface::~LRSystemInterface()
    {
    }

    float LRSystemInterface::GetElapsedTime()
    {
        return t_;
    }

    bool LRSystemInterface::LogMessage(Rocket::Core::Log::Type type, const Rocket::Core::String& message)
    {
        log4cplus::LogLevel level = log4cplus::OFF_LOG_LEVEL;

        switch (type) {
        case Rocket::Core::Log::LT_ALWAYS: {
            logger().forcedLog(log4cplus::INFO_LOG_LEVEL, message.CString(), __FILE__, __LINE__);
            break;
        }
        case Rocket::Core::Log::LT_ERROR: level = log4cplus::ERROR_LOG_LEVEL; break;
        case Rocket::Core::Log::LT_ASSERT: level = log4cplus::FATAL_LOG_LEVEL; break;
        case Rocket::Core::Log::LT_WARNING: level = log4cplus::WARN_LOG_LEVEL; break;
        case Rocket::Core::Log::LT_INFO: level = log4cplus::INFO_LOG_LEVEL; break;
        case Rocket::Core::Log::LT_DEBUG:
        default:
            level = log4cplus::DEBUG_LOG_LEVEL;
            break;
        }

        log4cplus::NDCContextCreator ndc("rocket");

        logger().log(level, message.CString(), __FILE__, __LINE__);

        return true;
    }

    int LRSystemInterface::TranslateString(Rocket::Core::String& translated, const Rocket::Core::String& input)
    {
        if (!L_ || input.Empty()) {
            translated = input;
            return 0;
        }

        int pos1;
        int pos2;

        for (pos1 = 0; pos1 < static_cast<int>(input.Length()); ++pos1) {
            if ((input[pos1] != ' ') &&
                (input[pos1] != '\t') &&
                (input[pos1] != '\r') &&
                (input[pos1] != '\n')) {
                if (input[pos1] != '[') {
                    translated = input;
                    return 0;
                } else {
                    break;
                }
            }
        }

        if (pos1 == static_cast<int>(input.Length())) {
            translated = input;
            return 0;
        }

        for (pos2 = static_cast<int>(input.Length() - 1); pos2 >= 0; --pos2) {
            if ((input[pos2] != ' ') &&
                (input[pos2] != '\t') &&
                (input[pos2] != '\r') &&
                (input[pos2] != '\n')) {
                if (input[pos2] != ']') {
                    translated = input;
                    return 0;
                } else {
                    break;
                }
            }
        }

        if (pos1 + 1 >= pos2) {
            translated = input;
            return 0;
        }

        const Rocket::Core::String& tmp = input.Substring(pos1 + 1, pos2 - pos1 - 1);

        lua_getglobal(L_, "tr");
        lua_getfield(L_, -1, "ui");
        lua_getfield(L_, -1, tmp.CString());

        const char* s = lua_tostring(L_, -1);

        if (s == NULL) {
            translated = tmp;
            lua_pop(L_, 3);
            return 1;
        }

        translated = s;
        lua_pop(L_, 3);

        return 1;
    }
}
