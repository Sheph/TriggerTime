--- Object component class.
-- See @{02-object-model.md|Object and component model} for more details.
--
-- @classmod Component

--- Remove this component from parent object.
-- Same as
--    self.parent:removeComponent(self);
-- @see SceneObject:removeComponent
-- @see Component:onUnregister
function Component:removeFromParent()
end

--- Called when component is registered with a particular @{Scene}. i.e.:
--
-- * If a component is added to some @{SceneObject} by using
-- @{SceneObject:addComponent} and that object is NOT yet added to the
-- @{Scene} then this method is NOT called
-- * If a component is added to some @{SceneObject} by using
-- @{SceneObject:addComponent} and that object was already added to the @{Scene}
-- then this method is called immediately
-- * When @{SceneObject} is being added to the @{Scene} by using
-- @{Scene:addObject} then this method is called for all of its components
function Component:onRegister()
end

--- Called when component is unregistered from a @{Scene}. i.e.:
--
-- * If parent @{SceneObject} was added to @{Scene} and
-- @{SceneObject:removeComponent} is called then this
-- method is called immediately
-- * If parent @{SceneObject} was NOT added to @{Scene} and
-- @{SceneObject:removeComponent} is called then this
-- method is NOT called
-- * If parent @{SceneObject} is being removed from the @{Scene} by using
-- @{Scene:removeObject} then this method is called for all of its components
--
-- A component that was registered and then unregistered must not be registered
-- again, i.e. it's illegal to call @{SceneObject:addComponent} for it again.
function Component:onUnregister()
end

--- Parent object.
-- @tfield ?SceneObject parent
