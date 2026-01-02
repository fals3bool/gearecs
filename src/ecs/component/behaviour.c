#include <ecs/component.h>

void EcsScript(ECS *ecs, Entity e, Script s, EcsLayer ly) {
  Behaviour *beh = EcsGet(ecs, e, Behaviour);
  beh->scripts[ly] = s;
}
