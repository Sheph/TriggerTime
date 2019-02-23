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

#include "PNGUtils.h"

namespace af
{
    static void PNGErrorFunc(png_structp pngPtr, png_const_charp msg)
    {
        PNGError* error = reinterpret_cast<PNGError*>(png_get_error_ptr(pngPtr));
        ::longjmp(error->setjmp_buffer, 1);
    }

    static void PNGWarningFunc(png_structp pngPtr, png_const_charp msg)
    {
    }

    static void PNGReadFunc(png_structp pngPtr, png_bytep data, png_size_t length)
    {
        std::istream* stream = reinterpret_cast<std::istream*>(::png_get_io_ptr(pngPtr));

        if (!stream->read(reinterpret_cast<char*>(data), length)) {
            PNGThrow(pngPtr);
        }
    }

    png_structp PNGCreateReadStruct(PNGError& pngError)
    {
        return ::png_create_read_struct(PNG_LIBPNG_VER_STRING, &pngError, PNGErrorFunc, PNGWarningFunc);
    }

    void PNGThrow(png_structp pngPtr)
    {
        ::png_error(pngPtr, "");
    }

    void PNGSetSource(png_structp pngPtr, std::istream& is)
    {
        ::png_set_read_fn(pngPtr, &is, PNGReadFunc);
    }
}
