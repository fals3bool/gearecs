#include <falsecs/gameobject.h>

Entity GameObject(ECS *ecs) {
  Entity e = EcsEntity(ecs);
  AddComponent(ecs, e, EntityData, ENTITYDATA_ACTIVE);
  AddComponent(ecs, e, Transform2, TRANSFORM_ZERO);
  return e;
}
