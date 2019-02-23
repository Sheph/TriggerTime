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

#ifndef _AF_RUBEWORLD_H_
#define _AF_RUBEWORLD_H_

#include "af/RUBEBody.h"
#include "af/RUBEJoint.h"
#include "af/RUBEImage.h"
#include "af/RUBEObject.h"
#include <json/json-forwards.h>
#include <iostream>

namespace af
{
    class RUBEWorld;
    typedef boost::shared_ptr<RUBEWorld> RUBEWorldPtr;

    class RUBEWorld : public RUBEBase
    {
    public:
        explicit RUBEWorld(const std::string& name);
        ~RUBEWorld();

        static RUBEWorldPtr fromStream(const std::string& fileName, std::istream& is);

        static RUBEWorldPtr fromString(const std::string& fileName, const std::string& json);

        static RUBEWorldPtr fromJsonValue(const std::string& fileName, const Json::Value& jsonValue);

        void addBody(const RUBEBodyPtr& body);

        void addJoint(const RUBEJointPtr& joint);

        void addImage(const RUBEImagePtr& image);

        void addObject(const RUBEObjectPtr& obj);

        inline int numBodies() const { return bodies_.size(); }
        inline bool haveBody(int i) const { return (i >= 0) && (i < numBodies()); }
        inline const RUBEBodyPtr& body(int i) const { return bodies_[i]; }

        RUBEBodyPtr body(const std::string& bodyName) const;

        inline int numJoints() const { return joints_.size(); }
        inline bool haveJoint(int i) const { return (i >= 0) && (i < numJoints()); }
        inline const RUBEJointPtr& joint(int i) const { return joints_[i]; }

        RUBEJointPtr joint(const std::string& jointName) const;

        inline int numImages() const { return images_.size(); }
        inline bool haveImage(int i) const { return (i >= 0) && (i < numImages()); }
        inline const RUBEImagePtr& image(int i) const { return images_[i]; }

        RUBEImagePtr image(const std::string& imageName) const;

        inline int numObjects() const { return objects_.size(); }
        inline bool haveObject(int i) const { return (i >= 0) && (i < numObjects()); }
        inline const RUBEObjectPtr& object(int i) const { return objects_[i]; }

        RUBEObjectPtr object(const std::string& objName) const;

    private:
        typedef std::vector<RUBEBodyPtr> Bodies;
        typedef std::vector<RUBEJointPtr> Joints;
        typedef std::vector<RUBEImagePtr> Images;
        typedef std::vector<RUBEObjectPtr> Objects;

        Bodies bodies_;
        Joints joints_;
        Images images_;
        Objects objects_;
    };
}

#endif
