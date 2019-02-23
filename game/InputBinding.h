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

#ifndef _INPUTBINDING_H_
#define _INPUTBINDING_H_

#include "InputKeyboard.h"
#include "af/Types.h"
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace af
{
    class InputBinding;
    typedef boost::shared_ptr<InputBinding> InputBindingPtr;

    class InputBinding : boost::noncopyable
    {
    public:
        InputBinding();
        ~InputBinding();

        static bool isLongKey(KeyIdentifier ki);
        static const std::string& keyToString(KeyIdentifier ki);
        static const std::string& mbToString(bool left);

        static bool stringToKey(const std::string& str, KeyIdentifier& ki);
        static bool stringToMb(const std::string& str, bool& left);

        bool isKey() const;
        bool isLongKey() const;
        bool isMb(bool left) const;

        inline KeyIdentifier ki() const { return ki_; }

        const std::string& str() const;

        void setKey(KeyIdentifier value);
        void setMb(bool left);

        bool equal(const InputBindingPtr& other) const;

        void assign(const InputBindingPtr& other);

        void clear();

        bool empty() const;

        bool pressed() const;

        bool triggered() const;

    private:
        KeyIdentifier ki_;
        bool mb_[2];
    };

    class ActionBinding : boost::noncopyable
    {
    public:
        ActionBinding();
        ~ActionBinding();

        inline const InputBindingPtr& ib(int i) const { assert((i == 0) || (i == 1)); return ib_[i]; }

        bool pressed() const;

        bool triggered() const;

    private:
        InputBindingPtr ib_[2];
    };

    typedef boost::shared_ptr<ActionBinding> ActionBindingPtr;
}

#endif
