#include <scene/manager.h>

#include <raymath.h>
#include <stdio.h>

#define SCREEN_W 800
#define SCREEN_H 450

void script_move(Registry *r, Entity self) {
  Transform2 *t = ecs_get(r, self, Transform2);
  RigidBody *rb = ecs_get(r, self, RigidBody);

  Camera2D *cam = ecs_get(r, 0, Camera2D);
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

void script_impulse(Registry *r, Entity self) {
  Transform2 *t = ecs_get(r, self, Transform2);
  RigidBody *rb = ecs_get(r, self, RigidBody);

  Vector2 d = {0};
  if (IsKeyDown(KEY_W))
    d.y -= 1;
  if (IsKeyDown(KEY_S))
    d.y += 1;
  if (IsKeyDown(KEY_A))
    d.x -= 1;
  if (IsKeyDown(KEY_D))
    d.x += 1;

  rb_apply_impulse(rb, Vector2Scale(Vector2Normalize(d), 400.f));
}

void script_gui(Registry *r, Entity self) {
  DrawText("Transform: [ARROWS]", 10, 10, 16, WHITE);
  DrawText("Impulse: [WASD]", 10, 30, 16, WHITE);
  DrawText("Switch RigidBody: [SPACE]", 10, 50, 16, WHITE);
}

void script_show_data(Registry *r, Entity self) {
  Transform2 *t = ecs_get(r, self, Transform2);
  RigidBody *rb = ecs_get(r, self, RigidBody);
  Collider *c = ecs_get(r, self, Collider);

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

void event_on_collision(CollisionEvent *event) {
  printf("Entity: {id: %d}, {collision of: %.2f}, {against: %d}\n", event->self,
         event->collision.distance, event->other);
}

void load_own_entities(Scene *s) {

  // COLLIDER: [SOLID]
  // BODY: [NONE]
  Entity A = ecs_entity_wdata(s);
  ecs_add(s, A, Transform2, TRANSFORM_POS(-250, 100));
  Collider colA = collider_solid(5, 16);
  colA.OnCollision = event_on_collision;
  ecs_add_obj(s, A, Collider, colA);
  ecs_add(s, A, Behaviour, BEHAVIOUR_EMPTY);
  ecs_script(s, A, script_show_data, EcsOnRender);

  // COLLIDER: [TRIGGER]
  // BODY: [NONE]
  Entity B = ecs_entity_wdata(s);
  ecs_add(s, B, Transform2, TRANSFORM_POS(-150, -100));
  Collider colB = collider_trigger(4, 20);
  colB.OnCollision = event_on_collision;
  ecs_add_obj(s, B, Collider, colB);
  ecs_add(s, B, Behaviour, BEHAVIOUR_EMPTY);
  ecs_script(s, B, script_show_data, EcsOnRender);

  // COLLIDER: [SOLID]
  // BODY: [DYNAMIC]
  Entity C = ecs_entity_wdata(s);
  ecs_add(s, C, Transform2, TRANSFORM_POS(0, 100));
  Collider colC = collider_solid(6, 18);
  ecs_add_obj(s, C, Collider, colC);
  RigidBody rbC = rigidbody_create(80, 1.2f, RIGIDBODY_DYNAMIC);
  rbC.gravity = false;
  ecs_add_obj(s, C, RigidBody, rbC);
  ecs_add(s, C, Behaviour, BEHAVIOUR_EMPTY);
  ecs_script(s, C, script_show_data, EcsOnRender);

  // COLLIDER: [SOLID]
  // BODY: [STATIC]
  Entity D = ecs_entity_wdata(s);
  ecs_add(s, D, Transform2, TRANSFORM_POS(150, -100));
  Collider colD = collider_solid(7, 24);
  ecs_add_obj(s, D, Collider, colD);
  ecs_add(s, D, RigidBody, rigidbody_create(50, 1, RIGIDBODY_STATIC));
  ecs_add(s, D, Behaviour, BEHAVIOUR_EMPTY);
  ecs_script(s, D, script_show_data, EcsOnRender);

  // COLLIDER: [SOLID]
  // BODY: [KINEMATIC]
  Entity E = ecs_entity_wdata(s);
  ecs_add(s, E, Transform2, TRANSFORM_POS(250, 100));
  Collider colE = collider_solid(4, 24);
  ecs_add_obj(s, E, Collider, colE);
  ecs_add(s, E, RigidBody, rigidbody_create(30, 1, RIGIDBODY_KINEMATIC));
  ecs_add(s, E, Behaviour, BEHAVIOUR_EMPTY);
  ecs_script(s, E, script_show_data, EcsOnRender);

  // PLAYER
  Entity P = ecs_entity_wdata(s);
  ecs_add(s, P, Transform2, TRANSFORM_ZERO);
  Collider colP = collider_solid(3, 22);
  colP.OnCollision = event_on_collision;
  ecs_add_obj(s, P, Collider, colP);
  ecs_add(s, P, Behaviour, BEHAVIOUR_EMPTY);
  ecs_add(s, P, RigidBody, rigidbody_create(50, 1.5f, RIGIDBODY_STATIC));
  ecs_script(s, P, script_move, EcsOnUpdate);
  ecs_script(s, P, script_impulse, EcsOnFixedUpdate);
  ecs_script(s, P, script_show_data, EcsOnRender);
  ecs_script(s, P, script_gui, EcsOnGui);

  ecs_system(s, EcsOnRender, ecs_debug_collider_system, Transform2, Collider);
}

int main(void) {

  InitWindow(SCREEN_W, SCREEN_H, "scene manager usage");
  Camera2D cam = {{SCREEN_W / 2.f, SCREEN_H / 2.f}, {0, 0}, 0, 1.f};
  Color bg = {0, 0, 0, 255};

  FalsECS falsecs = falsecs_start(32, bg);
  Scene *sc = falsecs_scene(&falsecs, cam);

  load_own_entities(sc);

  falsecs_loop(&falsecs);
  falsecs_clean(&falsecs);
  CloseWindow();

  return 0;
}
