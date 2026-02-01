#ifndef ECS_COMPONENT_H
#define ECS_COMPONENT_H

/**
 * @file component.h
 * @brief Built-in component definitions for gearecs ECS library
 *
 * This header provides a comprehensive set of ready-to-use components
 * for common game development needs. These components integrate seamlessly
 * with the ECS registry and include:
 *
 * - Entity metadata and tagging system
 * - 2D transforms with hierarchical support
 * - Physics components (RigidBody, Collider)
 * - Rendering components (Sprite)
 * - Scripting and behavior system
 * - Entity hierarchy management
 *
 * All components are designed to be used with the ECS registry macros
 * (Component, AddComponent, GetComponent, etc.) and include helpful
 * initialization macros and utility functions.
 */

#include <ecs/registry.h>

#include <raylib.h>
#include <raymath.h>

/**
 * Creates an empty component.
 *
 * Example: AddComponent(world, player, Behaviour, Empty)
 */
#define Empty {0}

// ############ //
//  ENTITY DATA //
// ############ //

/**
 * Component storing entity metadata and state information.
 *
 * Provides tagging capabilities and explicit control over entity
 * active/visible state. Useful for entity identification and state
 * management beyond the basic ECS entity state.
 *
 * @note Entities without this component are considered active and visible
 */
typedef struct {
  uint8_t active;  ///< Whether entity participates in Update systems
  uint8_t visible; ///< Whether entity participates in Render systems
  char *tag;       ///< String identifier for entity lookup
} EntityData;

/**
 * Creates an EntityData component with active and visible state.
 *
 * @param tag String tag for entity identification (can be NULL)
 * @return EntityData initializer
 *
 * Example: AddComponent(world, e, EntityData, EntityDataActive("Player"));
 */
#define EntityDataActive(tag) {1, 1, tag}

/**
 * Creates an EntityData component with active but hidden state.
 *
 * Entity will participate in Update systems but not Render systems.
 *
 * @param tag String tag for entity identification (can be NULL)
 * @return EntityData initializer
 *
 * Example: AddComponent(world, e, EntityData,
 * EntityDataHidden("TriggerBlock"));
 */
#define EntityDataHidden(tag) {1, 0, tag}

/**
 * Creates an EntityData component with inactive and hidden state.
 *
 * Useful to hide entities and use them later.
 *
 * @param tag String tag for entity identification (can be NULL)
 * @return EntityData initializer
 *
 * Example: AddComponent(world, e, EntityData,
 * EntityDataInactive("EnemyForLater"));
 */
#define EntityDataInactive(tag) {0, 0, tag}

/**
 * Finds the first entity with the specified tag.
 *
 * Performs linear search through all entities with EntityData components.
 * Returns an invalid entity ID if no matching entity is found.
 *
 * @param ecs Registry to search in
 * @param tag Tag string to search for
 * @return Entity with matching tag, or invalid id value if not found
 *
 * Example: if(FindByTag(ecs, "Player2") > EcsEntityCount())
 *
 *    printf("Not Found\n");
 */
Entity FindByTag(ECS *ecs, char *tag);

/**
 * Checks if an entity has a specific tag.
 *
 * @param ecs Registry containing the entity
 * @param e Entity to check
 * @param tag Tag string to compare
 * @return true if entity has matching tag, false otherwise
 */
uint8_t HasTag(ECS *ecs, Entity e, char *tag);

// ########### //
//  TRANSFORM  //
// ########### //

/**
 * 2D transform component with hierarchical support.
 *
 * Supports both world-space and local-space transformations for
 * entity hierarchies. World space represents the final position
 * after applying parent transforms, while local space represents
 * position relative to the parent.
 *
 * The hierarchy transform system automatically updates world space values
 * when parent transforms change.
 *
 * @see HierarchyTransformSystem()
 */
typedef struct {
  Vector2 position;      ///< World-space position
  Vector2 scale;         ///< World-space scale
  float rotation;        ///< World-space rotation (radians)
  Vector2 localPosition; ///< Position relative to parent
  Vector2 localScale;    ///< Scale relative to parent
  float localRotation;   ///< Rotation relative to parent (radians)
} Transform2;

