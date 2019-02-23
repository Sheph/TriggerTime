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

#include "PlatformWin32.h"
#include "af/Utils.h"
#include "Logger.h"
#include <boost/algorithm/string/replace.hpp>
#include <fstream>
#include <windows.h>
#include <shlobj.h>

namespace af
{
    PlatformPtr platform(new PlatformWin32());

    PlatformWin32::PlatformWin32()
    {
    }

    PlatformWin32::~PlatformWin32()
    {
    }

    bool PlatformWin32::init(const std::string& assetsPath)
    {
        initTimeUs();
        assetsPath_ = assetsPath;
        return true;
    }

    void PlatformWin32::shutdown()
    {
    }

    std::streambuf* PlatformWin32::openFile(const std::string& fileName)
    {
        std::filebuf* buf = new std::filebuf();

        if (!buf->open((assetsPath_ + "/" + fileName).c_str(), std::ios::in | std::ios::binary)) {
            delete buf;
            buf = NULL;
        }

        return buf;
    }

    std::string PlatformWin32::readUserConfig() const
    {
        return readUserFile("user.ini");
    }

    bool PlatformWin32::writeUserConfig(const std::string& value)
    {
        std::string tmp = value;

        boost::replace_all(tmp, "\n", "\r\n");

        return writeUserFile("user.ini", tmp);
    }

    std::string PlatformWin32::readUserSaveData() const
    {
        return readUserFile("savedata");
    }

    bool PlatformWin32::writeUserSaveData(const std::string& value)
    {
        return writeUserFile("savedata", value);
    }

    std::string PlatformWin32::readUserFile(const std::string& fileName)
    {
        wchar_t buffer[MAX_PATH];

        if (!SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, buffer))) {
            LOG4CPLUS_WARN(logger(), "$AppData not defined, not reading " << fileName);
            return "";
        }

        std::wstring fileNameW(fileName.begin(), fileName.end());

        std::wstring fp = std::wstring(buffer) + L"\\TriggerTime\\" + fileNameW;

        std::ifstream is(fp.c_str());

        if (!is) {
            LOG4CPLUS_WARN(logger(), "$AppData\\TriggerTime\\" << fileName << " not found");
            return "";
        }

        std::string str;

        str.assign(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());

        if (is.fail()) {
            LOG4CPLUS_WARN(logger(), "Error reading $AppData\\TriggerTime\\" << fileName);
            return "";
        }

        return str;
    }

    bool PlatformWin32::writeUserFile(const std::string& fileName, const std::string& value)
    {
        wchar_t buffer[MAX_PATH];

        if (!SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, buffer))) {
            LOG4CPLUS_WARN(logger(), "$AppData not defined, not writing " << fileName);
            return false;
        }

        std::wstring fileNameW(fileName.begin(), fileName.end());

        std::wstring d = std::wstring(buffer) + L"\\TriggerTime";

        if (_wmkdir(d.c_str()) != 0 && errno != EEXIST) {
            LOG4CPLUS_WARN(logger(), "Cannot create $AppData\\TriggerTime, not writing " << fileName);
            return false;
        }

        std::wstring fp = d + L"/" + fileNameW;

        try {
            std::ofstream os(fp.c_str(),
                std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);

            if (!os) {
                throw std::runtime_error("");
            }

            os.exceptions(std::ios::failbit | std::ios::badbit);

            os.write(value.c_str(), value.size());

            os.close();
        } catch (...) {
            LOG4CPLUS_WARN(logger(), "Error writing $AppData\\TriggerTime\\" << fileName);

            return false;
        }

        return true;
    }
}
