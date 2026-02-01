# Systems

Systems run under different execution layers:

1. **OnStart** - Initialization (runs once)
2. **OnUpdate** - Frame-dependent game logic
3. **OnLateUpdate** - Post-processing
4. **OnFixedUpdate** - Physics (fixed timestep 60/s)
5. **OnRender** - Drawing operations
6. **OnGui** - UI overlay

Use the macro System() to register a system under a execution layer and a set of components
To run the list of systems there is a function EcsRunSystems()

## Create and run systems

```C
typedef struct {
    float x;
    float y;
} Position;

void MoveUpSystem(ECS *world, Entity e){
    Position *p = GetComponent(world, e, Position);
    p.y--;
}

void foo() {
    ECS *world = EcsRegistry();
    Component(world, Position);
    // Will only execute for entities that have a Position component
    System(world, MoveUpSystem, EcsOnUpdate, Position); 

    Entity entity = EcsEntity(world);
    AddComponent(world, entity, Position, {0, 200});
    
    EcsRunSystems(world, EcsOnUpdate); // Run all systems
}
```

## GearEcs default systems

```C
System(ecs, BehaviourStartSystem, EcsOnStart, Behaviour);
System(ecs, BehaviourUpdateSystem, EcsOnUpdate, Behaviour);
System(ecs, BehaviourLateSystem, EcsOnLateUpdate, Behaviour);
System(ecs, BehaviourFixedSystem, EcsOnFixedUpdate, Behaviour);
System(ecs, BehaviourRenderSystem, EcsOnRender, Behaviour);
System(ecs, BehaviourGuiSystem, EcsOnGui, Behaviour);

System(ecs, HierarchyTransform, EcsOnUpdate, Transform2, Children);
System(ecs, TransformColliderSystem, EcsOnUpdate, Transform2, Collider);
System(ecs, CollisionSystem, EcsOnUpdate, Transform2, Collider);

System(ecs, GravitySystem, EcsOnFixedUpdate, RigidBody);
System(ecs, PhysicsSystem, EcsOnFixedUpdate, RigidBody, Transform2);

System(ecs, SpriteSystem, EcsOnRender, Transform2, Sprite);

// debug, not registered by default
System(ecs, DebugColliderSystem, EcsOnRender, Transform2, Collider);
```
