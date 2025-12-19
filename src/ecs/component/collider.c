#include <ecs/component.h>

#include <math.h>
#include <stdlib.h>

Collider ecs_collider(int vertices, float radius) {
  return ecs_collider_offset(vertices, radius, 0, (Vector2){0, 0});
}

Collider ecs_collider_rot(int vertices, float radius, float rot) {
  return ecs_collider_offset(vertices, radius, rot, (Vector2){0, 0});
}

Collider ecs_collider_offset(int vertices, float radius, float rot,
                             Vector2 origin) {
  Collider col = {0};
  col.vx = (Vector2 *)malloc(sizeof(Vector2) * vertices);
  col.md = (Vector2 *)malloc(sizeof(Vector2) * vertices);
  col.vertices = vertices;
  col.overlap = 0;
  col.origin = origin;
  col.rot = rot;

  float angle = PI * 2 / vertices;
  for (int i = 0; i < vertices; i++) {
    col.vx[i] = (Vector2){radius * cosf(angle * i), radius * sinf(angle * i)};
    col.md[i] = col.vx[i];
  }
  return col;
}
