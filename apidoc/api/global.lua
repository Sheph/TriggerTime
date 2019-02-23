--- Global definitions.
-- You can refer to these without a module qualifier, i.e.:
--
--    print(const.zOrderBackground);
--
-- or:
--
--    print(scene.gameWidth);
--
-- @module global

--- Script finalizer function.
-- If your script defines this function it'll be called when level is unloaded,
-- i.e. when next level is loaded, when user goes back to main menu, etc.
function finalizer()
end

--- Render z-order values.
-- @tfield int zOrderBackground Background, parallax, etc.
-- @tfield int zOrderBack Back plan, i.e. shrooms, etc.
-- @tfield int zOrderMain Main plan, i.e. player, enemies, etc.
-- @tfield int zOrderExplosion Explosions, etc.
-- @tfield int zOrderTerrain Front terrain.
-- @tfield int zOrderFront Front plan, i.e. grass, etc.
-- @usage -- Create a parallax
-- local obj = factory:createBackground("b.png", 20, 10, vec2(0.2, 0.2), const.zOrderBackground);
-- @table const

--- @{SceneObject} type constants.
-- @tfield int SceneObjectTypeOther Something else
-- @tfield int SceneObjectTypePlayer Player
-- @tfield int SceneObjectTypeEnemy Enemy
-- @tfield int SceneObjectTypePlayerMissile Missile emitted by player
-- @tfield int SceneObjectTypeEnemyMissile Missile emitted by enemy
-- @tfield int SceneObjectTypeTerrain Terrain
-- @tfield int SceneObjectTypeRock Rock
-- @tfield int SceneObjectTypeBlocker Blocker
-- @tfield int SceneObjectTypeAlly Ally
-- @usage -- Remove an object if it's player's enemy
-- if obj.type == const.SceneObjectTypeEnemy then
--     obj:removeFromParent();
-- end;
-- @table const

--- Types of easings.
-- See [http://easings.net](http://easings.net) for more details.
-- @tfield int EaseLinear
-- @tfield int EaseInQuad
-- @tfield int EaseOutQuad
-- @tfield int EaseInOutQuad
-- @usage -- Create a single tweening that'll linearly interpolate
-- -- between 0 and 10 during 3 seconds
-- local t = SingleTweening(3, const.EaseLinear, 0, 10, false);
-- @table const

--- Scene that's being run right now.
-- @tfield Scene scene

--- @{SceneObject} factory singleton.
-- @tfield SceneObjectFactory factory

--- Game settings singleton.
-- @tfield Settings settings

--- Audio manager singleton.
-- @tfield AudioManager audio
