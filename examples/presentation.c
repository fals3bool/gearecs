#include <gearecs.h>
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450

static float speed = 500.f;
static float jump = 10000.f;
static bool jumpColldown = true;

void ScriptImpulse(ECS *ecs, Entity self) {
  RigidBody *rb = GetComponent(ecs, self, RigidBody);

  if (IsKeyDown(KEY_A))
    ApplyImpulse(rb, (Vector2){-speed, 0});
  if (IsKeyDown(KEY_D))
    ApplyImpulse(rb, (Vector2){speed, 0});

  if (IsKeyDown(KEY_SPACE) && !jumpColldown) {
    ApplyImpulse(rb, (Vector2){0, -jump});
    jumpColldown = true;
  }
}

void OnGround(ECS *ecs, CollisionEvent *event) {
  RigidBody *rb = GetComponent(ecs, event->self, RigidBody);
  rb->acc = (Vector2){0, 0};
  jumpColldown = false;
}

int main(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Getting Started with GearECS!");
  Camera2D camera = {{SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f}, {0, 0}, 0, 1};

  ECS *world = EcsWorld(32, camera);

  System(world, DebugColliderSystem, EcsOnRender, Collider, Transform2);

  Entity ball = GameObject(world, "ball");
  Rectangle ballr = {0, 0, 10, 10};
  Collider ballc = ColliderRect(ballr, true);
  AddComponentByRef(world, ball, Collider, ballc);
  AddComponent(world, ball, CollisionListener, {OnGround});
  AddComponent(world, ball, RigidBody, RigidBodyDynamic(50, 3));
  AddComponent(world, ball, Behaviour, Empty);
  AddScript(world, ball, ScriptImpulse, EcsOnFixedUpdate);

  Entity floor = GameObject(world, "floor");
  Rectangle floorr = {-SCREEN_WIDTH / 2.f + 20, 0, SCREEN_WIDTH - 40, 10};
  Collider floorc = ColliderRect(floorr, true);
  AddComponentByRef(world, floor, Collider, floorc);
  AddComponent(world, floor, RigidBody, RigidBodyStatic);
  Transform2 *t = GetComponent(world, floor, Transform2);
  t->position.y += 100;

  EcsLoop(world);

  EcsFree(world);
  return 0;
}
