#include <ecs/component.h>
#include <ecs/system.h>

void bsp(ECS *ecs, Entity e, EcsPhase phase) {
  Behaviour *beh = GetComponent(ecs, e, Behaviour);
  if (beh->scripts[phase])
    beh->scripts[phase](ecs, e);
}

void BehaviourStartSystem(ECS *ecs, Entity e) { bsp(ecs, e, EcsOnStart); }
void BehaviourUpdateSystem(ECS *ecs, Entity e) { bsp(ecs, e, EcsOnUpdate); }
void BehaviourLateSystem(ECS *ecs, Entity e) { bsp(ecs, e, EcsOnLateUpdate); }
void BehaviourFixedSystem(ECS *ecs, Entity e) { bsp(ecs, e, EcsOnFixedUpdate); }
void BehaviourRenderSystem(ECS *ecs, Entity e) { bsp(ecs, e, EcsOnRender); }
void BehaviourGuiSystem(ECS *ecs, Entity e) { bsp(ecs, e, EcsOnGui); }
