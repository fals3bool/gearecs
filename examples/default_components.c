#include <scene/manager.h>

#define SCREEN_W 800
#define SCREEN_H 450
void load_own_entities(Scene *sc);

void move_system(Registry *r, Entity e) {
  Transform2 *t = ecs_get(r, e, Transform2);
  t->position.x += 20 * GetFrameTime();
}

void draw_system(Registry *r, Entity e) {
  Transform2 *t = ecs_get(r, e, Transform2);
  Rectangle rec = {t->position.x, t->position.y, 16, 16};
  DrawRectangleRec(rec, RED);
}

int main(void) {

  InitWindow(SCREEN_W, SCREEN_H, "Default Components");
  Camera2D cam = {{SCREEN_W / 2.f, SCREEN_H / 2.f}, {0, 0}, 0, 1.f};
  Color bg = {0, 0, 0, 255};

  FalsECS falsecs = falsecs_start(bg);

  // Creating a scene using falsecs, also register some components by default.
  // Camera2D is a component with id:{0}
  // (Also creates an entity with id:{0} with the camera as its component).
  Scene *sc = falsecs_scene(&falsecs, cam);

  // Here we add default components to entities such as Transform2.
  load_own_entities(sc);

  falsecs_loop(&falsecs);
  falsecs_clean(&falsecs);
  CloseWindow();

  return 0;
}

void load_own_entities(Scene *sc) {

  // IMPORTANT: Cannot use the component_id local-scope variable. It was defined
  // by falsecs internally.

  // ecs_add_cid(sc, entity, Transform2, {0}); // err.
  // ecs_system(sc, EcsOnUpdate, move_system, Transform2); // err.
  // ecs_system(sc, EcsOnRender, draw_system, Transform2); // err.
  // error: 'Transform2_ not defined'.

  // we can solve this using ecs_cid() to get the component id.
  // using the component id, we create a mask (Signature).
  Signature mask = (1 << ecs_cid(sc, "Transform2"));
  ecs_alloc_system(sc, EcsOnUpdate, move_system, mask);
  ecs_alloc_system(sc, EcsOnRender, draw_system, mask);

  Entity e = ecs_entity(sc);
  Entity e2 = ecs_entity(sc);
  Entity e3 = ecs_entity(sc);

  // ecs_add_cid(sc, e, Transform2, TRANSFORM_DEFAULT); // err.

  ecs_add(sc, e, Transform2, TRANSFORM_DEFAULT);

  Transform t2 = {{0, 50}, {1, 1}, 0};
  ecs_add_obj(sc, e2, Transform2, t2);

  Transform2 t = {{0, -50}, {1, 1}, 0};
  ecs_add_component(sc, e3, ecs_cid(sc, "Transform2"), &t);
}
