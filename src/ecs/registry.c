#include <ecs/registry.h>

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char *name;
  void *list;
  size_t size;
  void (*dtor)(void *);
} ComponentList;

typedef struct {
  System *list;
  size_t size;
  size_t alloc;
} LayerSystems;

struct Registry {
  Entity max_entities;
  Signature *entities;
  Entity entity_count;
  Entity *free_entities;
  Entity free_count;

  ComponentList *components;
  Component comp_count;
  Component comp_alloc;

  LayerSystems *systems;
};

void EcsAllocSystems(ECS *ecs);
void EcsFreeSystems(ECS *ecs);

void EcsAllocEntities(ECS *ecs, Entity max_entities);
void EcsFreeEntities(ECS *ecs);

void EcsFreeComponents(ECS *ecs);

ECS *EcsRegistry(uint16_t max_entities) {
  ECS *ecs = malloc(sizeof(ECS));
  ecs->components = NULL;
  ecs->comp_alloc = 0;
  ecs->comp_count = 0;
  EcsAllocEntities(ecs, max_entities);
  EcsAllocSystems(ecs);
  return ecs;
}

void EcsFree(ECS *ecs) {
  EcsFreeSystems(ecs);
  EcsFreeComponents(ecs);
  EcsFreeEntities(ecs);
  free(ecs);
}

// ######## //
//  ENTITY  //
// ######## //

void EcsAllocEntities(ECS *ecs, Entity max_entities) {
  ecs->max_entities = max_entities;
  ecs->entities = calloc(max_entities, sizeof(Signature));
  ecs->free_entities = calloc(max_entities, sizeof(Entity));
  ecs->entity_count = 0;
  ecs->free_count = 0;
}

void EcsFreeEntities(ECS *ecs) {
  free(ecs->entities);
  ecs->entities = NULL;
  ecs->entity_count = 0;
  free(ecs->free_entities);
  ecs->free_entities = NULL;
  ecs->free_count = 0;
}

Entity EcsEntity(ECS *ecs) {
  Entity e;
  if (ecs->free_count > 0) {
    e = ecs->free_entities[--ecs->free_count];
  } else {
    if (ecs->entity_count >= ecs->max_entities)
      return (Entity)-1;
    e = ecs->entity_count++;
  }
  assert(e < ecs->max_entities &&
         "Exceeded maximum number of entities or Entity is invalid (-1)");
  ecs->entities[e] = 0; // signature
  return e;
}

bool EcsEntityIsAlive(ECS *ecs, Entity e) {
  for (Entity i = 0; i < ecs->free_count; i++)
    if (ecs->free_entities[i] == e)
      return false;
  return true;
}

void EcsEntityFree(ECS *ecs, Entity e) {
  // Remove all components with proper cleanup
  for (Component c = 0; c < ecs->comp_count; c++)
    EcsRemoveComponent(ecs, e, c);
  ecs->entities[e] = 0; // signature // just in case

  if (ecs->free_count < ecs->max_entities)
    ecs->free_entities[ecs->free_count++] = e;
}

Entity EcsEntityCount(ECS *ecs) { return ecs->entity_count - ecs->free_count; }

void EcsForEachEntity(ECS *ecs, Script script) {
  for (Entity e = 0; e < ecs->entity_count; e++) {
    if (!EcsEntityIsAlive(ecs, e))
      continue;
    script(ecs, e);
  }
}

// ########### //
//  COMPONENT  //
// ########### //

Component EcsRegisterComponent(ECS *ecs, char *name, size_t size,
                               void (*dtor)(void *)) {
  // maximum number of components for signatures
  if (ecs->comp_count >= 64)
    return ecs->comp_count; // invalid ID

  Component id = ecs->comp_count;
  if (ecs->comp_count >= ecs->comp_alloc) {
    Component new_alloc = ecs->comp_alloc ? ecs->comp_alloc * 2 : 4;
    ComponentList *new_components;
    if (!ecs->components)
      new_components = malloc(sizeof(ComponentList) * new_alloc);
    else
      new_components =
          realloc(ecs->components, sizeof(ComponentList) * new_alloc);

    if (!new_components)
      return ecs->comp_count; // invalid ID

    ecs->components = new_components;
    ecs->comp_alloc = new_alloc;
  }

  // Allocate component data array
  void *component_list = calloc(ecs->max_entities, size);
  if (!component_list)
    return ecs->comp_count; // invalid ID

  // If failed no free() is needed because comp_count didn't increased.
  ecs->comp_count++;
  ecs->components[id].name = name;
  ecs->components[id].size = size;
  ecs->components[id].dtor = dtor;
  ecs->components[id].list = component_list;
  return id;
}

void EcsFreeComponents(ECS *ecs) {
  while (ecs->comp_count > 0) {
    Component id = --ecs->comp_count;

    for (Entity e = 0; e < ecs->entity_count; ++e)
      EcsRemoveComponent(ecs, e, id);

    free(ecs->components[id].list);
    ecs->components[id].list = NULL;
  }

  free(ecs->components);
  ecs->components = NULL;
  ecs->comp_alloc = 0;
}

