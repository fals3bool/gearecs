#include <ecs/component.h>

void entity_enable(Registry *r, Entity e, uint8_t active) {
  EntityData *edata = ecs_get(r, e, EntityData);
  uint8_t prev = edata->active;
  edata->active = active;
  if (ecs_has_component(r, e, (1 << ecs_cid(r, "Behaviour"))) &&
      active != prev) {
    Behaviour *beh = ecs_get(r, e, Behaviour);
    if (active && beh->OnEnable)
      beh->OnEnable(r, e);
    else if (beh->OnDisable)
      beh->OnDisable(r, e);
  }
}

void entity_visible(Registry *r, Entity e, uint8_t visible) {
  EntityData *edata = ecs_get(r, e, EntityData);
  edata->visible = visible;
}