/**
 * Creates a zero-initialized transform.
 *
 * Position at origin, unit scale, no rotation for both world and local space.
 *
 * @return Transform2 initializer
 *
 * Example: AddComponent(world, e, Transform2, TransformZero);
 */
#define TransformZero {{0, 0}, {1, 1}, 0, {0, 0}, {1, 1}, 0}

/**
 * Creates a transform with specific world position.
 *
 * Unit scale and no rotation. Useful for root entities or when
 * absolute positioning is needed.
 *
 * @param x X coordinate in world space
 * @param y Y coordinate in world space
 * @return Transform2 initializer
 *
 * Example: AddComponent(world, e, Transform2, TransformPos(100.0f, 200.0f));
 */
#define TransformPos(x, y) {{x, y}, {1, 1}, 0, {0, 0}, {1, 1}, 0}

/**
 * Creates a transform with specific local position.
 *
 * World position is zero, local position is set. Useful for child
 * entities that should be positioned relative to their parent.
 *
 * @param x X coordinate in local space
 * @param y Y coordinate in local space
 * @return Transform2 initializer
 *
 * Example: AddComponent(world, e, Transform2, TransformLocalPos(50.0f, 25.0f));
 */
#define TransformLocalPos(x, y) {{0, 0}, {1, 1}, 0, {x, y}, {1, 1}, 0}

// ########## //
//  COLLIDER  //
// ########## //

/**
 * 2D collision detection component supporting convex polygons.
 *
 * Supports both solid colliders (block movement) and trigger colliders
 * (detect overlap without blocking). Uses polygon-based collision with
 * configurable vertices and collision layers for selective interaction.
 */
typedef struct {
  Vector2 *vx;      ///< Array of polygon vertices (local space)
  Vector2 *md;      ///< Axis-aligned bounding box data (internal)
  uint8_t vertices; ///< Number of vertices in polygon
  uint8_t overlap;  ///< Collision overlap flag (internal)
  uint8_t solid;    ///< 1 for solid, 0 for trigger

  uint8_t layer;           ///< Collision layer (0-7)
  Signature collisionMask; ///< Bitmask of layers this collider can collide with
} Collider;

/**
 * Creates a trigger collider.
 *
 * Trigger colliders detect overlap but don't block movement. Useful for
 * area effects, pickups, and detection zones.
 *
 * @param v Number of vertices
 * @param r Bounding radius for broad-phase culling
 * @return Collider instance
 */
#define ColliderTrigger(v, r) ColliderCreate(v, r, 0)

/**
 * Creates a solid collider.
 *
 * Solid colliders block movement and generate collision responses.
 * Useful for walls, platforms, and physical obstacles.
 *
 * @param v Number of vertices
 * @param r Bounding radius for broad-phase culling
 * @return Collider instance
 */
#define ColliderSolid(v, r) ColliderCreate(v, r, 1)

/**
 * Creates a collider with specified parameters.
 *
 * Low-level function used by the Collider* macros. Creates polygon
 * collision shape with specified number of vertices and properties.
 *
 * @param vertices Number of vertices for polygon
 * @param radius Bounding radius for optimization
 * @param solid 1 for solid, 0 for trigger
 * @return Configured Collider instance
 */
Collider ColliderCreate(int vertices, float radius, uint8_t solid);

/**
 * Creates a collider from a list of vertices.
 *
 * @note be aware to not create a concave polygon. Gearecs only support convex
 * polygons.
 *
 * @param vertices Number of vertices for polygon
 * @param solid 1 for solid, 0 for trigger
 * @param vecs List of vertices
 * @return Configured Collider instance
 */
Collider ColliderVec(uint8_t vertices, uint8_t solid, Vector2 *vecs);

/**
 * Creates a collider from a rectangle.
 *
 * @note The rectangle is a bounding box which center is located at
 * entity.transform.position
 *
 * @param rect Rectangle bounding box
 * @param solid 1 for solid, 0 for trigger
 * @return Configured Collider instance
 */
Collider ColliderRect(Rectangle rect, uint8_t solid);

/**
 * Destructor for Collider component.
 *
 * Automatically frees vertex array memory when collider is removed
 * or entity is destroyed. Registered with ComponentDtor().
 *
 * @param self Pointer to Collider instance
 */
void ColliderDestructor(void *self);