void EcsAddComponent(ECS *ecs, Entity e, Component id, void *data) {
  if (e >= ecs->max_entities || id >= ecs->comp_count || id >= 64)
    return; // TODO: is this check really necessary??

  size_t size = ecs->components[id].size;
  void *dest = (uint8_t *)ecs->components[id].list + e * size;
  memcpy(dest, data, size);
  ecs->entities[e] |= (1ULL << id);
}

void *EcsGetComponent(ECS *ecs, Entity e, Component id) {
  if (!EcsHasComponent(ecs, e, id))
    return NULL;

  return (uint8_t *)ecs->components[id].list + e * ecs->components[id].size;
}

void EcsRemoveComponent(ECS *ecs, Entity e, Component id) {
  if (!EcsHasComponent(ecs, e, id))
    return;

  size_t size = ecs->components[id].size;
  void *dest = (uint8_t *)ecs->components[id].list + e * size;
  if (ecs->components[id].dtor)
    ecs->components[id].dtor(dest);
  memset(dest, 0, size);
  ecs->entities[e] &= ~(1ULL << id);
}

bool EcsHasComponent(ECS *ecs, Entity e, Component id) {
  if (e >= ecs->max_entities || id >= ecs->comp_count || id >= 64)
    return false;
  if ((ecs->entities[e] & (1ULL << id)) == 0)
    return false;
  return true;
}

bool EcsHasComponents(ECS *ecs, Entity e, Signature mask) {
  if (e >= ecs->max_entities)
    return false;

  return (ecs->entities[e] & mask) == mask;
}

Component EcsCID(ECS *ecs, char *name) {
  for (Component id = 0; id < ecs->comp_count; id++) {
    if (strcmp(ecs->components[id].name, name) == 0)
      return id;
  }
  return ecs->comp_count;
}

// ########### //
//  SIGNATURE  //
// ########### //

size_t split(char *str, char **out, size_t max) {
  size_t count = 0;
  char *tok = strtok(str, ",");

  while (tok && count < max) {
    while (isspace((unsigned char)*tok))
      tok++;

    char *end = tok + strlen(tok) - 1;
    while (end > tok && isspace((unsigned char)*end)) {
      *end-- = '\0';
    }

    out[count++] = tok;
    tok = strtok(NULL, ",");
  }

  return count;
}

Signature EcsSignatureImpl(ECS *ecs, const char *str) {
  Signature mask = 0;

  char buffer[256];
  strncpy(buffer, str, sizeof(buffer));
  buffer[sizeof(buffer) - 1] = '\0';

  char *components[8];
  size_t n = split(buffer, components, 8);
  for (size_t i = 0; i < n; i++) {
    Component cid = EcsCID(ecs, components[i]);
    if (cid < 64) // overflow signature bits
      mask |= (1ULL << cid);
  }
  return mask;
}

// ######### //
//  SYSTEMS  //
// ######### //

void EcsAllocSystems(ECS *ecs) {
  ecs->systems = calloc(EcsTotalLayers, sizeof(LayerSystems));
}

void EcsFreeSystems(ECS *ecs) {
  for (int i = 0; i < EcsTotalLayers; i++)
    free(ecs->systems[i].list);
  free(ecs->systems);
  ecs->systems = NULL;
}

void EcsAddSystem(ECS *ecs, Script s, EcsLayer ly, Signature mask) {
  if (ly >= EcsTotalLayers)
    return;

  size_t cur = ecs->systems[ly].size;
  if (ecs->systems[ly].size >= ecs->systems[ly].alloc) {
    size_t new_alloc = ecs->systems[ly].alloc ? ecs->systems[ly].alloc * 2 : 4;
    System *new_list;

    if (cur == 0)
      new_list = malloc(sizeof(System) * new_alloc);
    else
      new_list = realloc(ecs->systems[ly].list, sizeof(System) * new_alloc);

    if (!new_list)
      return;

    ecs->systems[ly].list = new_list;
    ecs->systems[ly].size++;
  }

  ecs->systems[ly].list[cur].run = s;
  ecs->systems[ly].list[cur].mask = mask;
}

bool EcsCanRun(ECS *ecs, System *system, Entity e, EcsLayer ly) {
  if (!EcsHasComponents(ecs, e, system->mask))
    return 0;
  if (ly < EcsOnRender)
    return EntityIsActive(ecs, e);
  else
    return EntityIsVisible(ecs, e);
}

void EcsRunSystems(ECS *ecs, EcsLayer ly) {
  size_t len = ecs->systems[ly].size;
  for (size_t s = 0; s < len; s++) {
    for (Entity e = 0; e < ecs->entity_count; e++) {
      if (EcsCanRun(ecs, &ecs->systems[ly].list[s], e, ly))
        ecs->systems[ly].list[s].run(ecs, e);
    }
  }
}
