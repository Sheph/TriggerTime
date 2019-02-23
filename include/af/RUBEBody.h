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

#ifndef _AF_RUBEBODY_H_
#define _AF_RUBEBODY_H_

#include "af/RUBEFixture.h"
#include "af/RUBEImage.h"

namespace af
{
    class RUBEBody;
    typedef boost::shared_ptr<RUBEBody> RUBEBodyPtr;

    class RUBEBody : public RUBEBase
    {
    public:
        RUBEBody(const std::string& name, const b2BodyDef& bodyDef);
        ~RUBEBody();

        static RUBEBodyPtr fromJsonValue(const Json::Value& jsonValue);

        inline const b2BodyDef& bodyDef() const { return bodyDef_; }

        void addFixture(const RUBEFixturePtr& fixture);

        void addImage(const RUBEImagePtr& image);

        inline int numFixtures() const { return fixtures_.size(); }
        inline bool haveFixture(int i) const { return (i >= 0) && (i < numFixtures()); }
        inline const RUBEFixturePtr& fixture(int i) const { return fixtures_[i]; }

        template <class T>
        inline boost::shared_ptr<T> fixture(const std::string& fixtureName) const
        {
            for (int i = 0; i < numFixtures(); ++i) {
                if (fixtures_[i]->name() == fixtureName) {
                    const boost::shared_ptr<T>& tmp =
                        boost::dynamic_pointer_cast<T>(fixtures_[i]);
                    if (tmp) {
                        return tmp;
                    } else {
                        break;
                    }
                }
            }

            return boost::shared_ptr<T>();
        }

        b2AABB computeAABB(const b2Transform &t) const;

        inline int numImages() const { return images_.size(); }
        inline bool haveImage(int i) const { return (i >= 0) && (i < numImages()); }
        inline const RUBEImagePtr& image(int i) const { return images_[i]; }

        RUBEImagePtr image(const std::string& imageName) const;

        inline const std::string& path() const { return path_; }
        inline void setPath(const std::string& value) { path_ = value; }

    private:
        typedef std::vector<RUBEFixturePtr> Fixtures;
        typedef std::vector<RUBEImagePtr> Images;

        std::string path_;
        b2BodyDef bodyDef_;
        Fixtures fixtures_;
        Images images_;
    };
}

#endif
