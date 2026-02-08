#include <ecs/registry.h>

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  Component id;
  char *name;
} ComponentID;

typedef struct {
  void *list;
  size_t size;
  void (*dtor)(void *);
} ComponentData;

typedef struct {
  System *list;
  size_t size;
  size_t alloc;
} LayerSystems;

typedef struct {
  char *name;
  Signature mask;
} Layer;

typedef struct {
  Entity *entities;
  Entity count;
  Entity alloc;
} LayerEntities;

struct Registry {
  Entity max_entities;
  EntityData *entities;
  Entity entity_count;
  Entity *free_entities;
  Entity free_count;

  ComponentID *search;
  ComponentData *components;
  Component comp_count;
  Component comp_alloc;

  LayerSystems *systems;

  Layer *layers;
  LayerEntities *render;
  uint8_t layer_count;
};

void EcsAllocSystems(ECS *ecs);
void EcsFreeSystems(ECS *ecs);

void EcsAllocEntities(ECS *ecs, Entity max_entities);
void EcsFreeEntities(ECS *ecs);

void EcsFreeComponents(ECS *ecs);
void EcsFreeLayers(ECS *ecs);

ECS *EcsRegistry(uint16_t max_entities) {
  ECS *ecs = malloc(sizeof(ECS));
  ecs->layers = NULL;
  ecs->layer_count = 0;
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
  EcsFreeLayers(ecs);
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
  ecs->render = NULL;
}

void EcsFreeEntities(ECS *ecs) {
  free(ecs->entities);
  ecs->entities = NULL;
  ecs->entity_count = 0;
  free(ecs->free_entities);
  ecs->free_entities = NULL;
  ecs->free_count = 0;
}

void AddEntityToLayer(ECS *ecs, Entity e, uint8_t ly);
void RemoveEntityFromLayer(ECS *ecs, Entity e);

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
  ecs->entities[e] = (EntityData){0, true, true, tag, 0};
  AddEntityToLayer(ecs, e, 0);
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
  RemoveEntityFromLayer(ecs, e);
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

int comp(const void *a, const void *b) {
  return strcmp(((ComponentID *)a)->name, ((ComponentID *)b)->name);
}

