#include <gearecs.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450

static float speed = 500.f;
static float jump = 10000.f;
static bool jumpColldown = true;

void ScriptMove(ECS *ecs, Entity self) {
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

  Entity box = EcsEntity(world, "box");
  AddComponent(world, box, Transform2, TransformOrigin);
  AddComponent(world, box, Collider,
               ColliderRect((Rectangle){0, 0, 10, 10}, true));
  AddComponent(world, box, CollisionListener, {OnGround});
  AddComponent(world, box, RigidBody, RigidBodyDynamic(50, 3));
  AddScript(world, box, ScriptMove, EcsOnFixedUpdate);

  Entity floor = EcsEntity(world, "floor");
  AddComponent(world, floor, Transform2, TransformPos(0, 100));
  AddComponent(world, floor, Collider,
               ColliderRect((Rectangle){-SCREEN_WIDTH / 2.f + 20, 0,
                                        SCREEN_WIDTH - 40, 10},
                            true));
  AddComponent(world, floor, RigidBody, RigidBodyStatic);

  EcsLoop(world);

  EcsFree(world);
  return 0;
}