/**
 * Sets the collision layer for a collider.
 *
 * Collision layers allow selective interaction between groups of
 * colliders (e.g., player enemies, projectiles, walls).
 *
 * @param c Collider to modify
 * @param layer Layer number (0-64)
 */
void ColliderSetLayer(Collider *c, uint8_t layer);

/**
 * Enables collision with a specific layer.
 *
 * Adds the specified layer to the collision mask. The collider will
 * check for collisions with colliders on this layer.
 *
 * If no layer is enabled, it will collide with everything.
 *
 * @param c Collider to modify
 * @param layer Layer number to enable (0-64)
 */
void ColliderEnableLayer(Collider *c, uint8_t layer);

/**
 * Disables collision with a specific layer.
 *
 * Removes the specified layer from the collision mask.
 *
 * @param c Collider to modify
 * @param layer Layer number to disable (0-64)
 */
void ColliderDisableLayer(Collider *c, uint8_t layer);

/**
 * Disables collision with all layers.
 *
 * Clears the collision mask, making the collider collide
 * with anything.
 *
 * @param c Collider to modify
 */
void ColliderDisableAllLayers(Collider *c);

/**
 * Checks if collision is enabled for a specific layer.
 *
 * @param c Collider to check
 * @param layer Layer number to check (0-64)
 * @return true if enabled, false if disabled
 */
uint8_t ColliderHasLayerEnabled(const Collider *c, uint8_t layer);

/**
 * Checks if two colliders can collide based on their layers.
 *
 * @param c1 First collider
 * @param c2 Second collider
 * @return true if colliders can interact, false otherwise
 */
uint8_t CanCollide(Collider *c1, Collider *c2);

/**
 * Collision data structure.
 *
 * Contains information about collision normal and penetration depth
 * for collision resolution and response.
 */
typedef struct {
  Vector2 normal; ///< Collision normal vector (direction of separation)
  float distance; ///< Penetration depth (positive = overlapping)
} Collision;

/**
 * Collision event data structure.
 *
 * Passed to collision handlers with information about
 * the colliding entities and collision details.
 */
typedef struct {
  Entity self;         ///< Entity receiving the collision event
  Entity other;        ///< Entity being collided with
  Collision collision; ///< Collision details
} CollisionEvent;

/**
 * Collision handler function type.
 *
 * Function called when collision occurs. Implement custom collision
 * response logic (damage, physics, state changes, etc.).
 *
 * @param ecs Registry containing the entities
 * @param event Collision event data
 */
typedef void (*CollisionHandler)(ECS *, CollisionEvent *);

/**
 * Component for receiving collision events.
 *
 * Add this component to entities that need to handle custom logic on
 * collisions.
 */
typedef struct {
  // CollisionHandler OnCollisionEnter;  ///< TODO: Collision start handler
  // CollisionHandler OnCollisionStay;   ///< TODO: Collision continue handler
  // CollisionHandler OnCollisionExit;   ///< TODO: Collision end handler
  CollisionHandler OnCollision; ///< Current collision handler
} CollisionListener;

// ############ //
//  RIGID BODY  //
// ############ //

/**
 * Rigid body physics simulation types.
 *
 * Determines how an entity participates in physics simulation:
 * - Static: Immovable, infinite mass objects (walls, floors)
 * - Dynamic: Full physics simulation, affected by forces (players, objects)
 * - Kinematic: Moved manually, affects dynamics but isn't affected
 */
typedef enum {
  BODY_STATIC = 0, ///< Immovable objects with infinite mass
  BODY_DYNAMIC,    ///< Full physics simulation
  BODY_KINEMATIC   ///< Manually controlled objects
} BodyType;

/**
 * 2D rigid body physics component.
 *
 * Provides realistic physics simulation including forces, impulses,
 * mass, damping, and gravity support. Integrates with collider
 * components for collision response.
 */
typedef struct {
  float mass;      ///< Object mass (g), 0 or INFINITY for static objects
  float invmass;   ///< Inverse mass (1/mass), 0 for static objects
  float damping;   ///< Velocity damping factor (0-1, 1 = no damping)
  BodyType type;   ///< Physics behavior type
  uint8_t gravity; ///< Whether gravity affects this body
  Vector2 speed;   ///< Current velocity (units/second)
  Vector2 acc;     ///< Current acceleration (units/second²)
} RigidBody;

