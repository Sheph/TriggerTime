## Object and component model

### What's a scene

In AirForce what you see on the screen is a scene.

* Main menu is a scene. Everything you see, i.e. the background, menu items, etc. are parts of the scene
* Game level is different scene. Again, everything you see - the background, player, healthbar - everything is part of the scene
* Scenes are changed on demand, i.e. when user chooses a level to play, another scene - the level scene starts loading and is eventually displayed

Scene is represented by `Scene` class in lua.

### Scene objects

Scenes contain objects. An object by itself is just an abstract dummy, it has properties such as
position, linear velocity, angular velocity, etc. but it doesn't have a shape or size, it doesn't have
graphics or animation. The fact that the scene contains an object doesn't mean it's visible
to the player.

Scene objects are represented by `SceneObject` class in lua.

### Scene object components

Each object can contain one or more components. Component is sort of object behavior, i.e. each component describes a separate "feature".
e.g. an object visible to player will have `RenderComponent`, an object that's able to physically interact with other objets will have
`PhysicsComponent`, etc.

Object components are represented by `Component` class in lua. `Component` is a
base class for components, other component classes derive from it.

### Component phases

When the game is played the scene is continuously updated. Each object and each component within an object is
updated. But update is not object-wise it's component-wise. This means
that objects are not updated one by one, this means that first we update all
of the components of one type (e.g. `PhysicsComponent`), then we update all of the components of another type (e.g. `RenderComponent`), etc.
These are called component phases, each component is assigned one of these phases:

* Phase 1. Physics - This phase consists of `PhysicsComponent` components only. Physics components are responsible for things such as
world interactions, etc. e.g. if an object has non-0 velocity, then "physics" component will update object position according to velocity.
* Phase 2. Collision - If a component is a collision component it's usually responsible for handling of
collisions. e.g. collision component may do something like "if the object that I belong to just collided with
a wall, then explode the object and remove it from the scene"
* Phase 3. Think - If a component is a think component it's usually responsible for object logic, e.g. AI. Think component
may do something like "see if the object that I belong to is far enough from some other object and start shooting at it"
* Phase 4. PreRender - If a component is a pre-render component it's usually responsible for setting up
things to render in next phase. One example of pre-render component is the "animation" component.
* Phase 5. Render - If a component is a render component it's usually responsible for visual representation
of the object that it belongs to.
* Phase 6. UI - If a component is an UI component it's usually responsible for handling UI elements, such as
menus, etc. e.g. healthbar is one example here.

### Object nesting

Objects can be nested. This means logical nesting, not physical. e.g. consider a spider character, it has a body, a head and
8 legs. You can design it so that the body is the object and all the rest of the objects are body's subobjects.

The reason for object nesting is manipulation convenience, suppose you want to rotate an entire spider, not just his body, you'll then
call `SceneObject:setAngleRecursive` on
body object to specify that you want to rotate this object recursively, i.e. together with all subobjects.

On the other hand, you can rotate body object alone, without touching anything else, in that case you should assign `SceneObject:angle`.
