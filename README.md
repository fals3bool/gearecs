# GearECS

A lightweight Entity Component System (ECS) library for C game development, built on top of raylib. GearECS provides a modern, data-oriented architecture for creating efficient and scalable 2D games.

## Features

- **Entity Component System Architecture** - Flexible, data-driven design pattern
- **Built-in Components** - Transform2, RigidBody, Collider, Sprite, and more
- **Physics Simulation** - Collision detection, forces, gravity, and rigid body dynamics
- **Hierarchy System** - Parent-child relationships for complex object structures
- **Scripting Framework** - Entity behaviors across multiple execution layers
- **Scene Management** - Complete game loop with automatic setup
- **Multi-platform Support** - Desktop, Windows, and Web (via Emscripten)
- **Raylib Integration** - Seamless rendering and input handling

## Basic Example

```c
#include <gearecs.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450

void Move(ECS *ecs, Entity self) {
    RigidBody *rb = GetComponent(ecs, self, RigidBody);
    ApplyImpulse(rb, (Vector2){0, -9.8f});
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GearECS Example");
    Camera2D camera = {{SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f}, {0, 0}, 0, 1.0f};

    ECS *world = EcsWorld(32, camera);

    Entity player = GameObject(world, "Player");
    AddComponent(world, player, RigidBody, RigidBodyDynamic(90, 1.5f));
    AddComponent(world, player, Behaviour, {0});
    AddScript(world, player, Move, EcsOnFixedUpdate);

    GameLoop(world);

    EcsFree(world);
    return 0;
}
```




