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

#include "SceneObjectManager.h"
#include "SceneObject.h"
#include "Scene.h"

namespace af
{
    static const int SceneObjectTypeIntValues[] =
    {
        SceneObjectTypeOther,
        SceneObjectTypePlayer,
        SceneObjectTypeEnemy,
        SceneObjectTypePlayerMissile,
        SceneObjectTypeEnemyMissile,
        SceneObjectTypeTerrain,
        SceneObjectTypeRock,
        SceneObjectTypeBlocker,
        SceneObjectTypeAlly,
        SceneObjectTypeAllyMissile,
        SceneObjectTypeEnemyBuilding,
        SceneObjectTypeGizmo,
        SceneObjectTypeNeutralMissile,
        SceneObjectTypeGarbage,
        SceneObjectTypeVehicle,
        SceneObjectTypeDeadbody,
        SceneObjectTypeShield,
    };

    static const char* SceneObjectTypeStrValues[] =
    {
        "other",
        "player",
        "enemy",
        "player_missile",
        "enemy_missile",
        "terrain",
        "rock",
        "blocker",
        "ally",
        "ally_missile",
        "enemy_building",
        "gizmo",
        "neutral_missile",
        "garbage",
        "vehicle",
        "deadbody",
        "shield"
    };

    AF_ENUMTRAITS_IMPL(SceneObjectType, SceneObjectType);

    SceneObjectManager::SceneObjectManager()
    : parent_(NULL),
      scene_(NULL)
    {
    }

    SceneObjectManager::~SceneObjectManager()
    {
    }

    void SceneObjectManager::addObject(const SceneObjectPtr& obj)
    {
        assert(!obj->parent());

        objects_.insert(obj);
        obj->setParent(this);

        if (scene()) {
            registerObject(obj);
        }
    }

    void SceneObjectManager::removeObject(const SceneObjectPtr& obj)
    {
        /*
         * Hold on to this object while
         * removing.
         */
        SceneObjectPtr tmp = obj;

        if (objects_.erase(tmp)) {
            if (scene()) {
                unregisterObject(tmp);
            }

            tmp->setParent(NULL);
        }
    }

    void SceneObjectManager::removeAllObjects()
    {
        std::set<SceneObjectPtr> tmp = objects_;

        for (std::set<SceneObjectPtr>::iterator it = tmp.begin();
             it != tmp.end();
             ++it ) {
            removeObject(*it);
        }
    }

    void SceneObjectManager::addObjectUnparent(const SceneObjectPtr& obj)
    {
        std::set<SceneObjectPtr> tmp = obj->objects();

        obj->removeAllObjects();

        addObject(obj);

        for (std::set<SceneObjectPtr>::iterator it = tmp.begin();
             it != tmp.end();
             ++it ) {
            addObject(*it);
        }
    }

    void SceneObjectManager::reparent(const SceneObjectPtr& obj)
    {
        if (!obj->parent() || (obj->parent() == this)) {
            return;
        }

        SceneObjectPtr tmp = obj;

        obj->parent()->objects_.erase(tmp);

        objects_.insert(tmp);

        obj->setParent(this);
    }

    std::vector<SceneObjectPtr> SceneObjectManager::getObjects(const std::string& name) const
    {
        std::vector<SceneObjectPtr> res;

        for (std::set<SceneObjectPtr>::const_iterator it = objects().begin();
             it != objects().end();
             ++it ) {
            if ((*it)->name() == name) {
                res.push_back(*it);
            }
        }

        return res;
    }

    std::vector<SceneObjectPtr> SceneObjectManager::getObjects() const
    {
        std::vector<SceneObjectPtr> res;

        for (std::set<SceneObjectPtr>::const_iterator it = objects().begin();
             it != objects().end();
             ++it ) {
            res.push_back(*it);
        }

        return res;
    }

    SceneObjectPtr SceneObjectManager::findObject(SceneObjectType type) const
    {
        for (std::set<SceneObjectPtr>::const_iterator it = objects_.begin();
             it != objects_.end();
             ++it ) {
            if ((*it)->type() == type) {
                return *it;
            }
        }
        return SceneObjectPtr();
    }

    void SceneObjectManager::registerObject(const SceneObjectPtr& obj)
    {
        obj->setScene(scene());

        std::set<SceneObjectPtr> tmpObjs = obj->objects_;
        std::vector<ComponentPtr> tmpComponents = obj->components();

        for (std::set<SceneObjectPtr>::iterator it = tmpObjs.begin();
             it != tmpObjs.end();
             ++it) {
            registerObject(*it);
        }

        for (std::vector<ComponentPtr>::const_iterator it = tmpComponents.begin();
             it != tmpComponents.end();
             ++it) {
            scene()->registerComponent(*it);
        }
    }

    void SceneObjectManager::unregisterObject(const SceneObjectPtr& obj)
    {
        std::set<SceneObjectPtr> tmpObjs = obj->objects_;
        std::vector<ComponentPtr> tmpComponents = obj->components();

        for (std::vector<ComponentPtr>::const_reverse_iterator it = tmpComponents.rbegin();
             it != tmpComponents.rend();
             ++it) {
            scene()->unregisterComponent(*it);
        }

        for (std::set<SceneObjectPtr>::iterator it = tmpObjs.begin();
             it != tmpObjs.end();
             ++it) {
            unregisterObject(*it);
        }

        obj->setScene(NULL);
    }
}
