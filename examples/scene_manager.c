#include <scene/manager.h>

void DRAW_SELF(Scene *scene, Entity self) {
  Transform2 *t = ecs_get(scene, self, Transform2);
  Rectangle rect = {t->position.x, t->position.y, 32, 32};
  DrawRectanglePro(rect, (Vector2){16, 16}, t->rotation * RAD2DEG, RED);
}

void MOVE(Scene *scene, Entity self) {
  Transform2 *t = ecs_get(scene, self, Transform2);
  Vector2 m = {0};
  if (IsKeyDown(KEY_W))
    m.y -= 100 * GetFrameTime();
  if (IsKeyDown(KEY_S))
    m.y += 100 * GetFrameTime();
  if (IsKeyDown(KEY_A))
    m.x -= 100 * GetFrameTime();
  if (IsKeyDown(KEY_D))
    m.x += 100 * GetFrameTime();
  t->position.x += m.x;
  t->position.y += m.y;
  t->rotation += GetFrameTime();
}

void load_test_scene(Scene *sc) {

  Entity e0 = ecs_entity_wdata(sc);
  ecs_add_def(sc, e0, Transform2, TRANSFORM_DEFAULT);
  ecs_add_def(sc, e0, Behaviour, BEHAVIOUR_DEFAULT);
  Collider col = ecs_collider_rot(4, 16, PI / 4);
  ecs_add_obj(sc, e0, Collider, col);
  ecs_script(sc, e0, MOVE, EcsOnUpdate);
  ecs_script(sc, e0, DRAW_SELF, EcsOnRender);

  Entity e1 = ecs_entity_wdata(sc);
  ecs_add_def(sc, e1, Transform2, {{50, 0}, {1, 1}, 0});
  Collider col2 = ecs_collider_solid(5, 20);
  ecs_add_obj(sc, e1, Collider, col2);

  Entity e2 = ecs_entity(sc);
}

int main(void) {

  Color bg = {32, 33, 37, 255};
  FalsECS *falsecs = falsecs_start(800, 450, "raylib example", bg);
  Scene *sc = falsecs_scene(falsecs);

  load_test_scene(sc);
  falsecs_loop(falsecs);
  falsecs_free(falsecs);

  return 0;
}
