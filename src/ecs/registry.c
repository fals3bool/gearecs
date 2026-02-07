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
} ComponentData;

typedef struct {
  System *list;
  size_t size;
  size_t alloc;
} LayerSystems;

struct Registry {
  Entity max_entities;
  EntityData *entities;
  Entity entity_count;
  Entity *free_entities;
  Entity free_count;

  ComponentData *components;
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

Entity EcsEntity(ECS *ecs, char *tag) {
  Entity e;
  if (ecs->free_count > 0) {
    e = ecs->free_entities[--ecs->free_count];
  } else {
    if (ecs->entity_count >= ecs->max_entities)
      return InvalidID;
    e = ecs->entity_count++;
  }
  assert(e < ecs->max_entities && "Exceeded maximum number of entities");
  ecs->entities[e] = (EntityData){0, true, true, tag};
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
  ecs->entities[e] = (EntityData){0};

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

// ######### //
//   STATE   //
// ######### //

EntityData *EcsEntityData(ECS *ecs, Entity e) { return &ecs->entities[e]; }

Entity EntityFindByTag(ECS *ecs, char *tag) {
  for (Entity e = 0; e < ecs->comp_count; e++) {
    if (strcmp(ecs->entities[e].tag, tag) == 0)
      return e;
  }
  return InvalidID;
}

bool EntityHasTag(ECS *ecs, Entity e, char *tag) {
  assert(e < ecs->max_entities && "Invalid entity");
  return strcmp(ecs->entities[e].tag, tag) == 0;
}

void EntitySetActive(ECS *ecs, Entity e, bool active) {
  ecs->entities[e].active = active;
}

bool EntityIsActive(ECS *ecs, Entity e) { return ecs->entities[e].active; }

void EntitySetVisible(ECS *ecs, Entity e, bool visible) {
  ecs->entities[e].visible = visible;
}

bool EntityIsVisible(ECS *ecs, Entity e) { return ecs->entities[e].visible; }

// ########### //
//  COMPONENT  //
// ########### //

Component EcsRegisterComponent(ECS *ecs, char *name, size_t size,
                               void (*dtor)(void *)) {
  // maximum number of components for signatures
  if (ecs->comp_count >= 64)
    return InvalidID;

  Component id = ecs->comp_count;
  if (ecs->comp_count >= ecs->comp_alloc) {
    Component new_alloc = ecs->comp_alloc ? ecs->comp_alloc * 2 : 4;
    ComponentData *new_components;
    if (!ecs->components)
      new_components = malloc(sizeof(ComponentData) * new_alloc);
    else
      new_components =
          realloc(ecs->components, sizeof(ComponentData) * new_alloc);

    if (!new_components)
      return InvalidID;

    ecs->components = new_components;
    ecs->comp_alloc = new_alloc;
  }

  // Allocate component data array
  void *component_list = calloc(ecs->max_entities, size);
  if (!component_list)
    return InvalidID;

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
    Component id = ecs->comp_count - 1;

    for (Entity e = 0; e < ecs->entity_count; ++e)
      EcsRemoveComponent(ecs, e, id);

    free(ecs->components[id].list);
    ecs->components[id].list = NULL;
    ecs->comp_count--;
  }

  free(ecs->components);
  ecs->components = NULL;
  ecs->comp_alloc = 0;
}

void EcsAddComponent(ECS *ecs, Entity e, Component id, void *data) {
  assert(e < ecs->max_entities && "Invalid entity");
  assert(id < 64 && "Invalid component");
  assert(id < ecs->comp_count && "Component does not exist");

  size_t size = ecs->components[id].size;
  void *dest = (uint8_t *)ecs->components[id].list + e * size;
  memcpy(dest, data, size);
  ecs->entities[e].signature |= (1ULL << id);
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
  ecs->entities[e].signature &= ~(1ULL << id);
}

bool EcsHasComponent(ECS *ecs, Entity e, Component id) {
  assert(e < ecs->max_entities && "Invalid entity");
  assert(id < 64 && "Invalid component");
  assert(id < ecs->comp_count && "Component does not exist");

  if ((ecs->entities[e].signature & (1ULL << id)) == 0)
    return false;
  return true;
}

bool EcsHasComponents(ECS *ecs, Entity e, Signature mask) {
  assert(e < ecs->max_entities && "Invalid entity");

  return (ecs->entities[e].signature & mask) == mask;
}

Component EcsCID(ECS *ecs, char *name) {
  for (Component id = 0; id < ecs->comp_count; id++) {
    if (strcmp(ecs->components[id].name, name) == 0)
      return id;
  }
  return InvalidID;
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
    assert(cid < 64 && "Component not found"); // overflow signature bits
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
