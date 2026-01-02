#include <ecs/component.h>
#include <ecs/system.h>

void bsy(ECS *ecs, Entity e, EcsLayer ly) {
  Behaviour *beh = EcsGet(ecs, e, Behaviour);
  if (beh->scripts[ly])
    beh->scripts[ly](ecs, e);
}

void BehaviourStartSystem(ECS *ecs, Entity e) { bsy(ecs, e, EcsOnStart); }
void BehaviourUpdateSystem(ECS *ecs, Entity e) { bsy(ecs, e, EcsOnUpdate); }
void BehaviourLateSystem(ECS *ecs, Entity e) { bsy(ecs, e, EcsOnLateUpdate); }
void BehaviourFixedSystem(ECS *ecs, Entity e) { bsy(ecs, e, EcsOnFixedUpdate); }
void BehaviourRenderSystem(ECS *ecs, Entity e) { bsy(ecs, e, EcsOnRender); }
void BehaviourGuiSystem(ECS *ecs, Entity e) { bsy(ecs, e, EcsOnGui); }
