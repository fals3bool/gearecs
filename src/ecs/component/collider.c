#include <ecs/component.h>

#include <stdlib.h>

Collider ColliderCreate(int vertices, float radius, bool solid) {
  Collider col = {0};
  col.vx = (Vector2 *)malloc(sizeof(Vector2) * vertices);
  col.md = (Vector2 *)malloc(sizeof(Vector2) * vertices);
  col.vertices = vertices;
  col.solid = solid;

  float angle = PI * 2 / vertices;
  for (int i = 0; i < vertices; i++) {
    col.vx[i] = (Vector2){radius * cosf(angle * i), radius * sinf(angle * i)};
    col.md[i] = col.vx[i];
  }

  return col;
}

Collider ColliderVec(uint8_t vertices, Vector2 *vecs, bool solid) {
  Collider col = {0};
  col.vertices = vertices;
  col.solid = solid;
  col.vx = (Vector2 *)malloc(sizeof(Vector2) * vertices);
  col.md = (Vector2 *)malloc(sizeof(Vector2) * vertices);

  for (uint8_t i = 0; i < vertices; i++) {
    col.vx[i] = vecs[i];
    col.md[i] = vecs[i];
  }
  return col;
}

Collider ColliderRect(Rectangle rect, bool solid) {
  Vector2 vecs[] = {{rect.x, rect.y},
                    {rect.x, rect.y + rect.height},
                    {rect.x + rect.width, rect.y + rect.height},
                    {rect.x + rect.width, rect.y}};
  return ColliderVec(4, vecs, solid);
}

void ColliderDestructor(void *_self) {
  Collider *self = (Collider *)_self;
  free(self->md);
  free(self->vx);
}
