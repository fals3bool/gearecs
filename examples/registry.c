#include <ecs/registry.h>

#include <stdio.h>

#define PRINT_ID(world, entity)                                                \
  printf("{id:%d, tag:%s}\n", entity, EcsEntityData(world, entity)->tag)

typedef struct {
  float x;
  float y;
} Position;

void Move(ECS *ecs, Entity e) {
  Position *pos = GetComponent(ecs, e, Position);
  pos->x += 100;
  printf("\nFrom system [MOVE] -> x += 100\n");
}

void Print(ECS *ecs, Entity e) {
  Position *pos = GetComponent(ecs, e, Position);
  printf("\nFrom system [PRINT] -> position: {%.2f, %.2f}\n", pos->x, pos->y);
}

int main(void) {

  // ECS REGISTRY
  ECS *world = EcsRegistry();

  // ENTITIES
  Entity e0 = EcsEntity(world, "e0");
  PRINT_ID(world, e0);
  Entity e1 = EcsEntity(world, "e1");
  PRINT_ID(world, e1);
  Entity e2 = EcsEntity(world, "e2");
  PRINT_ID(world, e2);
  EcsEntityFree(world, e1);
  Entity e3 = EcsEntity(world, "e3");
  PRINT_ID(world, e3);
  e1 = EcsEntity(world, "e1");
  PRINT_ID(world, e1);

  Component(world, Position);
  AddComponent(world, e1, Position, {23, 25});

  // SYSTEMS USING THE COMPONENT 'Position'
  System(world, Move, 0, Position);
  System(world, Print, 1, Position);

  // RUN SYSTEMS UNDER EXECUTION LAYERS
  EcsRunSystems(world, 1);
  EcsRunSystems(world, 0);
  EcsRunSystems(world, 1);

  EcsFree(world);

  return 0;
}
