#include <scene/manager.h>

#define SCREEN_W 800
#define SCREEN_H 450

int main(void) {

  InitWindow(SCREEN_W, SCREEN_H, "Default Components");
  Camera2D cam = {{SCREEN_W / 2.f, SCREEN_H / 2.f}, {0, 0}, 0, 1.f};
  Color bg = {0, 0, 0, 255};

  FalsECS falsecs = falsecs_start(bg);

  // Creating a scene using falsecs, also register some default systems.
  // Eg. Collisions
  Scene *sc = falsecs_scene(&falsecs, cam);
  // IMPORTANT: The local-scope variable is not accesible from here.

  // can create an entity with an EntityData component.
  // EntityData store params like 'active' and 'visible'.
  // Its usefull for some systems.
  Entity e1 = ecs_entity_wdata(sc);
  Entity e2 = ecs_entity_wdata(sc);

  // cannot use this function because it automatically cast to (Collider).
  // err: cannot take the address of an rvalue of type 'Collider'.

  // ecs_add(sc, e1, Collider, collider_hollow(3, 16)); // err.

  // instead we can use ecs_add_obj:
  Collider c1 = collider_hollow(3, 16);
  ecs_add_obj(sc, e1, Collider, c1);
  Collider c2 = collider_solid(5, 16);
  ecs_add_obj(sc, e2, Collider, c2);

  ecs_add_def(sc, e1, Transform2, {{0, 0}, {1, 1}, 0});
  ecs_add_def(sc, e1, Transform2, {{5, 0}, {1, 1}, 0});

  // The collision_system resolve collisions between solid colliders.
  // Otherwise let them overlap.

  // This systems show colliders vertices and edges.
  // Uses components Transform2 and Collider.
  Signature mask =
      ((1 << ecs_cid(sc, "Collider") & (1 << ecs_cid(sc, "Transform2"))));
  ecs_alloc_system(sc, EcsOnRender, ecs_debug_collider_system, mask);
  // You will see them with different colors if overlapped.

  falsecs_loop(&falsecs);
  falsecs_clean(&falsecs);
  CloseWindow();

  return 0;
}