/**
 * Creates a rigid body with specified parameters.
 *
 * Low-level macro used by the specific body type macros. Automatically
 * handles mass/inverse mass calculation and gravity setting based on type.
 *
 * @param mass Mass value (use 0 or INFINITY for static objects)
 * @param damping Damping factor (0-1)
 * @param type BodyType enum value
 * @return RigidBody initializer
 */
#define RigidBodyCreate(mass, damping, type)                                   \
  {(mass > 0) ? mass : INFINITY,                                               \
   (mass > 0) ? 1.f / mass : 0,                                                \
   damping,                                                                    \
   type,                                                                       \
   (type == BODY_DYNAMIC) ? 1 : 0,                                             \
   {0, 0},                                                                     \
   {0, 0}}

/**
 * Creates a static rigid body.
 *
 * Static bodies have infinite mass and don't move, but can collide
 * with dynamic bodies. Perfect for walls, floors, platforms.
 *
 * @return RigidBody initializer
 *
 * Example: AddComponent(world, e, RigidBody, RigidBodyStatic);
 */
#define RigidBodyStatic RigidBodyCreate(0, 0, BODY_STATIC)

/**
 * Creates a dynamic rigid body.
 *
 * Dynamic bodies participate fully in physics simulation and are
 * affected by forces, gravity, and collisions. Perfect for players,
 * enemies, moveable objects.
 *
 * @param mass Object mass
 * @param damping Velocity damping factor (0-1..)
 * @return RigidBody initializer
 *
 * Example: AddComponent(world, e, RigidBody, RigidBodyDynamic(80.0f, 0.98f));
 */
#define RigidBodyDynamic(mass, damping)                                        \
  RigidBodyCreate(mass, damping, BODY_DYNAMIC)

/**
 * Creates a kinematic rigid body.
 *
 * Kinematic bodies are moved manually (direct velocity control) and
 * affect dynamic bodies but aren't affected by forces or gravity.
 * Perfect for moving platforms, elevators, controlled objects.
 *
 * @param mass Object mass (typically 0)
 * @param damping Damping factor
 * @return RigidBody initializer
 *
 * Example: AddComponent(world, e, RigidBody, RigidBodyKinematic(0, 0.95f));
 */
#define RigidBodyKinematic(mass, damping)                                      \
  RigidBodyCreate(mass, damping, BODY_KINEMATIC)

/**
 * Applies a continuous force to a rigid body.
 *
 * Force is accumulated and applied during physics integration.
 * Use for constant forces like gravity, thrusters, springs.
 *
 * @param rb RigidBody to apply force to
 * @param force Force vector
 *
 * @note Force is accumulated, call before physics update
 */
void ApplyForce(RigidBody *rb, Vector2 force);

/**
 * Applies an instantaneous impulse to a rigid body.
 *
 * Impulse immediately changes velocity regardless of mass.
 * Use for sudden impacts, explosions, jumps.
 *
 * @param rb RigidBody to apply impulse to
 * @param impulse Impulse vector
 *
 * @note Impulse is applied immediately
 */
void ApplyImpulse(RigidBody *rb, Vector2 impulse);

/**
 * Applies velocity damping to a rigid body.
 *
 * Reduces velocity over time to simulate air resistance or friction.
 * Automatically called by the physics system.
 *
 * @param rb RigidBody to apply damping to
 *
 * @note Called internally by PhysicsSystem()
 */
void ApplyDamping(RigidBody *rb);

// ######## //
//  SPRITE  //
// ######## //

/**
 * 2D sprite rendering component.
 *
 * Handles texture rendering with source rectangle selection
 * and color tinting. Supports sprite sheets through src rectangle.
 * Integrates with raylib's rendering system.
 */
typedef struct {
  Texture tex;   ///< Raylib texture to render
  Rectangle src; ///< Source rectangle within texture (for sprite sheets)
  Color tint;    ///< Color tint for rendering
} Sprite;

// ########### //
//  BEHAVIOUR  //
// ########### //

/**
 * Component for entity scripting and behavior management.
 *
 * Allows entities to have custom scripts that run at different
 * phases of the game loop. Supports enable/disable events and
 * per-layer script execution.
 */
