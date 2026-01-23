#include <gearecs/scene.h>

#include <stdlib.h>

#ifdef PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif

GameScene SceneStart(uint16_t max_entities, Camera2D camera) {
  ECS *ecs = EcsRegistry(max_entities);

  Component(ecs, EntityData);
  Component(ecs, Parent);
  Component(ecs, Children);
  Component(ecs, Transform2);
  Component(ecs, Camera2D);
  Component(ecs, Sprite);
  Component(ecs, Behaviour);
  Component collider_id = Component(ecs, Collider);
  EcsComponentDestructor(ecs, collider_id, ColliderDestructor);
  Component(ecs, RigidBody);

  Entity camEntity = EcsEntity(ecs);
  AddComponentEx(ecs, camEntity, Camera2D, camera);

  System(ecs, BehaviourStartSystem, EcsOnStart, Behaviour);
  System(ecs, BehaviourUpdateSystem, EcsOnUpdate, Behaviour);
  System(ecs, BehaviourLateSystem, EcsOnLateUpdate, Behaviour);
  System(ecs, BehaviourFixedSystem, EcsOnFixedUpdate, Behaviour);
  System(ecs, BehaviourRenderSystem, EcsOnRender, Behaviour);
  System(ecs, BehaviourGuiSystem, EcsOnGui, Behaviour);

  System(ecs, HierarchyTransform, EcsOnUpdate, Transform2, Children);
  System(ecs, TransformColliderSystem, EcsOnUpdate, Transform2, Collider);
  System(ecs, CollisionSystem, EcsOnUpdate, Transform2, Collider);

  System(ecs, GravitySystem, EcsOnFixedUpdate, RigidBody);
  System(ecs, PhysicsSystem, EcsOnFixedUpdate, RigidBody, Transform2);

  System(ecs, SpriteSystem, EcsOnRender, Transform2, Sprite);

  return (GameScene){ecs, (Color){23, 28, 29, 255}, 0};
}

void GameGenericLoop(void *scene) {
  GameScene *gs = (GameScene *)scene;

  RunSystem(gs->ecs, EcsOnUpdate);
  RunSystem(gs->ecs, EcsOnLateUpdate);

  gs->fixed_time += GetFrameTime();
  while (gs->fixed_time >= FIXED_DELTATIME) {
    RunSystem(gs->ecs, EcsOnFixedUpdate);
    gs->fixed_time -= FIXED_DELTATIME;
  }

  BeginDrawing();
  ClearBackground(gs->background);

  Camera2D *cam = GetComponent(gs->ecs, 0, Camera2D);
  BeginMode2D(*cam);
  RunSystem(gs->ecs, EcsOnRender);
  EndMode2D();

  RunSystem(gs->ecs, EcsOnGui);
  EndDrawing();
}

void SceneLoop(GameScene *scene) {
  RunSystem(scene->ecs, EcsOnStart);
#ifdef PLATFORM_WEB
  emscripten_set_main_loop_arg(GameGenericLoop, scene, 0, 1);
#else
  while (!WindowShouldClose()) {
    GameGenericLoop(scene);
  }
#endif
}

void SceneClean(GameScene *scene) {
  if (scene->ecs == NULL)
    return;
  EcsFree(scene->ecs);
  scene->ecs = NULL;
}
