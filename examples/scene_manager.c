#include <scene/manager.h>

#define SCREEN_W 800
#define SCREEN_H 450
void load_own_entities(Scene *sc);

typedef struct {
  float x;
  float y;
} PositionF;

void move_system(Registry *r, Entity e) {
  PositionF *p = ecs_get(r, e, PositionF);
  p->x += 20 * GetFrameTime();
}

void draw_system(Registry *r, Entity e) {
  PositionF *p = ecs_get(r, e, PositionF);
  Rectangle rec = {p->x, p->y, 16, 16};
  DrawRectangleRec(rec, RED);
}

int main(void) {

  // Window and Camera are created and modified manually.
  InitWindow(SCREEN_W, SCREEN_H, "scene manager usage");
  Camera2D cam = {{SCREEN_W / 2.f, SCREEN_H / 2.f}, {0, 0}, 0, 1.f};
  Color bg = {0, 0, 0, 255};

  // Start the scene manager.
  FalsECS falsecs = falsecs_start(bg);

  // Create and scene (Scene is the same as Registry).
  Scene *sc = falsecs_scene(&falsecs, cam);

  load_own_entities(sc); // The same as seen before

  // The main loop runs all systems under their execution layer.
  falsecs_loop(&falsecs);

  // When the main loop stops, the scene needs to be freed.
  falsecs_clean(&falsecs);

  CloseWindow();

  return 0;
}

void load_own_entities(Scene *sc) {
  ecs_component(sc, PositionF);
  ecs_system(sc, EcsOnUpdate, move_system, PositionF);
  ecs_system(sc, EcsOnRender, draw_system, PositionF);

  Entity e = ecs_entity(sc);
  ecs_add(sc, e, PositionF, {20});
}
