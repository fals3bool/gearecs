#include <ecs/component.h>
#include <string.h>

void EntitySetActive(ECS *ecs, Entity e, uint8_t active) {
  EntityData *edata = GetComponent(ecs, e, EntityData);
  if (edata) {
    uint8_t prev = edata->active;
    edata->active = active;
    Behaviour *beh = GetComponent(ecs, e, Behaviour);
    if (beh && active != prev) {
      if (active) {
        if (beh->OnEnable)
          beh->OnEnable(ecs, e);
      } else if (beh->OnDisable)
        beh->OnDisable(ecs, e);
    }
  }

  Children *children = GetComponent(ecs, e, Children);
  if (children)
    for (Entity i = 0; i < children->count; i++)
      EntitySetActive(ecs, children->list[i], active);
}

uint8_t EntityIsActive(ECS *ecs, Entity e) {
  EntityData *edata = GetComponent(ecs, e, EntityData);
  if (edata)
    return edata->active;
  if (!EcsEntityIsAlive(ecs, e))
    return false;

  Parent *parent = GetComponent(ecs, e, Parent);
  while (parent)
    return EntityIsActive(ecs, parent->entity);
  return true;
}

void EntitySetVisible(ECS *ecs, Entity e, uint8_t visible) {
  EntityData *edata = GetComponent(ecs, e, EntityData);
  if (edata)
    edata->visible = visible;

  Children *children = GetComponent(ecs, e, Children);
  if (children)
    for (Entity i = 0; i < children->count; i++)
      EntitySetVisible(ecs, children->list[i], visible);
}

uint8_t EntityIsVisible(ECS *ecs, Entity e) {
  EntityData *edata = GetComponent(ecs, e, EntityData);
  if (edata)
    return edata->visible;
  if (!EcsEntityIsAlive(ecs, e))
    return false;

  Parent *parent = GetComponent(ecs, e, Parent);
  while (parent)
    return EntityIsVisible(ecs, parent->entity);
  return true;
}

static Entity found = -1;
static char *findtag;
void FindByTagScript(ECS *ecs, Entity e) {
  if (HasTag(ecs, e, findtag))
    found = e;
}

Entity FindByTag(ECS *ecs, char *tag) {
  findtag = tag;
  found = -1;
  EcsForEachEntity(ecs, FindByTagScript);
  return found;
}

uint8_t HasTag(ECS *ecs, Entity e, char *tag) {
  EntityData *ed = GetComponent(ecs, e, EntityData);
  if (!ed)
    return 0;
  if (ed->tag && strcmp(ed->tag, tag) == 0)
    return 1;
  return 0;
}
