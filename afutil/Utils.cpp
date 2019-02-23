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

#include "Logger.h"
#include "af/Utils.h"
#include <boost/chrono.hpp>
#include <boost/thread/mutex.hpp>

namespace af
{
    boost::mutex objectPoolMutex;
    size_t objectPoolMaxBytes = 0;
    size_t objectPoolNumBytes = 0;
    static int objectPoolWarnThreshold = 0;
    static UInt64 objectPoolReportTimeoutMs = 1000;

    void objectPoolDeleted()
    {
        static UInt64 objectPoolTs = 0;
        static int objectPoolLastNumDeleted = 0;
        static int objectPoolNumDeleted = 0;

        ++objectPoolNumDeleted;
        UInt64 curTs = getTimeUs();
        if ((curTs - objectPoolTs) > objectPoolReportTimeoutMs * 1000) {
            if ((objectPoolNumDeleted - objectPoolLastNumDeleted) > objectPoolWarnThreshold) {
                LOG4CPLUS_TRACE(afutil::logger(), "pool deleted " << (objectPoolNumDeleted - objectPoolLastNumDeleted) <<
                    " objects for the past " << (curTs - objectPoolTs) / 1000 <<
                    "ms");
            }

            objectPoolTs = curTs;
            objectPoolLastNumDeleted = objectPoolNumDeleted;
        }
    }

    static boost::chrono::steady_clock::time_point startTime;

    void initTimeUs()
    {
        startTime = boost::chrono::steady_clock::now();
    }

    UInt64 getTimeUs()
    {
        boost::chrono::steady_clock::duration d =
           boost::chrono::steady_clock::now() - startTime;

        return boost::chrono::duration_cast<boost::chrono::microseconds>(d).count();
    }

    float getRandom(float minVal, float maxVal)
    {
        float r = static_cast<float>(rand() % RAND_MAX) / static_cast<float>(RAND_MAX);
        return minVal + (maxVal - minVal) * r;
    }

    Color getRandom(const Color& minVal, const Color& maxVal)
    {
        return Color(getRandom(minVal.rgba[0],maxVal.rgba[0]),
            getRandom(minVal.rgba[1], maxVal.rgba[1]),
            getRandom(minVal.rgba[2], maxVal.rgba[2]),
            getRandom(minVal.rgba[3], maxVal.rgba[3]));
    }

    bool readStream(std::istream& is, std::string& str)
    {
        str.assign(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
        return !is.fail();
    }

    void setObjectPoolMaxBytes(size_t value)
    {
        objectPoolMaxBytes = value;
    }

    void setObjectPoolWarnThreshold(int value)
    {
        objectPoolWarnThreshold = value;
    }

    void setObjectPoolReportTimeoutMs(UInt64 value)
    {
        objectPoolReportTimeoutMs = value;
    }

    static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";


    static inline bool is_base64(unsigned char c)
    {
        return (::isalnum(c) || (c == '+') || (c == '/'));
    }

    static std::string base64Encode(const UInt8* bytes_to_encode, unsigned int in_len)
    {
        std::string ret;
        int i = 0;
        int j = 0;
        unsigned char char_array_3[3];
        unsigned char char_array_4[4];

        while (in_len--) {
            char_array_3[i++] = *(bytes_to_encode++);
            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for (i = 0; (i <4) ; i++) {
                    ret += base64_chars[char_array_4[i]];
                }

                i = 0;
            }
        }

        if (i) {
            for(j = i; j < 3; j++) {
                char_array_3[j] = '\0';
            }

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (j = 0; (j < i + 1); j++) {
                ret += base64_chars[char_array_4[j]];
            }

            while ((i++ < 3)) {
                ret += '=';
            }
        }

        return ret;
    }

    std::string base64Encode(const std::string& value)
    {
        return base64Encode(reinterpret_cast<const UInt8*>(&value[0]), value.size());
    }

    std::string base64Decode(const std::string& encoded_string)
    {
        int in_len = encoded_string.size();
        int i = 0;
        int j = 0;
        int in_ = 0;
        unsigned char char_array_4[4], char_array_3[3];
        std::string ret;

        while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
            char_array_4[i++] = encoded_string[in_]; in_++;
            if (i == 4) {
                for (i = 0; i < 4; i++) {
                    char_array_4[i] = base64_chars.find(char_array_4[i]);
                }

                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                for (i = 0; (i < 3); i++) {
                    ret += char_array_3[i];
                }

                i = 0;
            }
        }

