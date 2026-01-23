#include <gearecs/scene.h>

#include <raymath.h>
#include <stdio.h>

#define SCREEN_W 800
#define SCREEN_H 450

void ScriptMove(ECS *ecs, Entity self) {
  Transform2 *t = GetComponent(ecs, self, Transform2);
  RigidBody *rb = GetComponent(ecs, self, RigidBody);

  Camera2D *cam = GetComponent(ecs, 0, Camera2D);
  Vector2 mu = GetScreenToWorld2D(GetMousePosition(), *cam);
  Vector2 md = {mu.x - t->position.x, mu.y - t->position.y};
  t->rotation = atan2f(md.y, md.x);

  Vector2 d = {0};
  if (IsKeyDown(KEY_UP))
    d.y -= 1;
  if (IsKeyDown(KEY_DOWN))
    d.y += 1;
  if (IsKeyDown(KEY_LEFT))
    d.x -= 1;
  if (IsKeyDown(KEY_RIGHT))
    d.x += 1;

  d = Vector2Scale(Vector2Normalize(d), 150 * GetFrameTime());
  t->position.x += d.x;
  t->position.y += d.y;

  if (IsKeyPressed(KEY_SPACE)) {
    rb->gravity = false;
    rb->type++;
    if (rb->type > RIGIDBODY_KINEMATIC)
      rb->type = 0;
  }
}

void ScriptImpulse(ECS *ecs, Entity self) {
  RigidBody *rb = GetComponent(ecs, self, RigidBody);

  Vector2 d = {0};
  if (IsKeyDown(KEY_W))
    d.y -= 1;
  if (IsKeyDown(KEY_S))
    d.y += 1;
  if (IsKeyDown(KEY_A))
    d.x -= 1;
  if (IsKeyDown(KEY_D))
    d.x += 1;

  BodyApplyImpulse(rb, Vector2Scale(Vector2Normalize(d), 400.f));
}

void ScriptGui(ECS *ecs, Entity self) {
  DrawText("Transform: [ARROWS]", 10, 10, 16, WHITE);
  DrawText("Impulse: [WASD]", 10, 30, 16, WHITE);
  DrawText("Switch RigidBody: [SPACE]", 10, 50, 16, WHITE);
}

void ScriptShowData(ECS *ecs, Entity self) {
  Transform2 *t = GetComponent(ecs, self, Transform2);
  RigidBody *rb = GetComponent(ecs, self, RigidBody);
  Collider *c = GetComponent(ecs, self, Collider);

  int fontsize = 12;
  char coltxt[20];
  snprintf(coltxt, 18, "Collider: %s", c->solid ? "SOLID" : "TRIGGER");
  DrawText(coltxt,
           t->position.x -
               MeasureTextEx(GetFontDefault(), coltxt, fontsize, 1).x / 2,
           t->position.y + 20, fontsize, WHITE);

  char rbtxt[20];
  snprintf(rbtxt, 16, "Body: %s",
           rb ? rb->type == RIGIDBODY_DYNAMIC  ? "DYNAMIC"
                : rb->type == RIGIDBODY_STATIC ? "STATIC"
                                               : "KINEMATIC"
              : "NONE");
  DrawText(rbtxt,
           t->position.x -
               MeasureTextEx(GetFontDefault(), rbtxt, fontsize, 1).x / 2,
           t->position.y + 40, fontsize, WHITE);
}

void OnCollisionHandler(ECS *ecs, CollisionEvent *event) {
  printf("COLLISION: Entity %d hit %d (depth: %.2f)\n", event->self,
         event->other, event->collision.distance);
}

