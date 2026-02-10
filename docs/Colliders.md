# Colliders

Gearecs provides convex polygon colliders with SAT (Separating Axis Theorem) collision detection and response.

## Collider Types

There are two types of colliders:

- **Solid**: Block movement and generate collision responses. Used for walls, floors, obstacles.
- **Trigger**: Detect overlap without blocking. Used for area effects, pickups, detection zones.

## Creating Colliders

### Basic Creation

Create colliders with radius and vertex count. The radius is used for regular polygon generation and broad-phase culling optimization.

```C
// Solid hexagon collider
AddComponent(ecs, entity, Collider, ColliderSolid(30, 6)); // radius=30, vertices=6

// Trigger octagon collider  
AddComponent(ecs, entity, Collider, ColliderTrigger(25, 8)); // radius=25, vertices=8
```

### Rectangle Colliders

Create rectangular colliders (axis-aligned bounding boxes):

```C
// 20x30 solid rectangle, centered at entity position
int w = 30, h = 20;
Rectangle rect = {-w/2, -h/2, w, h}; // x, y, width, height
AddComponent(ecs, entity, Collider, ColliderRect(rect, true));

// Rectangle trigger
AddComponent(ecs, entity, Collider, ColliderRect(rect, false));
```

### Custom Polygon Colliders

Create colliders from custom vertex arrays. Polygons **must be convex**:

```C
// Custom triangle
Vector2 triangle[3] = {
    {-10, -10},  // top-left
    {10, -10},   // top-right  
    {0, 10}      // bottom-center
};
AddComponent(ecs, entity, Collider, ColliderVec(3, triangle, true)); // solid

// Custom convex polygon
Vector2 shape[8] = {
    {-20, -10}, {-10, -15}, {10, -15}, {20, -10},
    {20, 10}, {10, 15}, {-10, 15}, {-20, 10}
};
AddComponent(ecs, entity, Collider, ColliderVec(8, shape, false)); // trigger
```

## Collision Requirements

For solid colliders to interact physically, entities need both `Collider` and `RigidBody` components:


```C
// Solid physics object
Entity player = EcsEntity(ecs, "Player");
AddComponent(ecs, player, Transform2, TransformOrigin);
AddComponent(ecs, player, Collider, ColliderSolid(15, 4)); // square
AddComponent(ecs, player, RigidBody, RigidBodyDynamic(80, 0.98f));

// Static wall (no RigidBody needed for static geometry)
Entity wall = EcsEntity(ecs, "Wall");
AddComponent(ecs, wall, Transform2, TransformPos(100, 0));
AddComponent(ecs, wall, Collider, ColliderSolid(20, 4));
```

## Collision Events

Handle collisions with the `CollisionListener` component:

```C
void OnCollision(ECS *ecs, CollisionEvent *event) {
    printf("Entity %d collided with %d!\n", event->self, event->other);
    
    // Access collision details
    Vector2 normal = event->collision.normal;
    float penetration = event->collision.distance;
    
    // Apply damage, trigger effects, etc.
}

Entity player = EcsEntity(ecs, "Player");
AddComponent(ecs, player, CollisionListener, {OnCollision}); // handler only
```

## Collision Systems

GearECS provides built-in collision systems:
- `TransformColliderSystem` - Updates collider positions based on transforms
- `CollisionSystem` - Detects and resolves collisions  
- `DebugColliderSystem` - Renders collider debug information

See [Systems](Systems.md) for information about registering these systems.

## Tips

- Keep colliders as simple as possible
- Use rectangles for axis-aligned objects
- Use triggers for non-physical interactions
- Combine with layer filtering for performance
- Debug with `DebugColliderSystem` to verify collision shapes

Colliders don't need to be regular polygons. They just MUST be convex.


