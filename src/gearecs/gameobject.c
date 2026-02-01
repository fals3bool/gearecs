#include <gearecs/gameobject.h>

Entity GameObject(ECS *ecs, char *tag) {
  Entity e = EcsEntity(ecs);
  AddComponent(ecs, e, EntityData, EntityDataActive(tag));
  AddComponent(ecs, e, Transform2, TransformZero);
  return e;
}
