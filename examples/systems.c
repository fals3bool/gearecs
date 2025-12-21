#include <ecs/system.h>
#include <stdio.h>

typedef struct {
  float x;
  float y;
} Position;

typedef struct {
  float x;
  float y;
} StaticPosition;

void move_system(Registry *r, Entity e) {
  Position *p = ecs_get(r, e, Position);
  p->x += 100;
}

int main(void) {
  Registry *r = ecs_registry();
  ecs_component(r, Position);
  ecs_component(r, StaticPosition);

  // Register a system under a execution layer.
  // Also need to specify the component to compare signatures.
  // The systems executes for every entity that has a Position component.
  ecs_system(r, EcsOnUpdate, move_system, Position);

  Entity e1 = ecs_entity(r);
  Entity e2 = ecs_entity(r);
  Entity e3 = ecs_entity(r);
  ecs_add(r, e1, Position, {20, 20});
  ecs_add(r, e2, Position, {20, 20});
  ecs_add(r, e3, StaticPosition, {20, 20});

  Position *p1 = ecs_get(r, e1, Position);
  Position *p2 = ecs_get(r, e2, Position);
  StaticPosition *p3 = ecs_get(r, e3, StaticPosition);
  printf("position 1: {%.2f, %.2f}\n", p1->x, p1->y);
  printf("position 2: {%.2f, %.2f}\n", p2->x, p2->y);
  printf("position 3: {%.2f, %.2f}\n\n", p3->x, p3->y);

  // Run systems:
  ecs_run(r, EcsOnUpdate);

  printf("position 1: {%.2f, %.2f}\n", p1->x, p1->y);
  printf("position 2: {%.2f, %.2f}\n", p2->x, p2->y);
  printf("position 3: {%.2f, %.2f}\n", p3->x, p3->y);

  ecs_registry_free(r);

  return 0;
}
