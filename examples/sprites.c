#include <scene/manager.h>

#define SCREEN_W 800
#define SCREEN_H 450

int main(void) {
  InitWindow(SCREEN_W, SCREEN_H, "Sprites");
  Camera2D cam = {0};
  cam.offset = (Vector2){SCREEN_W / 2.f, SCREEN_H / 2.f};
  cam.zoom = 1.f;

  FalsECS man = falsecs_start(BLACK);
  Scene *scn = falsecs_scene(&man, cam);

  Entity ent = ecs_entity_wdata(scn);
  Image img = GenImageColor(32, 32, WHITE);
  Texture2D tex = LoadTextureFromImage(img);
  UnloadImage(img);
  ecs_add(scn, ent, Sprite, {tex, {0, 0, 32, 32}, BLUE});
  ecs_add(scn, ent, Transform2, {{0, 10}, {1, 3}, PI / 4});

  falsecs_loop(&man);
  falsecs_clean(&man);

  UnloadTexture(tex);

  CloseWindow();
}
