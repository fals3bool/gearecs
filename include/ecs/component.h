#ifndef ECS_COMPONENT_H
#define ECS_COMPONENT_H

#include <ecs/registry.h>
#include <raylib.h>

#include <math.h>
#include <stdint.h>

#define VEC2ZERO (Vector2){0, 0}

typedef struct {
  uint8_t active;
  uint8_t visible;
} EntityData;
#define ENTITYDATA_ACTIVE {1, 1}

void entity_enable(Registry *r, Entity e, uint8_t active);
void entity_visible(Registry *r, Entity e, uint8_t visible);

typedef struct {
  Vector2 position;
  Vector2 scale;
  float rotation;
} Transform2;
#define TRANSFORM_ZERO {{0, 0}, {1, 1}, 0}
#define TRANSFORM_POS(x, y) {{x, y}, {1, 1}, 0}

typedef struct {
  Vector2 *vx;
  Vector2 *md;
  Vector2 origin;
  float rot;
  uint8_t vertices;
  uint8_t overlap;
  uint8_t solid;
} Collider;

#define collider_trigger(v, r) collider_create(v, r, 0, VEC2ZERO, 0)
#define collider_solid(v, r) collider_create(v, r, 0, VEC2ZERO, 1)

#define collider_trigger_angle(v, r, a) collider_create(v, r, a, VEC2ZERO, 0)
#define collider_solid_angle(v, r, a) collider_create(v, r, a, VEC2ZERO, 1)

#define collider_trigger_offset(v, r, a, o) collider_create(v, r, a, o, 0)
#define collider_solid_offset(v, r, a, o) collider_create(v, r, a, o, 1)

Collider collider_create(int vertices, float radius, float rot, Vector2 origin,
                         uint8_t solid);

typedef struct {
  float mass;
  float damping;
  uint8_t is_static;
  Vector2 speed;
  Vector2 acc;
} RigidBody;
#define RIGIDBODY_STATIC(m, d) {(m > 0) ? m : INFINITY, d, 1, {0, 0}, {0, 0}}
#define RIGIDBODY_DYNAMIC(m, d) {(m > 0) ? m : INFINITY, d, 0, {0, 0}, {0, 0}}

void rb_apply_force(RigidBody *rb, Vector2 force);
void rb_apply_impulse(RigidBody *rb, Vector2 impulse);
void rb_apply_damping(RigidBody *rb);

typedef struct {
  Texture tex;
  Rectangle src;
  Color tint;
} Sprite;

typedef struct {
  Script OnEnable;
  Script OnDisable;
  Script OnCollisionEnter;
  Script scripts[EcsSystemLayers];
} Behaviour;
#define BEHAVIOUR_EMPTY {0}

void ecs_script(Registry *r, Entity e, Script s, EcsLayer ly);

#endif
