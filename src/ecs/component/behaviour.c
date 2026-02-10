#include <ecs/component.h>

void AddScript(ECS *ecs, Entity e, Script s, EcsPhase phase) {
  Behaviour *beh = GetComponent(ecs, e, Behaviour);
  if (!beh) {
    AddComponent(ecs, e, Behaviour, Empty);
    beh = GetComponent(ecs, e, Behaviour);
  }
  beh->scripts[phase] = s;
}
