#include <ecs/registry.h>

#include <stdio.h>

void print_entities(Entity e1, Entity e2, Entity e3) {
  printf("\nentities ids:\n [e1] {%d}\n [e2] {%d}\n [e3] {%d}\n", e1, e2, e3);
}

int main(void) {

  Registry *world = ecs_registry();
  Entity e0 = ecs_entity(world); // do not destroy

  Entity e1 = ecs_entity(world);
  Entity e2 = ecs_entity(world);
  print_entities(e1, e2, -1);
  ecs_entity_destroy(world, e1);
  Entity e3 = ecs_entity(world);
  e1 = ecs_entity(world);
  print_entities(e1, e2, e3);

  ecs_entity_destroy(world, e2);
  ecs_entity_destroy(world, e1);
  e2 = ecs_entity(world);
  e1 = ecs_entity(world);
  print_entities(e1, e2, e3);

  Entity e4 = ecs_entity(world);
  print_entities(e4, -1, -1);

  ecs_registry_free(world);

  return 0;
}
