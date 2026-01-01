#include <ecs/component.h>

void EntityEnable(ECS *ecs, Entity e, uint8_t active) {
  EntityData *edata = EcsGet(ecs, e, EntityData);
  uint8_t prev = edata->active;
  edata->active = active;
  if (EcsHasComponent(ecs, e, EcsSignature(ecs, Behaviour)) && active != prev) {
    Behaviour *beh = EcsGet(ecs, e, Behaviour);
    if (active) {
      if (beh->OnEnable)
        beh->OnEnable(ecs, e);
    } else if (beh->OnDisable)
      beh->OnDisable(ecs, e);
  }
}

void EntityVisible(ECS *ecs, Entity e, uint8_t visible) {
  EntityData *edata = EcsGet(ecs, e, EntityData);
  edata->visible = visible;
}