        if (i) {
            for (j = i; j <4; j++) {
                char_array_4[j] = 0;
            }

            for (j = 0; j <4; j++) {
                char_array_4[j] = base64_chars.find(char_array_4[j]);
            }

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
        }

        return ret;
    }

    #define _NXT 0x80
    #define _SEQ2 0xc0
    #define _SEQ3 0xe0
    #define _SEQ4 0xf0
    #define _SEQ5 0xf8
    #define _SEQ6 0xfc

    static int WCharForbidden(unsigned int sym)
    {
        // Surrogate pairs
        if (sym >= 0xd800 && sym <= 0xdfff) {
            return -1;
        }

        return 0;
    }

    static int UTF8Forbidden(unsigned char octet)
    {
        switch (octet) {
        case 0xc0:
        case 0xc1:
        case 0xf5:
        case 0xff:
            return -1;
        default:
            return 0;
        }
    }

    void UTF8toUCS2(const std::string& input, std::vector<UInt16>& output)
    {
        if (input.empty()) {
            return;
        }

        const unsigned char* p = reinterpret_cast<const unsigned char*>(input.c_str());
        const unsigned char* lim = p + input.length();

        // Skip the UTF-8 byte order marker if it exists.
        if (input.substr(0, 3) == "\xEF\xBB\xBF") {
            p += 3;
        }

        int numBytes = 0;
        for (; p < lim; p += numBytes) {
            if (UTF8Forbidden(*p) != 0) {
                LOG4CPLUS_WARN(afutil::logger(), "forbidden char " << static_cast<int>(*p) << " in UTF8 string");
                break;
            }

            // Get number of bytes for one wide character.
            UInt16 high;
            numBytes = 1;

            if ((*p & 0x80) == 0) {
                high = static_cast<UInt16>(*p);
            } else if ((*p & 0xe0) == _SEQ2) {
                numBytes = 2;
                high = static_cast<UInt16>(*p & 0x1f);
            } else if ((*p & 0xf0) == _SEQ3) {
                numBytes = 3;
                high = static_cast<UInt16>(*p & 0x0f);
            } else if ((*p & 0xf8) == _SEQ4) {
                numBytes = 4;
                high = static_cast<UInt16>(*p & 0x07);
            } else if ((*p & 0xfc) == _SEQ5) {
                numBytes = 5;
                high = static_cast<UInt16>(*p & 0x03);
            } else if ((*p & 0xfe) == _SEQ6) {
                numBytes = 6;
                high = static_cast<UInt16>(*p & 0x01);
            } else {
                break;
            }

            // Does the sequence header tell us the truth about length?
            if (lim - p <= numBytes - 1) {
                LOG4CPLUS_WARN(afutil::logger(), "bad UTF8 string");
                break;
            }

            // Validate the sequence. All symbols must have higher bits set to 10xxxxxx.
            if (numBytes > 1) {
                int i;
                for (i = 1; i < numBytes; i++) {
                    if ((p[i] & 0xc0) != _NXT) {
                        break;
                    }
                }

                if (i != numBytes) {
                    LOG4CPLUS_WARN(afutil::logger(), "bad UTF8 string");
                    break;
                }
            }

            // Make up a single UCS-4 (32-bit) character from the required number of UTF-8 tokens. The first byte has
            // been determined earlier, the second and subsequent bytes contribute the first six of their bits into the
            // final character code.
            unsigned int ucs4char = 0;
            int numBits = 0;
            for (int i = 1; i < numBytes; i++) {
                ucs4char |= static_cast<UInt16>(p[numBytes - i] & 0x3f) << numBits;
                numBits += 6;
            }
            ucs4char |= high << numBits;

            // Check for surrogate pairs.
            if (WCharForbidden(ucs4char) != 0) {
                LOG4CPLUS_WARN(afutil::logger(), "forbidden char " << ucs4char << " in UTF8 string");
                break;
            }

            // Only add the character to the output if it exists in the Basic Multilingual Plane (ie, fits in a single
            // word).
            if (ucs4char <= 0xffff) {
                output.push_back(static_cast<UInt16>(ucs4char));
            }
        }
    }
}
