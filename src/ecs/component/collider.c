#include <ecs/component.h>

#include <stdlib.h>

Collider ColliderCreate(int vertices, float radius, uint8_t solid) {
  Collider col = {0};
  col.vx = (Vector2 *)malloc(sizeof(Vector2) * vertices);
  col.md = (Vector2 *)malloc(sizeof(Vector2) * vertices);
  col.vertices = vertices;
  col.solid = solid;
  col.overlap = 0;

  float angle = PI * 2 / vertices;
  for (int i = 0; i < vertices; i++) {
    col.vx[i] = (Vector2){radius * cosf(angle * i), radius * sinf(angle * i)};
    col.md[i] = col.vx[i];
  }

  return col;
}

void ColliderDestructor(void *_self) {
  Collider *self = (Collider *)_self;
  free(self->md);
  free(self->vx);
}

void ColliderSetLayer(Collider *c, uint8_t layer) { c->layer = layer % 64; }

void ColliderEnableLayer(Collider *c, uint8_t layer) {
  c->collisionMask |= (1u << (layer % 64));
}

void ColliderDisableLayer(Collider *c, uint8_t layer) {
  c->collisionMask |= ~(1u << (layer % 64));
}

void ColliderDisableAllLayers(Collider *c) { c->collisionMask = 0; }

uint8_t ColliderHasLayerEnabled(const Collider *c, uint8_t layer) {
  return (c->collisionMask & (1u << (layer % 64))) == c->collisionMask;
}

uint8_t CanCollide(Collider *c1, Collider *c2) {
  return (c1->collisionMask == 0 || ColliderHasLayerEnabled(c1, c2->layer)) &&
         (c2->collisionMask == 0 || ColliderHasLayerEnabled(c2, c1->layer));
}
