#include <gearecs.h>

typedef struct {
  float speed;
  float jump;
  bool cooldown;
} PlayerData;

void ScriptMove(ECS *ecs, Entity self) {
  RigidBody *rb = GetComponent(ecs, self, RigidBody);
  PlayerData *pd = GetComponent(ecs, self, PlayerData);

  if (IsKeyDown(KEY_A))
    ApplyImpulse(rb, (Vector2){-pd->speed, 0});
  if (IsKeyDown(KEY_D))
    ApplyImpulse(rb, (Vector2){pd->speed, 0});

  if (IsKeyDown(KEY_SPACE) && !pd->cooldown) {
    ApplyImpulse(rb, (Vector2){0, -pd->jump});
    pd->cooldown = true;
  }
}

void OnGround(ECS *ecs, CollisionEvent *event) {
  RigidBody *rb = GetComponent(ecs, event->self, RigidBody);
  PlayerData *pd = GetComponent(ecs, event->self, PlayerData);
  rb->acc = (Vector2){0, 0};
  pd->cooldown = false;
}

int main(void) {
  InitWindow(800, 450, "Getting Started with GearECS!");

  ECS *world = EcsWorld();
  Component(world, PlayerData);

  Entity box = EcsEntity(world, "box");
  AddComponent(world, box, PlayerData, {600, 10000, true});
  AddComponent(world, box, Transform2, TransformOrigin);
  AddComponent(world, box, Collider,
               ColliderRect((Rectangle){0, 0, 10, 10}, true));
  AddComponent(world, box, CollisionListener, {OnGround});
  AddComponent(world, box, RigidBody, RigidBodyDynamic(50, 3));
  AddScript(world, box, ScriptMove, EcsOnFixedUpdate);

  Entity floor = EcsEntity(world, "floor");
  AddComponent(world, floor, Transform2, TransformPos(0, 100));
  AddComponent(world, floor, Collider,
               ColliderRect((Rectangle){-350, 0, 700, 10}, true));
  AddComponent(world, floor, RigidBody, RigidBodyStatic);

  System(world, DebugColliderSystem, EcsOnRender, Collider, Transform2);
  EcsLoop(world);

  EcsFree(world);
  CloseWindow();
  return 0;
}
