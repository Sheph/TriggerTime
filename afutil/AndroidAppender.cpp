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

#include "af/AndroidAppender.h"
#include <log4cplus/streams.h>
#include <log4cplus/spi/loggingevent.h>
#include <android/log.h>
#include <sstream>

namespace log4cplus
{
    static android_LogPriority translateLogLevel(LogLevel logLevel)
    {
        switch (logLevel) {
        case TRACE_LOG_LEVEL: return ANDROID_LOG_VERBOSE;
        case DEBUG_LOG_LEVEL: return ANDROID_LOG_DEBUG;
        case INFO_LOG_LEVEL: return ANDROID_LOG_INFO;
        case WARN_LOG_LEVEL: return ANDROID_LOG_WARN;
        case ERROR_LOG_LEVEL: return ANDROID_LOG_ERROR;
        case FATAL_LOG_LEVEL: return ANDROID_LOG_FATAL;
        case OFF_LOG_LEVEL: return ANDROID_LOG_SILENT;
        case NOT_SET_LOG_LEVEL: return ANDROID_LOG_UNKNOWN;
        default: return ANDROID_LOG_UNKNOWN;
        }
    }

    AndroidAppender::AndroidAppender()
    {
    }

    AndroidAppender::AndroidAppender(const helpers::Properties& props)
    : Appender(props)
    {
    }

    AndroidAppender::AndroidAppender(const AndroidAppender& other)
    {
    }

    AndroidAppender& AndroidAppender::operator=(const AndroidAppender& other)
    {
    }

    AndroidAppender::~AndroidAppender()
    {
        destructorImpl();
    }

    void AndroidAppender::close()
    {
        closed = true;
    }

    void AndroidAppender::append(const spi::InternalLoggingEvent& event)
    {
        tostringstream buf;

        layout->formatAndAppend(buf, event);

        const tstring& sz = buf.str();

        __android_log_print(translateLogLevel(event.getLogLevel()),
                            event.getLoggerName().c_str(),
                            "%s",
                            sz.c_str());
    }
}
