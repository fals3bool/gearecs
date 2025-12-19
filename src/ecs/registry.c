#include <ecs/registry.h>

#include <stdlib.h>

struct Registry {
  Entity free_entities[MAX_ENTITIES];
  Entity entity_count;
};

Registry *ecs_registry() {
  Registry *r = malloc(sizeof(Registry));
  r->entity_count = 0;
  return r;
}

void ecs_registry_free(Registry *r) { free(r); }

Entity ecs_entity(Registry *r) {
  Entity e;
  if (r->free_entities[r->entity_count])
    e = r->free_entities[r->entity_count];
  else
    e = r->entity_count;
  r->free_entities[r->entity_count++] = 0;
  return e;
}

void ecs_entity_destroy(Registry *r, Entity e) {
  r->free_entities[--r->entity_count] = e;
}
