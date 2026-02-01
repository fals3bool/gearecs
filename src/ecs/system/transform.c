#include <ecs/component.h>
#include <ecs/system.h>

void HierarchyTransformSystem(ECS *ecs, Entity e) {
  Parent *p = GetComponent(ecs, e, Parent);
  Transform2 *t = GetComponent(ecs, e, Transform2);
  Transform2 *tp = GetComponent(ecs, p->entity, Transform2);
  if (!tp)
    return;
  t->position = Vector2Add(tp->position, t->localPosition);
  t->scale =
      (Vector2){tp->scale.x * t->localScale.x, tp->scale.y * t->localScale.y};
  t->rotation = tp->rotation + t->localRotation;
}
