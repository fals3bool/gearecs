#include <gearecs/world.h>

#ifdef PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif

static float fixed_time; ///< Accumulator for fixed timestep integration
static Color background; ///< Window background color

ECS *EcsWorld(uint16_t max_entities, Camera2D camera) {
  ECS *ecs = EcsRegistry(max_entities);

  Component(ecs, EntityData);
  Component(ecs, Parent);
  // Component(ecs, Children);
  // @see ecs/component/hierarchy to check component size
  EcsRegisterComponent(ecs, "Children", sizeof(Entity) * 2 + sizeof(Entity *));
  Component(ecs, Transform2);
  Component(ecs, Camera2D);
  Component(ecs, Sprite);
  Component(ecs, Behaviour);
  Component(ecs, Collider);
  ComponentDtor(ecs, Collider, ColliderDestructor);
  Component(ecs, CollisionListener);
  Component(ecs, RigidBody);

  Entity camEntity = EcsEntity(ecs);
  AddComponentByRef(ecs, camEntity, Camera2D, camera);

  System(ecs, BehaviourStartSystem, EcsOnStart, Behaviour);
  System(ecs, BehaviourUpdateSystem, EcsOnUpdate, Behaviour);
  System(ecs, BehaviourLateSystem, EcsOnLateUpdate, Behaviour);
  System(ecs, BehaviourFixedSystem, EcsOnFixedUpdate, Behaviour);
  System(ecs, BehaviourRenderSystem, EcsOnRender, Behaviour);
  System(ecs, BehaviourGuiSystem, EcsOnGui, Behaviour);

  System(ecs, HierarchyTransformSystem, EcsOnUpdate, Transform2, Parent);
  System(ecs, TransformColliderSystem, EcsOnUpdate, Transform2, Collider);
  System(ecs, CollisionSystem, EcsOnUpdate, Transform2, Collider);

  System(ecs, GravitySystem, EcsOnFixedUpdate, RigidBody);
  System(ecs, PhysicsSystem, EcsOnFixedUpdate, RigidBody, Transform2);

  System(ecs, SpriteSystem, EcsOnRender, Transform2, Sprite);

  background = (Color){23, 28, 29, 255};
  return ecs;
}

void GameGenericLoop(void *world) {
  ECS *ecs = (ECS *)world;

  EcsRunSystems(ecs, EcsOnUpdate);
  EcsRunSystems(ecs, EcsOnLateUpdate);

  fixed_time += GetFrameTime();
  while (fixed_time >= FIXED_DELTATIME) {
    EcsRunSystems(ecs, EcsOnFixedUpdate);
    fixed_time -= FIXED_DELTATIME;
  }

  BeginDrawing();
  ClearBackground(background);

  Camera2D *cam = GetComponent(ecs, 0, Camera2D);
  BeginMode2D(*cam);
  EcsRunSystems(ecs, EcsOnRender);
  EndMode2D();

  EcsRunSystems(ecs, EcsOnGui);
  EndDrawing();
}

void EcsLoop(ECS *world) {
  if (!world)
    return;
  EcsRunSystems(world, EcsOnStart);
#ifdef PLATFORM_WEB
  emscripten_set_main_loop_arg(GameGenericLoop, scene, 0, 1);
#else
  while (!WindowShouldClose()) {
    GameGenericLoop(world);
  }
#endif
}
