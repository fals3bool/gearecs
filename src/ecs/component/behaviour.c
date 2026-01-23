#include <ecs/component.h>

void AddScript(ECS *ecs, Entity e, Script s, EcsLayer ly) {
  Behaviour *beh = GetComponent(ecs, e, Behaviour);
  beh->scripts[ly] = s;
}
