#include <ecs/registry.h>

#include <stdio.h>

#define PRINT_ID(entity) printf("%s {id:%d}\n", #entity, entity)

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
  ECS *world = EcsRegistry(32);

  // ENTITIES
  Entity e0 = EcsEntity(world);
  PRINT_ID(e0);
  Entity e1 = EcsEntity(world);
  PRINT_ID(e1);
  Entity e2 = EcsEntity(world);
  PRINT_ID(e2);
  EcsEntityFree(world, e1);
  Entity e3 = EcsEntity(world);
  PRINT_ID(e3);
  e1 = EcsEntity(world);
  PRINT_ID(e1);

  // REGISTER AND ADD COMPONENT
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
