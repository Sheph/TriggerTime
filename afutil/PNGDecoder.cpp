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

#include "af/PNGDecoder.h"
#include "PNGUtils.h"
#include "Logger.h"
#include <string.h>
#include <errno.h>

namespace af
{
    class PNGDecoder::Impl
    {
    public:
        Impl(const std::string& path, std::istream& is)
        : path_(path),
          is_(is),
          pngPtr_(NULL),
          pngInfoPtr_(NULL),
          width_(0),
          height_(0)
        {
        }

        ~Impl()
        {
            if (pngPtr_) {
                PNGCatch(pngPtr_) {
                    pngPtr_ = NULL;
                    pngInfoPtr_ = NULL;

                    return;
                }

                ::png_destroy_read_struct(&pngPtr_, (pngInfoPtr_ ? &pngInfoPtr_ : NULL), NULL);

                pngPtr_ = NULL;
                pngInfoPtr_ = NULL;
            }
        }

        std::string path_;
        std::istream& is_;
        PNGError pngError_;
        png_structp pngPtr_;
        png_infop pngInfoPtr_;
        UInt32 width_;
        UInt32 height_;
        std::vector<png_bytep> rowPointers_;
    };

    PNGDecoder::PNGDecoder(const std::string& path, std::istream& is)
    : impl_(new Impl(path, is))
    {
    }

    PNGDecoder::~PNGDecoder()
    {
        delete impl_;
    }

    bool PNGDecoder::init(bool debug)
    {
        if (debug) {
            LOG4CPLUS_TRACE(afutil::logger(), "Opening " << impl_->path_ << "...");
        }

        if (!impl_->is_) {
            LOG4CPLUS_ERROR(afutil::logger(), "Cannot open " << impl_->path_);

            return false;
        }

        impl_->pngPtr_ = PNGCreateReadStruct(impl_->pngError_);

        if (!impl_->pngPtr_) {
            LOG4CPLUS_ERROR(afutil::logger(), "Cannot create PNG read struct");

            return false;
        }

        PNGCatch(impl_->pngPtr_) {
            LOG4CPLUS_ERROR(afutil::logger(), "Error reading PNG file");

            return false;
        }

        impl_->pngInfoPtr_ = ::png_create_info_struct(impl_->pngPtr_);

        if (!impl_->pngInfoPtr_) {
            LOG4CPLUS_ERROR(afutil::logger(), "Cannot create PNG info struct");

            return false;
        }

        PNGSetSource(impl_->pngPtr_, impl_->is_);

        ::png_read_info(impl_->pngPtr_, impl_->pngInfoPtr_);

        int bitDepth, colorType;

        ::png_get_IHDR(impl_->pngPtr_, impl_->pngInfoPtr_,
                       &impl_->width_, &impl_->height_,
                       &bitDepth, &colorType, NULL, NULL, NULL);

        if ((impl_->width_ == 0) ||
            (impl_->height_ == 0) ||
            (colorType != PNG_COLOR_TYPE_RGB_ALPHA) ||
            (bitDepth != 8)) {
            LOG4CPLUS_ERROR(afutil::logger(), "Bad PNG file");

            return false;
        }

        ::png_read_update_info(impl_->pngPtr_, impl_->pngInfoPtr_);

        if (::png_get_rowbytes(impl_->pngPtr_, impl_->pngInfoPtr_) != (impl_->width_ * 4)) {
            LOG4CPLUS_ERROR(afutil::logger(), "Bad PNG file");

            return false;
        }

        return true;
    }

    bool PNGDecoder::decode(std::vector<Byte>& data)
    {
        assert(impl_->pngPtr_);

        if (!impl_->pngPtr_) {
            LOG4CPLUS_ERROR(afutil::logger(), "PNG decoder not initialized");

            return false;
        }

        PNGCatch(impl_->pngPtr_) {
            LOG4CPLUS_ERROR(afutil::logger(), "Error reading PNG file");

            return false;
        }

        png_size_t rowBytes = ::png_get_rowbytes(impl_->pngPtr_, impl_->pngInfoPtr_);

        data.resize(rowBytes * impl_->height_);

        impl_->rowPointers_.resize(impl_->height_);

        for (UInt32 i = 0; i < impl_->height_; ++i)
        {
            impl_->rowPointers_[impl_->height_ - 1 - i] = &data[0] + i * rowBytes;
        }

        ::png_read_image(impl_->pngPtr_, &impl_->rowPointers_[0]);

        return true;
    }

    UInt32 PNGDecoder::width() const
    {
        return impl_->width_;
    }

    UInt32 PNGDecoder::height() const
    {
        return impl_->height_;
    }
}
