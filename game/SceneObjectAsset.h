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

#ifndef _SCENEOBJECTASSET_H_
#define _SCENEOBJECTASSET_H_

#include "af/RUBEWorld.h"
#include "SceneObject.h"
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

namespace af
{
    class SceneObjectAsset : boost::noncopyable
    {
    public:
        explicit SceneObjectAsset(const RUBEWorldPtr& world);
        ~SceneObjectAsset();

        inline const RUBEWorldPtr& world() const { return world_; }

        inline const RUBEBodyPtr& rootBody() const { return rootBody_; }

        inline const SceneObjectPtr& root() const { return root_; }

        inline int numObjects() const { return objects_.size(); }
        inline bool haveObject(int i) const { return (i >= 0) && (i < numObjects()); }
        inline SceneObjectPtr object(int i) const { return objects_[i]->shared_from_this(); }

        SceneObjectPtr object(const std::string& bodyName) const;

    private:
        struct TentaclePart
        {
            TentaclePart()
            : obj(NULL)
            {
            }

            TentaclePart(SceneObject* obj, const RUBEPolygonFixturePtr& fixture)
            : obj(obj), fixture(fixture)
            {
            }

            TentaclePart(SceneObject* obj, const RUBECircleFixturePtr& fixture)
            : obj(obj), circleFixture(fixture)
            {
            }

            SceneObject* obj;
            RUBEPolygonFixturePtr fixture;
            RUBECircleFixturePtr circleFixture;
        };

        struct Tentacle
        {
            std::map<int, TentaclePart> parts;
            std::string image;
            int zOrder;
            Color color;
            float width1;
            float width2;
            bool flip;
            int numIterations;
            float tension;
            float step;
        };

        typedef std::vector<SceneObject*> Objects;
        typedef std::map<std::string, Tentacle> TentacleMap;

        void processLight(const std::string& type,
            const RUBEImagePtr& image, SceneObject* obj);

        void processTentacles();

        RUBEWorldPtr world_;
        RUBEBodyPtr rootBody_;
        SceneObjectPtr root_;
        Objects objects_;
        TentacleMap tentacles_;
    };

    typedef boost::shared_ptr<SceneObjectAsset> SceneObjectAssetPtr;
}

#endif
