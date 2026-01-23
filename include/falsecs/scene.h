#ifndef GEARECS_SCENE
#define GEARECS_SCENE

#include <falsecs/gameobject.h>
#include <ecs/system.h>

typedef struct {
  ECS *ecs;
  Color background;
  float fixed_time;
} GameScene;

GameScene SceneStart(uint16_t max_entities, Camera2D camera);
void SceneLoop(GameScene *scene);
void SceneClean(GameScene *scene);

#endif
