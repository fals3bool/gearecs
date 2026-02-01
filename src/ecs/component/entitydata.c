#include <ecs/component.h>

#include <string.h>

void ActiveChildScript(ECS *ecs, Entity e) { EntitySetActive(ecs, e, true); }
void DeactiveChildScript(ECS *ecs, Entity e) { EntitySetActive(ecs, e, false); }
void ShowChildScript(ECS *ecs, Entity e) { EntitySetVisible(ecs, e, true); }
void HideChildScript(ECS *ecs, Entity e) { EntitySetVisible(ecs, e, false); }

void EntitySetActive(ECS *ecs, Entity e, bool active) {
  EntityData *edata = GetComponent(ecs, e, EntityData);
  if (edata) {
    bool prev = edata->active;
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

  if (active)
    EntityForEachChildRecursive(ecs, e, ActiveChildScript);
  else
    EntityForEachChildRecursive(ecs, e, DeactiveChildScript);
}

bool EntityIsActive(ECS *ecs, Entity e) {
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

void EntitySetVisible(ECS *ecs, Entity e, bool visible) {
  EntityData *edata = GetComponent(ecs, e, EntityData);
  if (edata)
    edata->visible = visible;

  if (visible)
    EntityForEachChildRecursive(ecs, e, ShowChildScript);
  else
    EntityForEachChildRecursive(ecs, e, HideChildScript);
}

bool EntityIsVisible(ECS *ecs, Entity e) {
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
  found = (Entity)-1;
  EcsForEachEntity(ecs, FindByTagScript);
  return found;
}

bool HasTag(ECS *ecs, Entity e, char *tag) {
  EntityData *ed = GetComponent(ecs, e, EntityData);
  if (!ed)
    return false;
  if (ed->tag && strcmp(ed->tag, tag) == 0)
    return true;
  return false;
}
