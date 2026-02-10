# Systems

Systems are functions that process entities with specific components

## System Execution Phases

gearecs organizes systems into distinct execution phases that run in a specific order each frame.

1. `EcsOnStart` - Initialization
2. `EcsOnUpdate` - Frame-dependent game logic
3. `EcsOnLateUpdate` - Post-processing
4. `EcsOnFixedUpdate` - Physics simulation (fixed timestep)
5. `EcsOnRender` - Drawing operations
6. `EcsOnGui` - UI overlay

## Creating Systems

```C
void SystemName(ECS *ecs, Entity entity);
```

- `ecs`: Pointer to the ECS registry/world
- `entity`: The current entity being processed

Example System

```C
// Movement system that processes entities with Position and Input components
void MovementSystem(ECS *ecs, Entity entity) {
    Position *pos = GetComponent(ecs, entity, Position);
    Input *input = GetComponent(ecs, entity, Input);
    if (input->left)
        pos->x -= GetFrameTime();
    if (input->right)
        pos->x += GetFrameTime();
    if (input->up)
        pos->y -= GetFrameTime();
    if (input->down)
        pos->y += GetFrameTime();
}
```

## Registering Systems

The `System()` macro registers a system to run in a specific phase and process entities with required components:

```C
System(ecs, MovementSystem, EcsOnUpdate, Position, Input);
```

## Running Systems

```C
EcsRunSystems(ecs, EcsOnStart);
while (!WindowShouldClose()) {
    EcsRunSystems(ecs, EcsOnUpdate);
    EcsRunSystems(ecs, EcsOnLateUpdate);
    
    BeginDrawing();
    EcsRunSystems(ecs, EcsOnRender);
    EndDrawing();
}
```

## Built-in Systems

gearecs provides several built-in systems that handle common game functionality:

- **HierarchyTransformSystem**: Updates child transforms based on parent transforms
- **TransformColliderSystem**: Synchronizes collider positions with transform positions
- **CollisionSystem**: Detects and resolves collisions between entities

- **GravitySystem**: Applies gravitational forces to rigid bodies
- **PhysicsSystem**: Handles velocity, forces, and integration for rigid bodies

- **SpriteSystem**: Renders sprite components
- **DebugColliderSystem**: Draws collider bounds and collision events (for debugging)