void LoadScene(ECS *ecs) {

  // COLLIDER: [SOLID]
  // BODY: [NONE]
  Entity A = EcsEntity(ecs);
  AddComponent(ecs, A, Transform2, TRANSFORM_POS(-250, 100));
  Collider colA = ColliderSolid(5, 16);
  AddComponentEx(ecs, A, Collider, colA);
  AddComponent(ecs, A, CollisionListener, {OnCollisionHandler});
  AddComponent(ecs, A, Behaviour, BEHAVIOUR_EMPTY);
  AddScript(ecs, A, ScriptShowData, EcsOnRender);

  // COLLIDER: [TRIGGER]
  // BODY: [NONE]
  Entity B = EcsEntity(ecs);
  AddComponent(ecs, B, Transform2, TRANSFORM_POS(-150, -100));
  Collider colB = ColliderTrigger(4, 20);
  AddComponentEx(ecs, B, Collider, colB);
  AddComponent(ecs, B, CollisionListener, {OnCollisionHandler});
  AddComponent(ecs, B, Behaviour, BEHAVIOUR_EMPTY);
  AddScript(ecs, B, ScriptShowData, EcsOnRender);

  // COLLIDER: [SOLID]
  // BODY: [DYNAMIC]
  Entity C = EcsEntity(ecs);
  AddComponent(ecs, C, Transform2, TRANSFORM_POS(0, 100));
  Collider colC = ColliderSolid(6, 18);
  AddComponentEx(ecs, C, Collider, colC);
  RigidBody rbC = rigidbody_create(80, 1.2f, RIGIDBODY_DYNAMIC);
  rbC.gravity = false;
  AddComponentEx(ecs, C, RigidBody, rbC);
  AddComponent(ecs, C, Behaviour, BEHAVIOUR_EMPTY);
  AddScript(ecs, C, ScriptShowData, EcsOnRender);

  // COLLIDER: [SOLID]
  // BODY: [STATIC]
  Entity D = EcsEntity(ecs);
  AddComponent(ecs, D, Transform2, TRANSFORM_POS(150, -100));
  Collider colD = ColliderSolid(7, 24);
  AddComponentEx(ecs, D, Collider, colD);
  AddComponent(ecs, D, RigidBody, rigidbody_create(50, 1, RIGIDBODY_STATIC));
  AddComponent(ecs, D, Behaviour, BEHAVIOUR_EMPTY);
  AddScript(ecs, D, ScriptShowData, EcsOnRender);

  // COLLIDER: [SOLID]
  // BODY: [KINEMATIC]
  Entity E = EcsEntity(ecs);
  AddComponent(ecs, E, Transform2, TRANSFORM_POS(250, 100));
  Collider colE = ColliderSolid(4, 24);
  AddComponentEx(ecs, E, Collider, colE);
  AddComponent(ecs, E, RigidBody, rigidbody_create(30, 1, RIGIDBODY_KINEMATIC));
  AddComponent(ecs, E, Behaviour, BEHAVIOUR_EMPTY);
  AddScript(ecs, E, ScriptShowData, EcsOnRender);

  // PLAYER
  Entity P = EcsEntity(ecs);
  AddComponent(ecs, P, Transform2, TRANSFORM_ZERO);
  Collider colP = ColliderSolid(3, 22);
  AddComponentEx(ecs, P, Collider, colP);
  AddComponent(ecs, P, CollisionListener, {OnCollisionHandler});
  AddComponent(ecs, P, Behaviour, BEHAVIOUR_EMPTY);
  AddComponent(ecs, P, RigidBody, rigidbody_create(50, 1.5f, RIGIDBODY_STATIC));
  AddScript(ecs, P, ScriptMove, EcsOnUpdate);
  AddScript(ecs, P, ScriptImpulse, EcsOnFixedUpdate);
  AddScript(ecs, P, ScriptShowData, EcsOnRender);
  AddScript(ecs, P, ScriptGui, EcsOnGui);

  System(ecs, DebugColliderSystem, EcsOnRender, Transform2, Collider);
}

int main(void) {

  InitWindow(SCREEN_W, SCREEN_H, "Colliders & RigidBodies");
  Camera2D cam = {{SCREEN_W / 2.f, SCREEN_H / 2.f}, {0, 0}, 0, 1.f};

  GameScene gs = SceneStart(32, cam);
  LoadScene(gs.ecs);
  SceneLoop(&gs);

  SceneClean(&gs);
  CloseWindow();

  return 0;
}
