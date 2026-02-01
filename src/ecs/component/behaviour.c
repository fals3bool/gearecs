#include <ecs/component.h>

void AddScript(ECS *ecs, Entity e, Script s, EcsLayer ly) {
  Behaviour *beh = GetComponent(ecs, e, Behaviour);
  if (!beh) {
    AddComponent(ecs, e, Behaviour, Empty);
    beh = GetComponent(ecs, e, Behaviour);
  }
  beh->scripts[ly] = s;
}
