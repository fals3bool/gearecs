# ECS Registry

- **Entities** are just unique IDs that represent game objects
- **Components** are data containers (position, velocity, health, etc.)
- **Systems** are functions that operate on entities with specific components

## Using the registry

```C
typedef struct {
    int left;
    int right;
} UserInput;

typedef struct {
    float x;
    float y;
} Vector2;

typedef struct {
    Vector2 position;
    Vector2 scale;
    float rotation;
} Transform;

void MoveSystem(ECS *ecs, Entity e) {
    Transform *t = GetComponent(ecs, e, Transform);
    UserInput *input = GetComponent(ecs, e, UserInput);
    if(input->left)
        t->position.x--;
    if(input->right)
        t->position.x++;
}

void foo() {
    ECS *world = EcsRegistry(32);

    Component(world, UserInput);
    Component(world, Transform);
    System(world, MoveSystem, EcsOnUpdate, Transform, UserInput);

    Entity player = EcsEntity(world);
    AddComponent(ecs, player, UserInput, {0, 0});
    AddComponent(ecs, player, Transform, {{0, 0}, {0, 0}, 0});

    while(bar) {
        RunSystems(ecs, EcsOnUpdate);
    }
}
```