Component EcsComponent(ECS *ecs, char *name, size_t size,
                       void (*dtor)(void *)) {
  // maximum number of components for signatures
  if (ecs->comp_count >= 64)
    return InvalidID;

  Component id = ecs->comp_count;
  if (ecs->comp_count >= ecs->comp_alloc) {
    Component new_alloc = ecs->comp_alloc ? ecs->comp_alloc * 2 : 4;
    ComponentData *new_components;
    ComponentID *new_search;
    if (!ecs->components) {
      new_components = malloc(sizeof(ComponentData) * new_alloc);
      new_search = malloc(sizeof(ComponentID) * new_alloc);
    } else {
      new_components =
          realloc(ecs->components, sizeof(ComponentData) * new_alloc);
      new_search = realloc(ecs->search, sizeof(ComponentID) * new_alloc);
    }

    if (!new_components || !new_search)
      return InvalidID;

    ecs->components = new_components;
    ecs->search = new_search;
    ecs->comp_alloc = new_alloc;
  }

  // Allocate component data array
  void *component_list = calloc(ecs->max_entities, size);
  if (!component_list)
    return InvalidID;

  // If failed no free() is needed because comp_count didn't increased.
  ecs->comp_count++;
  ecs->components[id].size = size;
  ecs->components[id].dtor = dtor;
  ecs->components[id].list = component_list;

  ecs->search[id].id = id;
  ecs->search[id].name = name;
  qsort(ecs->search, ecs->comp_count, sizeof(ComponentID), comp);

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
  free(ecs->search);
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

Component EcsComponentID(ECS *ecs, char *name) {
  Component a = 0, b = ecs->comp_count - 1;
  while (a <= b) {
    Component k = (a + b) / 2;
    int c = strcmp(ecs->search[k].name, name);
    if (c == 0)
      return ecs->search[k].id;

    if (c > 0)
      b = k - 1;
    else
      a = k + 1;
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
    Component cid = EcsComponentID(ecs, components[i]);
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

void EcsRunSystems(ECS *ecs, EcsLayer ly) {
  size_t len = ecs->systems[ly].size;
  System *list = ecs->systems[ly].list;

  if (ly < EcsOnRender) {
    for (size_t s = 0; s < len; s++) {
      for (Entity e = 0; e < ecs->entity_count; e++) {
        if (EcsHasComponents(ecs, e, list[s].mask) && EntityIsActive(ecs, e))
          list[s].run(ecs, e);
      }
    }
    return;
  }

  // for rendering systems
  for (size_t s = 0; s < len; s++) {
    for (uint8_t l = 0; l < ecs->layer_count; l++) {
      for (Entity i = 0; i < ecs->render[l].count; i++) {
        Entity e = ecs->render[l].entities[i];
        if (EcsHasComponents(ecs, e, list[s].mask) && EntityIsVisible(ecs, e))
          list[s].run(ecs, e);
      }
    }
  }
}

// ######## //
//  LAYERS  //
// ######## //

uint8_t LayerIndex(ECS *ecs, char *name) {
  for (int i = 0; i < ecs->layer_count; i++)
    if (strcmp(ecs->layers[i].name, name) == 0)
      return i;
  assert(!"Layer does not exist!");
  return ecs->layer_count;
}

void AddLayer(ECS *ecs, char *name) {
  uint8_t new_count = ecs->layer_count + 1;
  Layer *new_layers;
  LayerEntities *new_entities;
  if (!ecs->layers) {
    new_layers = malloc(sizeof(Layer) * new_count);
    new_entities = malloc(sizeof(LayerEntities) * new_count);
  } else {
    new_layers = realloc(ecs->layers, sizeof(Layer) * new_count);
    new_entities = realloc(ecs->render, sizeof(LayerEntities) * new_count);
  }

  if (!new_layers || !new_entities)
    return;
  ecs->layers = new_layers;
  ecs->render = new_entities;

  ecs->render[ecs->layer_count].entities = NULL;
  ecs->render[ecs->layer_count].count = 0;
  ecs->render[ecs->layer_count].alloc = 0;
  ecs->layers[ecs->layer_count].name = name;
  ecs->layers[ecs->layer_count].mask = (Signature)-1; // enable all
  ecs->layer_count++;
}

void EcsFreeLayers(ECS *ecs) {
  if (ecs->layers)
    free(ecs->layers);
  ecs->layers = NULL;
  ecs->layer_count = 0;
}

void EntitySetLayer(ECS *ecs, Entity e, char *layer) {
  RemoveEntityFromLayer(ecs, e);
  uint8_t ly = LayerIndex(ecs, layer);
  ecs->entities[e].layer = ly;
  AddEntityToLayer(ecs, e, ly);
}

void AddEntityToLayer(ECS *ecs, Entity e, uint8_t ly) {
  if (!ecs->render || ecs->layer_count <= ly)
    return;
  if (ecs->render[ly].count >= ecs->render[ly].alloc) {
    Entity *new_list;
    Entity new_alloc = ecs->render[ly].alloc ? ecs->render[ly].alloc * 2 : 16;
    if (!ecs->render[ly].entities)
      new_list = malloc(sizeof(Entity) * new_alloc);
    else
      new_list = realloc(ecs->render[ly].entities, sizeof(Entity) * new_alloc);

    if (!new_list)
      return;
    ecs->render[ly].entities = new_list;
  }
  ecs->render[ly].entities[ecs->render[ly].count++] = e;
}

void RemoveEntityFromLayer(ECS *ecs, Entity e) {
  uint8_t ly = ecs->entities[e].layer;
  if (!ecs->render || ecs->layer_count <= ly || !ecs->render[ly].entities)
    return;
  Entity last = ecs->render[ly].count - 1;
  for (Entity i = last; i >= 0; i--) {
    if (ecs->render[ly].entities[i] == e) {
      ecs->render[ly].entities[i] = ecs->render[ly].entities[last];
      ecs->render[ly].count--;
      return;
    }
  }
}

void LayerEnable(ECS *ecs, char *layer1, char *layer2) {
  uint8_t ly1 = LayerIndex(ecs, layer1);
  uint8_t ly2 = LayerIndex(ecs, layer2);
  ecs->layers[ly1].mask |= (1ULL << ly2);
  ecs->layers[ly2].mask |= (1ULL << ly1);
}

void LayerDisable(ECS *ecs, char *layer1, char *layer2) {
  uint8_t ly1 = LayerIndex(ecs, layer1);
  uint8_t ly2 = LayerIndex(ecs, layer2);
  ecs->layers[ly1].mask &= ~(1ULL << ly2);
  ecs->layers[ly2].mask &= ~(1ULL << ly1);
}

void LayerDisableAll(ECS *ecs, char *layer) {
  uint8_t ly = LayerIndex(ecs, layer);
  ecs->layers[ly].mask = 0;
}

bool LayerIncludes(ECS *ecs, uint8_t layer1, uint8_t layer2) {
  Signature mask = (1 << layer2);
  return (ecs->layers[layer1].mask & mask) == mask;
}
