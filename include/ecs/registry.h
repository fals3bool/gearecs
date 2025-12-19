#ifndef ECS_REGISTRY_H
#define ECS_REGISTRY_H

#include <stdint.h>

typedef struct Registry Registry;

#define MAX_ENTITIES 1024
typedef uint16_t Entity;

Registry *ecs_registry();
void ecs_registry_free(Registry *r);

Entity ecs_entity(Registry *r);
void ecs_entity_destroy(Registry *r, Entity e);

#endif
