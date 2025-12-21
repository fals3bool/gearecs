#include <ecs/registry.h>
#include <stdio.h>

#define PRINT_ID(entity) printf("%s {id:%d}\n", #entity, entity)

int main(void) {

  Registry *r = ecs_registry();

  Entity e0 = ecs_entity(r); // id:0
  Entity e1 = ecs_entity(r); // id:1
  Entity e2 = ecs_entity(r); // id:2

  PRINT_ID(e0);
  PRINT_ID(e1);
  PRINT_ID(e2);

  ecs_entity_destroy(r, e1); // free id 1
  Entity e3 = ecs_entity(r); // id:1
  e1 = ecs_entity(r);        // id:3
  printf("- - -\n");

  PRINT_ID(e0);
  PRINT_ID(e1);
  PRINT_ID(e2);
  PRINT_ID(e3);

  return 0;
}
