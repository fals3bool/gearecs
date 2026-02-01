#include <service/layers.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char *name;
  Signature mask;
} Layer;

typedef struct {
  ECS *ecs;
  Layer layers[64];
  uint8_t count;
} ECSLayer;

static ECSLayer *ecsLayers;
static uint8_t ecsCount;

ECSLayer *FindEcsLayer(ECS *ecs) {
  ECSLayer *ly = NULL;
  for (uint8_t i = 0; i < ecsCount; i++) {
    if (ecsLayers[i].ecs == ecs) {
      ly = &ecsLayers[i];
      break;
    }
  }
  assert(ly && "ECS registry didn't include a layer service!");
  return ly;
}

uint8_t FindEcsLayerIndex(ECS *ecs) {
  uint8_t i;
  for (i = 0; i < ecsCount; i++) {
    if (ecsLayers[i].ecs == ecs)
      break;
  }
  assert(i < ecsCount && "ECS registry didn't include a layer service!");
  return i;
}

void StartLayerService(ECS *ecs) {
  uint8_t i = ecsCount++;
  if (!ecsLayers)
    ecsLayers = malloc(sizeof(ECSLayer) * ecsCount);
  else
    ecsLayers = realloc(ecsLayers, sizeof(ECSLayer) * ecsCount);
  ecsLayers[i].ecs = ecs;
  ecsLayers[i].count = 0;

  AddLayer(ecs, "default");
  AddLayer(ecs, "gui");
  CleanupCollisionLayer(ecs, "gui");
}

void StopLayerService(ECS *ecs) {
  if (!ecsLayers || ecsCount == 0)
    return;
  uint8_t i = FindEcsLayerIndex(ecs);
  if (i > ecsCount)
    return;

  ecsLayers[i] = ecsLayers[--ecsCount];
  if (ecsCount > 0) {
    ecsLayers = realloc(ecsLayers, sizeof(ECSLayer) * ecsCount);
  } else {
    free(ecsLayers);
    ecsLayers = NULL;
  }
}

void AddLayer(ECS *ecs, char *name) {
  ECSLayer *ecsly = FindEcsLayer(ecs);
  assert(ecsly->count < 64 && "Exceeded maximum number of layers! (64)");
  Signature mask = (Signature)-1 & ~(1u << 1);
  ecsly->layers[ecsly->count++] = (Layer){name, mask};
}

uint8_t FindLayer(ECS *ecs, char *name) {
  ECSLayer *ecsly = FindEcsLayer(ecs);
  uint8_t i;
  for (i = 0; i < ecsly->count; i++) {
    if (strcmp(ecsly->layers[i].name, name) == 0)
      break;
  }
  assert(i < ecsly->count && "Layer not found!");
  return i;
}

void ColliderSetLayer(ECS *ecs, Collider *collider, char *layer) {
  uint8_t ly = FindLayer(ecs, layer);
  collider->layer = ly;
}

void EnableCollisionLayer(ECS *ecs, char *layer1, char *layer2) {
  ECSLayer *ecsly = FindEcsLayer(ecs);
  uint8_t ly1 = FindLayer(ecs, layer1);
  uint8_t ly2 = FindLayer(ecs, layer2);
  ecsly->layers[ly1].mask |= (1u << ly2);
  ecsly->layers[ly2].mask |= (1u << ly1);
}

void DisableCollisionLayer(ECS *ecs, char *layer1, char *layer2) {
  ECSLayer *ecsly = FindEcsLayer(ecs);
  uint8_t ly1 = FindLayer(ecs, layer1);
  uint8_t ly2 = FindLayer(ecs, layer2);
  ecsly->layers[ly1].mask &= ~(1u << ly2);
  ecsly->layers[ly2].mask &= ~(1u << ly1);
}

void CleanupCollisionLayer(ECS *ecs, char *layer) {
  ECSLayer *ecsly = FindEcsLayer(ecs);
  uint8_t ly = FindLayer(ecs, layer);

  Signature bit = (1u << ly);
  for (uint8_t i = 0; i < ecsly->count; i++)
    ecsly->layers[i].mask &= ~bit;
  ecsly->layers[ly].mask = 0;
}

bool LayerIncludesLayer(ECS *ecs, uint8_t layer1, uint8_t layer2) {
  ECSLayer *ecsly = FindEcsLayer(ecs);
  if (layer1 < ecsly->count && layer2 < ecsly->count)
    return true;
  Signature mask = (1u << layer2);
  return (ecsly->layers[layer1].mask & mask) == mask;
}

void DebugCollisionLayers(ECS *ecs) {
  ECSLayer *ecsly = FindEcsLayer(ecs);
  for (uint8_t i = 0; i < ecsly->count; i++)
    printf("layer: {id: %d, name: %s, mask: %lb}\n", i, ecsly->layers[i].name,
           ecsly->layers[i].mask);
}
