#ifndef ECS_COMPONENT_H
#define ECS_COMPONENT_H

#include <ecs/registry.h>
#include <raylib.h>

#include <math.h>
#include <stdint.h>

typedef struct {
  uint8_t active;
  uint8_t visible;
  char *tag;
} EntityData;
#define EntityDataActive(tag) {1, 1, tag}
#define EntityDataHidden(tag) {1, 0, tag}

Entity FindByTag(ECS *ecs, char *tag);
uint8_t HasTag(ECS *ecs, Entity e, char *tag);

typedef struct {
  Vector2 position;
  Vector2 scale;
  float rotation;
  Vector2 localPosition;
  Vector2 localScale;
  float localRotation;
} Transform2;
#define TransformZero {{0, 0}, {1, 1}, 0, {0, 0}, {1, 1}, 0}
#define TransformPos(x, y) {{x, y}, {1, 1}, 0, {0, 0}, {1, 1}, 0}
#define TransformLocalPos(x, y) {{0, 0}, {1, 1}, 0, {x, y}, {1, 1}, 0}

typedef struct {
  Vector2 *vx;
  Vector2 *md;
  uint8_t vertices;
  uint8_t overlap;
  uint8_t solid;

  uint8_t layer;
  Signature collisionMask;
} Collider;

#define ColliderTrigger(v, r) ColliderCreate(v, r, 0)
#define ColliderSolid(v, r) ColliderCreate(v, r, 1)
Collider ColliderCreate(int vertices, float radius, uint8_t solid);
void ColliderDestructor(void *self);

void ColliderSetLayer(Collider *c, uint8_t layer);
void ColliderEnableLayer(Collider *c, uint8_t layer);
void ColliderDisableLayer(Collider *c, uint8_t layer);
void ColliderDisableAllLayers(Collider *c);
uint8_t ColliderHasLayerEnabled(const Collider *c, uint8_t layer);
uint8_t CanCollide(Collider *c1, Collider *c2);

typedef struct {
  Vector2 normal;
  float distance;
} Collision;

typedef struct {
  Entity self, other;
  Collision collision;
} CollisionEvent;

typedef void (*CollisionHandler)(ECS *, CollisionEvent *);

typedef struct {
  // CollisionHandler OnCollisionEnter;
  // CollisionHandler OnCollisionStay;
  // CollisionHandler OnCollisionExit;
  CollisionHandler OnCollision;
} CollisionListener;

typedef enum { BODY_STATIC = 0, BODY_DYNAMIC, BODY_KINEMATIC } BodyType;

typedef struct {
  float mass;
  float invmass;
  float damping;
  BodyType type;
  uint8_t gravity;
  Vector2 speed;
  Vector2 acc;
} RigidBody;

#define RigidBodyCreate(mass, damping, type)                                   \
  {(mass > 0) ? mass : INFINITY,                                               \
   (mass > 0) ? 1.f / mass : 0,                                                \
   damping,                                                                    \
   type,                                                                       \
   (type == BODY_DYNAMIC) ? 1 : 0,                                             \
   {0, 0},                                                                     \
   {0, 0}}

#define RigidBodyStatic(mass, damping)                                         \
  RigidBodyCreate(mass, damping, BODY_STATIC)

#define RigidBodyDynamic(mass, damping)                                        \
  RigidBodyCreate(mass, damping, BODY_DYNAMIC)

#define RigidBodyKinematic(mass, damping)                                      \
  RigidBodyCreate(mass, damping, BODY_KINEMATIC)

void ApplyForce(RigidBody *rb, Vector2 force);
void ApplyImpulse(RigidBody *rb, Vector2 impulse);
void ApplyDamping(RigidBody *rb);

typedef struct {
  Texture tex;
  Rectangle src;
  Color tint;
} Sprite;

typedef struct {
  Script OnEnable;
  Script OnDisable;
  Script scripts[EcsSystemLayers];
} Behaviour;
#define BEHAVIOUR_EMPTY {0}

void AddScript(ECS *ecs, Entity e, Script s, EcsLayer ly);

typedef struct {
  Entity entity;
} Parent;

typedef struct {
  Entity *list;
  Entity count;     // use the maximum value of Entity as max length.
  Entity allocated; // NEVER TOUCH THIS OR YOUR DEVICE WILL EXPLODE!!
} Children;

void EntityAddParent(ECS *ecs, Entity e, Entity p);
void EntityAddChild(ECS *ecs, Entity e, Entity c);
void EntityRemoveChild(ECS *ecs, Entity e, Entity c);

void EntityDestroy(ECS *ecs, Entity e);
void EntityDestroyRecursive(ECS *ecs, Entity e);

void EntityForEachChild(ECS *ecs, Entity e, Script s);
void EntityForEachChildRecursive(ECS *ecs, Entity e, Script s);

#endif
