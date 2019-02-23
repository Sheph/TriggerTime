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

#ifndef _SCENEOBJECTMANAGER_H_
#define _SCENEOBJECTMANAGER_H_

#include "af/Types.h"
#include "af/EnumSet.h"
#include "af/EnumTraits.h"
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <set>

namespace af
{
    class Scene;

    class SceneObject;
    typedef boost::shared_ptr<SceneObject> SceneObjectPtr;

    enum SceneObjectType
    {
        SceneObjectTypeOther = 0,
        SceneObjectTypePlayer = 1,
        SceneObjectTypeEnemy = 2,
        SceneObjectTypePlayerMissile = 3,
        SceneObjectTypeEnemyMissile = 4,
        SceneObjectTypeTerrain = 5,
        SceneObjectTypeRock = 6,
        SceneObjectTypeBlocker = 7,
        SceneObjectTypeAlly = 8,
        SceneObjectTypeAllyMissile = 9,
        SceneObjectTypeEnemyBuilding = 10,
        SceneObjectTypeGizmo = 11,
        SceneObjectTypeNeutralMissile = 12,
        SceneObjectTypeGarbage = 13,
        SceneObjectTypeVehicle = 14,
        SceneObjectTypeDeadbody = 15,
        SceneObjectTypeShield = 16,
    };

    static const int SceneObjectTypeMax = SceneObjectTypeShield;

    typedef EnumSet<SceneObjectType, SceneObjectTypeMax + 1> SceneObjectTypes;

    enum Material
    {
        MaterialOther = 0,
        MaterialFlesh = 1,
        MaterialBulletProof = 2
    };

    class SceneObjectManager : boost::noncopyable
    {
    public:
        SceneObjectManager();
        virtual ~SceneObjectManager();

        void addObject(const SceneObjectPtr& obj);

        void removeObject(const SceneObjectPtr& obj);

        void removeAllObjects();

        void addObjectUnparent(const SceneObjectPtr& obj);

        void reparent(const SceneObjectPtr& obj);

        inline const std::set<SceneObjectPtr>& objects() const { return objects_; }

        std::vector<SceneObjectPtr> getObjects(const std::string& name) const;

        std::vector<SceneObjectPtr> getObjects() const;

        SceneObjectPtr findObject(SceneObjectType type) const;

        inline SceneObjectManager* parent() { return parent_; }
        inline void setParent(SceneObjectManager* value) { parent_ = value; }

        inline Scene* scene() { return scene_; }
        inline void setScene(Scene* value) { scene_ = value; }

    private:
        void registerObject(const SceneObjectPtr& obj);

        void unregisterObject(const SceneObjectPtr& obj);

        SceneObjectManager* parent_;

        Scene* scene_;

        std::set<SceneObjectPtr> objects_;
    };

    AF_ENUMTRAITS(SceneObjectType);
}

#endif
