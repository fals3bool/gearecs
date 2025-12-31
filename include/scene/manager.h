#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <ecs/component.h>
#include <ecs/system.h>

typedef Registry Scene;

typedef struct {
  Color bg;
  float fixed_time;
  uint32_t max_entities;
  Registry *scene;
} FalsECS;

Entity ecs_entity_wdata(Scene *sc);

FalsECS falsecs_start(uint16_t max_entities, Color bg);
void falsecs_loop(FalsECS *falsecs);

Scene *falsecs_scene(FalsECS *falsecs, Camera2D camera);
void falsecs_clean(FalsECS *falsecs);

#endif