typedef struct {
  Script OnEnable;                ///< Script called when entity is enabled
  Script OnDisable;               ///< Script called when entity is disabled
  Script scripts[EcsTotalLayers]; ///< Scripts for each ecs layer
} Behaviour;

/**
 * Adds a script to an entity's Behaviour component.
 *
 * Automatically adds Behaviour component if entity doesn't have one.
 * The script will be called during the specified ecs layer.
 *
 * @param ecs Registry containing the entity
 * @param e Entity to add script to
 * @param s Script to add
 * @param ly System layer to run script in
 *
 * Example: AddScript(world, player, PlayerUpdate, EcsOnUpdate);
 */
void AddScript(ECS *ecs, Entity e, Script s, EcsLayer ly);

// ########### //
//  HIERARCHY  //
// ########### //

/**
 * Parent component for entity hierarchy.
 *
 * Links an entity to its parent in the hierarchy. Used with Transform2
 * to implement hierarchical transformations where child entities
 * inherit parent transformations.
 */
typedef struct {
  Entity entity; ///< Parent entity ID
} Parent;

/**
 * Children component for entity hierarchy.
 *
 * Maintains a list of child entities. Used internally by the hierarchy
 * system to manage parent-child relationships and enable operations
 * on all children of an entity.
 *
 * @see EntityForEachChild()
 * @see EntityForEachChildRecursive()
 */
typedef struct Children Children;

/**
 * Sets a parent for an entity in the hierarchy.
 *
 * Removes the entity from its current parent (if any) and adds it
 * as a child of the specified parent. Automatically updates hierarchy
 * components on both entities.
 *
 * @param ecs Registry containing the entities
 * @param e Child entity
 * @param p Parent entity
 *
 * @see EntityAddChild() for the inverse operation
 */
void EntityAddParent(ECS *ecs, Entity e, Entity p);

/**
 * Adds a child entity to another entity.
 *
 * Removes the child from its current parent (if any) and adds it as
 * a child of the specified parent.
 *
 * @param ecs Registry containing the entities
 * @param e Parent entity
 * @param c Child entity to add
 *
 * @see EntityAddParent() for the inverse operation
 */
void EntityAddChild(ECS *ecs, Entity e, Entity c);

/**
 * Removes a child from its parent.
 *
 * Removes the parent-child relationship between the specified entities.
 * The child becomes a root entity (no parent).
 *
 * @param ecs Registry containing the entities
 * @param e Parent entity
 * @param c Child entity to remove
 */
void EntityRemoveChild(ECS *ecs, Entity e, Entity c);

/**
 * Destroys an entity and removes it from hierarchy.
 *
 * Removes the entity from its parent (if any), but keeps children alive.
 * Children become root entities. Use EntityDestroyRecursive() to destroy
 * the entire hierarchy branch.
 *
 * @param ecs Registry containing the entity
 * @param e Entity to destroy
 *
 * @see EntityDestroyRecursive() to destroy with all descendants
 * @see EcsEntityFree() for basic entity destruction
 */
void EntityDestroy(ECS *ecs, Entity e);

/**
 * Destroys an entity and all its descendants.
 *
 * Recursively destroys the entity and all entities in its subtree.
 * Use this when you want to remove an entire hierarchy branch.
 *
 * @param ecs Registry containing the entity
 * @param e Entity to destroy
 *
 * @see EntityDestroy() to destroy without descendants
 * @see EcsEntityFree() for basic entity destruction
 */
void EntityDestroyRecursive(ECS *ecs, Entity e);

/**
 * Executes a script on all direct children of an entity.
 *
 * Iterates over all immediate children and calls the specified script.
 * Does not include grandchildren or deeper descendants.
 *
 * @param ecs Registry containing the entity
 * @param e Parent entity
 * @param s Script to execute on each child
 *
 * @see EntityForEachChildRecursive() for all descendants
 */
void EntityForEachChild(ECS *ecs, Entity e, Script s);

/**
 * Executes a script on all descendants of an entity.
 *
 * Recursively iterates over all descendants (children, grandchildren,
 * etc.) and calls the specified script on each one.
 *
 * @param ecs Registry containing the entity
 * @param e Root entity
 * @param s Script to execute on each descendant
 *
 * @see EntityForEachChild() for direct children only
 */
void EntityForEachChildRecursive(ECS *ecs, Entity e, Script s);

#endif
