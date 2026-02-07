# GearECS

A lightweight Entity-Component-System (ECS) library for C game development, built on top of raylib.
GearECS provides a modern, data-oriented architecture for creating efficient and scalable 2D games.

## Features

- **ECS Architecture** - Flexible, data-driven design pattern
- **Built-in Components** - Transform2, RigidBody, Collider, Sprite, and more
- **Physics Simulation** - Collision detection, forces, gravity, and rigid body dynamics
- **Hierarchy Relationships** - Parent-child relationships for complex object structures
- **Entity Behaviour** - Entity scripting across multiple execution layers
- **Scene Management** - Automatic scene setup with camera and generic game loop
- **Multi-platform Support** - Linux, Windows, and Web (via Emscripten)
- **Raylib Integration** - Seamless rendering and input handling

## Basic Example

```c
#include <gearecs.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450
#define SPEED 900

void MoveScript(ECS *ecs, Entity self) {
  RigidBody *rb = GetComponent(ecs, self, RigidBody);
  Transform2 *t = GetComponent(ecs, self, Transform2);
  if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
    Vector2 d = {cosf(t->rotation), sinf(t->rotation)};
    ApplyImpulse(rb, Vector2Scale(d, SPEED));
  }

  Camera2D *cam = GetComponent(ecs, 0, Camera2D);

  Vector2 mu = GetScreenToWorld2D(GetMousePosition(), *cam);
  Vector2 md = {mu.x - t->position.x, mu.y - t->position.y};
  t->rotation = atan2f(md.y, md.x);
}

int main(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GearECS Example");
  Camera2D camera = {
      {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f}, {0, 0}, 0, 1.0f};

  ECS *world = EcsWorld(32, camera);

  Texture tex = LoadTexture("res/example.png");
  Entity player = EcsEntity(world, "Player");
  AddComponent(world, player, Sprite, {tex, {0, 0, 32, 32}, WHITE});
  AddComponent(world, player, RigidBody, RigidBodyKinematic(50, 1.5f));
  AddScript(world, player, MoveScript, EcsOnFixedUpdate);

  EcsLoop(world);
  EcsFree(world);
  UnloadTexture(tex);
  return 0;
}
```




