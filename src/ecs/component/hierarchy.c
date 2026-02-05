#include <ecs/component.h>

#include <stdlib.h>

// Every parent operation will call the children operation with swapped entities
// and change the parent component.
//
// The children operation is the one with the logic.

bool EntitySetParent(ECS *ecs, Entity e, Entity p) {
  Parent *old = GetComponent(ecs, e, Parent);
  if (old && old->entity == p)
    return true;

  if (old)
    EntityRemoveChild(ecs, old->entity, e);
  AddComponent(ecs, e, Parent, {p});
  return true;
}

bool EntitySetChild(ECS *ecs, Entity e, Entity c) {
  // E child of C child of E -> loop! (recursively)
  Parent *parent = GetComponent(ecs, e, Parent);
  while (parent) {
    if (parent->entity == c)
      return false;
    parent = GetComponent(ecs, parent->entity, Parent);
  }

  Children *children = GetComponent(ecs, e, Children);
  if (children) {
    // Was already added?
    for (Entity i = 0; i < children->count; i++)
      if (children->list[i] == c)
        return false;

    // add to existing component, realloc if necessary
    if (children->count >= children->allocated) {
      size_t new_allocated = children->allocated ? children->allocated * 2 : 4;
      Entity *new_list =
          realloc(children->list, sizeof(Entity) * new_allocated);
      if (!new_list) {
        return false; // Reallocation failed
      }
      children->list = new_list;
      children->allocated = new_allocated;
    }
    children->list[children->count++] = c;
  } else {
    // add within a new component instance
    Entity *list = malloc(sizeof(Entity));
    if (!list)
      return false;
    list[0] = c;
    AddComponent(ecs, e, Children, {list, 1, 1});
  }

  // Hierarchical entitydata states
  EntitySetActive(ecs, c, EntityIsActive(ecs, e));
  EntitySetVisible(ecs, c, EntityIsVisible(ecs, e));
  return true;
}

void EntityAddParent(ECS *ecs, Entity e, Entity p) {
  EntityAddChild(ecs, p, e);
}

void EntityAddChild(ECS *ecs, Entity e, Entity c) {
  if (e == c)
    return;
  if (EntitySetChild(ecs, e, c))
    EntitySetParent(ecs, c, e);
}

void EntityRemoveParent(ECS *ecs, Entity e) {
  Parent *parent = GetComponent(ecs, e, Parent);
  if (!parent)
    return;
  EntityRemoveChild(ecs, parent->entity, e);
}

void EntityRemoveChild(ECS *ecs, Entity e, Entity c) {
  Children *children = GetComponent(ecs, e, Children);
  if (!children || children->count <= 0)
    return;
  // replaced by last one
  // if count == 1 replaced by itself
  // ALWAYS count--; so id is never used until reasigned again.
  for (Entity i = 0; i < children->count; i++) {
    if (children->list[i] == c)
      children->list[i] = children->list[--children->count];
  }

  RemoveComponent(ecs, c, Parent);
  if (children->count == 0)
    RemoveComponent(ecs, e, Children);
}

void EntityDestroy(ECS *ecs, Entity e) {
  EntityRemoveParent(ecs, e);

  Children *children = GetComponent(ecs, e, Children);
  if (children)
    for (Entity i = 0; i < children->count; i++)
      EntityRemoveParent(ecs, children->list[i]);

  EcsEntityFree(ecs, e);
}

void EntityDestroyRecursive(ECS *ecs, Entity e) {
  EntityRemoveParent(ecs, e);

  Children *children = GetComponent(ecs, e, Children);
  if (children)
    for (Entity i = 0; i < children->count; i++)
      EntityDestroyRecursive(ecs, children->list[i]);

  EcsEntityFree(ecs, e);
}

void EntityForEachChild(ECS *ecs, Entity e, Script s) {
  Children *children = GetComponent(ecs, e, Children);
  if (!children)
    return;
  for (Entity i = 0; i < children->count; i++)
    s(ecs, children->list[i]);
}

void EntityForEachChildRecursive(ECS *ecs, Entity e, Script s) {
  Children *children = GetComponent(ecs, e, Children);
  if (!children)
    return;
  for (Entity i = 0; i < children->count; i++) {
    s(ecs, children->list[i]);
    EntityForEachChildRecursive(ecs, children->list[i], s);
  }
}

void ChildrenDestructor(void *self) {
  Children *children = (Children *)self;
  if (children && children->list) {
    free(children->list);
    children->list = NULL;
    children->count = 0;
    children->allocated = 0;
  }
}
