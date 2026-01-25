#include <ecs/component.h>
#include <ecs/system.h>

void PhysicsSystem(ECS *ecs, Entity e) {
  RigidBody *rb = GetComponent(ecs, e, RigidBody);
  Transform2 *t = GetComponent(ecs, e, Transform2);

  rb->speed.x += rb->acc.x * FIXED_DELTATIME;
  rb->speed.y += rb->acc.y * FIXED_DELTATIME;

  t->position.x += rb->speed.x * FIXED_DELTATIME;
  t->position.y += rb->speed.y * FIXED_DELTATIME;

  if (rb->damping > 0.f)
    ApplyDamping(rb);
}

void GravitySystem(ECS *ecs, Entity e) {
  RigidBody *rb = GetComponent(ecs, e, RigidBody);
  if (!(rb->type == BODY_DYNAMIC && rb->gravity))
    return;

  Vector2 w = {0, 9.8f * rb->mass};
  ApplyForce(rb, w);
}
