#include <gearecs.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450
#define SPEED 900

void MoveScript(ECS *ecs, Entity self) {
  RigidBody *rb = GetComponent(ecs, self, RigidBody);
  Transform2 *t = GetComponent(ecs, self, Transform2);
  if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
    Vector2 d = {cosf(t->rotation), sinf(t->rotation)};
    ApplyImpulse(rb, Vector2Scale(d, SPEED));
  }

  Camera2D *cam = GetComponent(ecs, 0, Camera2D);

  Vector2 mu = GetScreenToWorld2D(GetMousePosition(), *cam);
  Vector2 md = {mu.x - t->position.x, mu.y - t->position.y};
  t->rotation = atan2f(md.y, md.x);
}

int main(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GearECS Example");
  Camera2D camera = {
      {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f}, {0, 0}, 0, 1.0f};

  ECS *world = EcsWorld(32, camera);

  Texture tex = LoadTexture("examples/res/example.png");
  Entity player = GameObject(world, "Player");
  AddComponent(world, player, Sprite, {tex, {0, 0, 32, 32}, WHITE});
  AddComponent(world, player, RigidBody, RigidBodyKinematic(50, 1.5f));
  AddScript(world, player, MoveScript, EcsOnFixedUpdate);

  EcsLoop(world);
  EcsFree(world);
  UnloadTexture(tex);
  return 0;
}
