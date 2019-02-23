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

#ifndef _PLATFORMWIN32_H_
#define _PLATFORMWIN32_H_

#include "Platform.h"

namespace af
{
    class PlatformWin32 : public Platform
    {
    public:
        PlatformWin32();
        ~PlatformWin32();

        bool init(const std::string& assetsPath);

        void shutdown();

        virtual std::streambuf* openFile(const std::string& fileName);

        virtual bool changeVideoMode(bool fullscreen, int videoMode, int msaaMode, bool vsync, bool trilinearFilter);

        virtual std::string readUserConfig() const;
        virtual bool writeUserConfig(const std::string& value);

        virtual std::string readUserSaveData() const;
        virtual bool writeUserSaveData(const std::string& value);

    private:
        std::string assetsPath_;

        static std::string readUserFile(const std::string& fileName);

        static bool writeUserFile(const std::string& fileName, const std::string& value);
    };

#define platformWin32 dynamic_cast<af::PlatformWin32*>(af::platform.get())
}

#endif
