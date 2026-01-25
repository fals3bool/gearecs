#include <ecs/component.h>
#include <ecs/system.h>

#include <stdio.h>

void printTree(ECS *ecs, Entity e) { printf("^\n[%d]\n", e); }

void printHierarchy(ECS *ecs, Entity e) {
  Parent *parent = GetComponent(ecs, e, Parent);
  Children *children = GetComponent(ecs, e, Children);
  printf("Hierarchy Relations of [Entity: %d] {%s}\n", e,
         EntityIsActive(ecs, e) ? "ACTIVE" : "NOT ACTIVE");
  if (parent)
    printf("Parent -> %d\n", parent->entity);
  if (children) {
    printf("Children:\n");
    for (Entity e = 0; e < children->count; e++)
      printf(" -> %d\n", children->list[e]);
  }

  Transform2 *t = GetComponent(ecs, e, Transform2);
  if (t)
    printf("Position: {%.2f, %.2f}\n", t->position.x, t->position.y);

  printf("\n");
}

int main(void) {

  ECS *ecs = EcsRegistry(32);
  Component(ecs, Parent);
  Component(ecs, Children);
  Component(ecs, EntityData);
  Component(ecs, Transform2);

  System(ecs, HierarchyTransform, 0, Transform2, Children);
  SystemGlobal(ecs, printHierarchy, 0);

  Entity A = EcsEntity(ecs);
  Entity B = EcsEntity(ecs);
  Entity C = EcsEntity(ecs);

  AddComponent(ecs, A, EntityData, EntityDataActive("A"));
  AddComponent(ecs, B, EntityData, EntityDataActive("B"));

  AddComponent(ecs, A, Transform2, TransformLocalPos(20, 30));
  AddComponent(ecs, C, Transform2, TransformPos(20, 30));

  Entity f = FindByTag(ecs, "B");
  printf("found: {id: %d}\n", f);
  f = FindByTag(ecs, "A");
  printf("found: {id: %d}\n", f);

  EntityAddChild(ecs, B, A);
  EntityAddChild(ecs, B, C);

  EntityAddChild(ecs, A, C); // remove child from B, move to A
  EntityAddChild(ecs, C, C); // cannot
  EntityAddChild(ecs, C, A); // cannot
  EntityAddChild(ecs, C, B); // cannot

  EntityAddParent(ecs, C, A); // already done
  EntityAddParent(ecs, A, B); // already done
  EntityAddParent(ecs, A, C); // cannot
  EntityAddParent(ecs, C, B); // remove child from A, move to B
  EntityAddParent(ecs, A, C); // remove child from B, move to C
  EntityAddParent(ecs, B, A); // cannot

  EntitySetActive(ecs, C, false); // C will still be active because it does not
                                  // have EntityData and parent (B) is active

  // Deactivated entities won't be read by systems
  EntitySetActive(ecs, B, true); // All entities are now active

  RunSystem(ecs, 0);

  printf("for each:\n");
  EntityForEachChild(ecs, B, printTree);
  printf("for each recursive:\n");
  EntityForEachChildRecursive(ecs, B, printTree);

  printf("Destroy B id:%d\n\n", B);
  EntityDestroy(ecs, B); // destroy B, remove parent from C
  // EntityDestroyRecursive(ecs, C); // destroy C and children...

  RunSystem(ecs, 0);

  return 0;
}
