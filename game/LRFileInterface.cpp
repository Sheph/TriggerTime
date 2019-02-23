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

#include "LRFileInterface.h"
#include "Platform.h"

namespace af
{
    LRFileInterface::LRFileInterface()
    {
    }

    LRFileInterface::~LRFileInterface()
    {
    }

    Rocket::Core::FileHandle LRFileInterface::Open(const Rocket::Core::String& path)
    {
        PlatformIFStream* is = new PlatformIFStream(path.CString());

        if (!*is) {
            delete is;
            return static_cast<Rocket::Core::FileHandle>(NULL);
        }

        return reinterpret_cast<Rocket::Core::FileHandle>(is);
    }

    void LRFileInterface::Close(Rocket::Core::FileHandle file)
    {
        PlatformIFStream* is = reinterpret_cast<PlatformIFStream*>(file);
        delete is;
    }

    size_t LRFileInterface::Read(void* buffer, size_t size, Rocket::Core::FileHandle file)
    {
        PlatformIFStream* is = reinterpret_cast<PlatformIFStream*>(file);

        if (!is->read(reinterpret_cast<char*>(buffer), size)) {
            return is->gcount();
        }

        return is->gcount();
    }

    bool LRFileInterface::Seek(Rocket::Core::FileHandle file, long offset, int origin)
    {
        PlatformIFStream* is = reinterpret_cast<PlatformIFStream*>(file);

        is->clear();

        if (origin == SEEK_SET) {
            is->seekg(offset, std::ios_base::beg);
        } else if (origin == SEEK_CUR) {
            is->seekg(offset, std::ios_base::cur);
        } else if (origin == SEEK_END) {
            is->seekg(offset, std::ios_base::end);
        }

        return 0;
    }

    size_t LRFileInterface::Tell(Rocket::Core::FileHandle file)
    {
        PlatformIFStream* is = reinterpret_cast<PlatformIFStream*>(file);

        return is->tellg();
    }
}
