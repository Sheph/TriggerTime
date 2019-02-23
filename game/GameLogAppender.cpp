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

#include "GameLogAppender.h"
#include <log4cplus/streams.h>
#include <log4cplus/spi/loggingevent.h>
#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#else
#include <sys/stat.h>
#include <errno.h>
#endif


namespace log4cplus
{
    GameLogAppender::GameLogAppender()
    : buffer_(NULL)
    {
        init("log.txt");
    }

    GameLogAppender::GameLogAppender(const helpers::Properties& props)
    : Appender(props),
      buffer_(NULL)
    {
        init("log.txt");
    }

    GameLogAppender::GameLogAppender(const GameLogAppender& other)
    {
    }

    GameLogAppender& GameLogAppender::operator=(const GameLogAppender& other)
    {
        return *this;
    }

    GameLogAppender::~GameLogAppender()
    {
        destructorImpl();
    }

    void GameLogAppender::close()
    {
        closed = true;
        os_.close();
        delete [] buffer_;
        buffer_ = NULL;
    }

    void GameLogAppender::append(const spi::InternalLoggingEvent& event)
    {
        if (!os_.good()) {
            return;
        }

        layout->formatAndAppend(os_, event);
    }

    void GameLogAppender::init(const std::string& fileName)
    {
        int bufferSize = 512;

        delete [] buffer_;
        buffer_ = new tchar[bufferSize];
        os_.rdbuf()->pubsetbuf(buffer_, bufferSize);

#ifdef _WIN32
        wchar_t buffer[MAX_PATH];

        if (!SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, buffer))) {
            getErrorHandler()->error(LOG4CPLUS_TEXT("$AppData not defined, not writing ") + fileName);
            return;
        }

        std::wstring fileNameW(fileName.begin(), fileName.end());

        std::wstring d = std::wstring(buffer) + L"\\TriggerTime";

        if (_wmkdir(d.c_str()) != 0 && errno != EEXIST) {
            getErrorHandler()->error(LOG4CPLUS_TEXT("Cannot create $AppData\\TriggerTime, not writing ") + fileName);
            return;
        }

        std::wstring fp = d + L"/" + fileNameW;

        os_.open(fp.c_str(), std::ios_base::out | std::ios_base::trunc);

        if (!os_.good()) {
            getErrorHandler()->error(LOG4CPLUS_TEXT("Unable to write $AppData\\TriggerTime\\") + fileName);
            return;
        }
#else
        const char* p = getenv("HOME");

        if (!p) {
            getErrorHandler()->error(LOG4CPLUS_TEXT("$HOME not defined, not writing ") + fileName);
            return;
        }

        std::string d = std::string(p) + "/.TriggerTime";

        if ((mkdir(d.c_str(), 0700) != 0) && (errno != EEXIST)) {
            getErrorHandler()->error(LOG4CPLUS_TEXT("Cannot create ") + d + LOG4CPLUS_TEXT(", not writing") + fileName);
            return;
        }

        std::string fp = d + "/" + fileName;

        mode_t oldMask = ::umask(~(S_IRUSR | S_IWUSR));

        os_.open(fp.c_str(), std::ios_base::out | std::ios_base::trunc);

        ::umask(oldMask);

        if (!os_.good()) {
            getErrorHandler()->error(LOG4CPLUS_TEXT("Unable to write ") + fp);
            return;
        }
#endif
    }
}
