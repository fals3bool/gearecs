# Components

Components are registered using the macro Component(), which creates an ID using its name.

Using that EcsID, components can be added and removed from an entity.

## Create Components

```C
typedef struct {
    float health;
    float speed;
    int level;
} PlayerStats;

void foo(ECS *ecs) {
    Entity player = EcsEntity(ecs);

    // Register component
    Component(ecs, PlayerStats);
    
    // Add component to player
    AddComponent(ecs, player, PlayerStats, {20.f, 9.f, 1});

    PlayerStats stats = {0};
    stats.health = 20;
    stats.speed = 5;
    stats.level = 2;

    // For complex initialization, use a temporary variable
    PlayerStats tempStats = {20, 9.0f, 2};
    AddComponent(ecs, player, PlayerStats, tempStats);

    // Retrieve component
    PlayerStats *ps = GetComponent(ecs, player, PlayerStats);
    ps->level++;

    // Remove component
    RemoveComponent(ecs, player, PlayerStats);
}
```

## Built-in Components

- **EntityData** - Tagging and state management
- **Transform2** - 2D position, scale, rotation (with hierarchy support)
- **RigidBody** - Physics simulation (mass, forces, damping)
- **Collider** - Polygon collision detection with layers
- **Sprite** - 2D texture rendering
- **Behaviour** - Entity scripting system
- **Parent / Children** - Hierarchy relationships

Using EcsWorld() there is no need to register any Component.

```C
Camera2D camera;

void foo() {
    ECS *ecs = EcsWorld(32, camera);
    Entity e = EcsEntity(ecs);
    AddComponent(ecs, e, Transform2, TransformOrigin);
    AddComponent(ecs, e, RigidBody, RigidBodyStatic);
    AddComponent(ecs, e, Collider, ColliderSolid(5, 20));

    EcsLoop(ecs);
    EcsFree(ecs);
}
```
