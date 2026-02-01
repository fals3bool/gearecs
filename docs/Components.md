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
    // AddByRef allows complex initialization
    AddComponentByRef(ecs, player, PlayerStats, stats);

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
    AddComponent(ecs, e, Transform2, TransformZero);
    AddComponent(ecs, e, RigidBody, RigidBodyStatic);
    Collider col = ColliderSolid(5, 20);
    AddComponentByRef(ecs, e, Collider, col);

    EcsLoop(ecs);
    EcsFree(ecs);
}
```
