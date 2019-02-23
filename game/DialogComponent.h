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

#ifndef _DIALOGCOMPONENT_H_
#define _DIALOGCOMPONENT_H_

#include "UIComponent.h"
#include "Image.h"
#include "TextArea.h"
#include "Frame.h"

namespace af
{
    class DialogComponent : public UIComponent
    {
    public:
        DialogComponent(const b2Vec2& pos,
                        float width, float height);
        ~DialogComponent();

        virtual void update(float dt);

        virtual void render();

        void setTitlePlayer(const std::string& name);

        void setTitleAlly(const std::string& name,
                          const std::string& image);

        void setTitleEnemy(const std::string& name,
                           const std::string& image);

        void setMessage(const std::string& message);

        void endDialog();

        inline bool fastComplete() const { return fastComplete_; }
        inline void setFastComplete(bool value) { fastComplete_ = value; }

    private:
        void renderQuad(const Image& image, const b2Vec2& pos,
                        float width, float height);

        virtual void okPressed() = 0;

        b2Vec2 pos_;
        float width_;
        float height_;

        FramePtr frame_;

        Image portrait_;

        TextAreaPtr title_;
        TextAreaPtr message_;

        int letterCur_;
        float letterTime_;

        float blinkTime_;
        bool blinkState_;

        bool fastComplete_;
        bool okPressed_;
    };

    typedef boost::shared_ptr<DialogComponent> DialogComponentPtr;
}

#endif
