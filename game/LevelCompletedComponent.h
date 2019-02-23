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

#ifndef _LEVELCOMPLETEDCOMPONENT_H_
#define _LEVELCOMPLETEDCOMPONENT_H_

#include "UIComponent.h"
#include "StainedGlassComponent.h"
#include "Image.h"
#include "TextArea.h"
#include "AudioManager.h"
#include <boost/enable_shared_from_this.hpp>

namespace af
{
    class LevelCompletedComponent : public boost::enable_shared_from_this<LevelCompletedComponent>,
                                    public UIComponent
    {
    public:
        LevelCompletedComponent(const std::string& thisMission,
            const std::string& nextMission,
            const std::string& scriptPath,
            const std::string& assetPath,
            int zOrder = 0);
        ~LevelCompletedComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        virtual void update(float dt);

        virtual void render();

    private:
        struct Entry
        {
            TextAreaPtr key;
            TextAreaPtr value;
        };

        typedef std::vector<Entry> Entries;

        virtual void onRegister();

        virtual void onUnregister();

        void addEntry(const std::string& key, const std::string& value);

        void renderQuad(const Image& image, const b2Vec2& pos,
                        float width, float height);

        void onPress(int i);

        Image logo_;
        Entries entries_;

        StainedGlassComponentPtr stainedGlass_;

        float itemTime_;
        size_t numEntries_;
        bool blackoutDone_;
        bool displayButtons_;

        int currentItem_;
        TweeningPtr tweening_;
        float tweenTime_;
        float selectionTimeout_;

        int finger_;
        int fingerItem_;

        Image button_[2];
        b2Vec2 buttonPos_[2];
        float buttonWidth_[2];
        float buttonHeight_[2];

        std::string scriptPath_;
        std::string assetPath_;

        AudioSourcePtr strm_;
        AudioSourcePtr sndHit_;
    };

    typedef boost::shared_ptr<LevelCompletedComponent> LevelCompletedComponentPtr;
}

#endif
