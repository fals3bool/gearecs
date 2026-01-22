#ifndef ECS_REGISTRY_H
#define ECS_REGISTRY_H

#include <stddef.h>
#include <stdint.h>

typedef struct ECS ECS;
typedef uint16_t Entity;
typedef uint16_t Signature;
typedef uint16_t Component;
typedef void (*Script)(ECS *, Entity);
typedef struct {
  Script run;
  Signature mask;
} System;

ECS *EcsRegistry(uint16_t max_entities);
void EcsFree(ECS *ecs);

Entity EcsEntity(ECS *ecs);
uint8_t EcsEntityIsAlive(ECS *ecs, Entity e);
void EcsEntityFree(ECS *ecs, Entity e);

// non-object-oriented encapsulation
Entity EcsEntityCount(ECS *ecs);

void EntitySetActive(ECS *ecs, Entity e, uint8_t active);
uint8_t EntityIsActive(ECS *ecs, Entity e);

void EntitySetVisible(ECS *ecs, Entity e, uint8_t visible);
uint8_t EntityIsVisible(ECS *ecs, Entity e);

// ########### //
//  COMPONENT  //
// ########### //

#define EcsComponent(ecs, C) EcsRegisterComponent(ecs, #C, sizeof(C))

#define EcsAdd(ecs, entity, C, ...)                                            \
  EcsAddComponent(ecs, entity, EcsCID(ecs, #C), &(C)__VA_ARGS__)

#define EcsAddLocal(ecs, entity, C, ...)                                       \
  EcsAddComponent(ecs, entity, C##_, &(C)__VA_ARGS__)

#define EcsAddExt(ecs, entity, C, ...)                                         \
  EcsAddComponent(ecs, entity, EcsCID(ecs, #C), &__VA_ARGS__)

#define EcsGet(ecs, entity, C)                                                 \
  (C *)EcsGetComponent(ecs, entity, EcsCID(ecs, #C))

#define EcsGetOptional(ecs, entity, C)                                         \
  (C *)EcsGetComponentOptional(ecs, entity, EcsCID(ecs, #C))

#define EcsRemove(ecs, entity, C)                                              \
  EcsRemoveComponent(ecs, entity, EcsCID(ecs, #C))

Component EcsRegisterComponent(ECS *ecs, char *name, size_t size);
void EcsAddComponent(ECS *ecs, Entity e, Component id, void *data);
void EcsRemoveComponent(ECS *ecs, Entity e, Component id);
void *EcsGetComponent(ECS *ecs, Entity e, Component id);
void *EcsGetComponentOptional(ECS *ecs, Entity e, Component id);

uint8_t EcsHasComponent(ECS *ecs, Entity e, Signature mask);
Component EcsCID(ECS *ecs, char *name);

// ######### //
//  SYSTEMS  //
// ######### //

#define ECS_FIXED_DELTATIME 1.f / 60.f

typedef enum {
  EcsOnStart = 0,
  EcsOnUpdate,
  EcsOnLateUpdate,
  EcsOnFixedUpdate,
  EcsOnRender,
  EcsOnGui,
  EcsSystemLayers
} EcsLayer;

#define EcsSignature(ecs, ...) EcsSignatureImpl(ecs, #__VA_ARGS__)
Signature EcsSignatureImpl(ECS *ecs, const char *str);

#define EcsSystem(ecs, script, layer, ...)                                     \
  EcsAddSystem(ecs, script, layer, EcsSignature(ecs, __VA_ARGS__))

#define EcsSystemGlobal(ecs, script, layer) EcsAddSystem(ecs, script, layer, 0);

void EcsAddSystem(ECS *ecs, Script s, EcsLayer ly, Signature mask);
void EcsRun(ECS *ecs, EcsLayer ly);

#endif
