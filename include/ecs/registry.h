#ifndef ECS_REGISTRY_H
#define ECS_REGISTRY_H

#include <stddef.h>
#include <stdint.h>

typedef struct Registry ECS;
typedef uint16_t EcsID;
typedef EcsID Entity;
typedef EcsID Component;
typedef uint64_t Signature;
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
Entity EcsEntityCount(ECS *ecs);

void EcsForEachEntity(ECS *ecs, Script script);

void EntitySetActive(ECS *ecs, Entity e, uint8_t active);
uint8_t EntityIsActive(ECS *ecs, Entity e);

void EntitySetVisible(ECS *ecs, Entity e, uint8_t visible);
uint8_t EntityIsVisible(ECS *ecs, Entity e);

// ########### //
//  COMPONENT  //
// ########### //

#define Component(ecs, C) EcsRegisterComponent(ecs, #C, sizeof(C))

#define ComponentDtor(ecs, C, dtor)                                            \
  EcsComponentDestructor(ecs, EcsCID(ecs, #C), dtor);

#define AddComponent(ecs, entity, C, ...)                                      \
  EcsAddComponent(ecs, entity, EcsCID(ecs, #C), &(C)__VA_ARGS__)

#define AddComponentById(ecs, entity, C, ...)                                  \
  EcsAddComponent(ecs, entity, C##_, &(C)__VA_ARGS__)

#define AddComponentByRef(ecs, entity, C, ...)                                 \
  EcsAddComponent(ecs, entity, EcsCID(ecs, #C), &__VA_ARGS__)

#define GetComponent(ecs, entity, C)                                           \
  (C *)EcsGetComponent(ecs, entity, EcsCID(ecs, #C))

#define RemoveComponent(ecs, entity, C)                                        \
  EcsRemoveComponent(ecs, entity, EcsCID(ecs, #C))

#define ComponentID(ecs, C) EcsCID(ecs, #C)

Component EcsRegisterComponent(ECS *ecs, char *name, size_t size);
void EcsAddComponent(ECS *ecs, Entity e, Component id, void *data);
void EcsRemoveComponent(ECS *ecs, Entity e, Component id);
void *EcsGetComponent(ECS *ecs, Entity e, Component id);

void EcsComponentDestructor(ECS *ecs, Component id, void (*_dtor)(void *));

uint8_t EcsHasComponent(ECS *ecs, Entity e, Signature mask);
Component EcsCID(ECS *ecs, char *name);

// ######### //
//  SYSTEMS  //
// ######### //

#define FIXED_DELTATIME 1.f / 60.f

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

#define System(ecs, script, layer, ...)                                        \
  EcsAddSystem(ecs, script, layer, EcsSignature(ecs, __VA_ARGS__))

#define SystemGlobal(ecs, script, layer) EcsAddSystem(ecs, script, layer, 0);

void EcsAddSystem(ECS *ecs, Script s, EcsLayer ly, Signature mask);
void RunSystem(ECS *ecs, EcsLayer ly);

#endif
