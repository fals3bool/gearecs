#ifndef ECS_COMPONENT_H
#define ECS_COMPONENT_H

#include <ecs/registry.h>
#include <raylib.h>

#include <stdint.h>

typedef struct {
  uint8_t active;
  uint8_t visible;
} EntityData;
#define ENTITYDATA_DEFAULT {1, 1}

typedef struct {
  Vector2 position;
  Vector2 scale;
  float rotation;
} Transform2;
#define TRANSFORM_DEFAULT {{0, 0}, {1, 1}, 0}

typedef struct {
  Vector2 *vx;
  Vector2 *md;
  Vector2 origin;
  float rot;
  uint8_t vertices;
  uint8_t overlap;
  uint8_t solid;
} Collider;

Collider ecs_collider(int vertices, float radius);
Collider ecs_collider_solid(int vertices, float radius);
Collider ecs_collider_rot(int vertices, float radius, float rot);
Collider ecs_collider_solid_rot(int vertices, float radius, float rot);
Collider ecs_collider_offset(int vertices, float radius, float rot, Vector2 origin);
Collider ecs_collider_solid_offset(int vertices, float radius, float rot, Vector2 origin);

typedef struct {
  Texture tex;
  Rectangle src;
  Color tint;
} Sprite;

typedef struct {
  Script scripts[EcsSystemLayers];
  Script OnEnable;
  Script OnDisable;
  Script OnCollisionEnter;
} Behaviour;
#define BEHAVIOUR_DEFAULT {0}

void ecs_script(Registry *r, Entity e, Script s, EcsLayer ly);

#endif
