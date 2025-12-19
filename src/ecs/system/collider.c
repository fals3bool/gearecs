#include <ecs/component.h>
#include <ecs/system.h>
#include <math.h>

void ecs_transform_collider_system(Registry *r, Entity e) {
  Transform2 *t = ecs_get(r, e, Transform2);
  Collider *c = ecs_get(r, e, Collider);

  float angle = t->rotation + c->rot;
  for (uint8_t i = 0; i < c->vertices; i++) {
    c->vx[i].x = c->md[i].x * cosf(angle) - c->md[i].y * sinf(angle) +
                 t->position.x + c->origin.x;
    c->vx[i].y = c->md[i].x * sinf(angle) + c->md[i].y * cosf(angle) +
                 t->position.y + c->origin.y;
    c->overlap = false;
  }
}

void ecs_debug_collider_system(Registry *r, Entity e) {
  Collider *col = ecs_get(r, e, Collider);
  for (uint8_t i = 0; i < col->vertices; i++) {
    Vector2 p = col->vx[i];
    Vector2 q = col->vx[(i + 1) % col->vertices];
    DrawLineEx(p, q, 2, col->overlap ? RED : SKYBLUE);
  }
}
