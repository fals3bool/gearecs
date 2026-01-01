#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <ecs/component.h>
#include <ecs/system.h>

typedef struct {
  ECS *ecs;
  Color background;
  float fixed_time;
} GameScene;

Entity EcsEntityData(ECS *ecs);

GameScene SceneStart(uint16_t max_entities, Camera2D camera);
void SceneLoop(GameScene *scene);
void SceneClean(GameScene *scene);

#endif
