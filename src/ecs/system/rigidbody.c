#include <ecs/component.h>
#include <ecs/system.h>

void PhysicsSystem(ECS *ecs, Entity e) {
  RigidBody *rb = EcsGet(ecs, e, RigidBody);
  Transform2 *t = EcsGet(ecs, e, Transform2);

  rb->speed.x += rb->acc.x * ECS_FIXED_DELTATIME;
  rb->speed.y += rb->acc.y * ECS_FIXED_DELTATIME;

  t->position.x += rb->speed.x * ECS_FIXED_DELTATIME;
  t->position.y += rb->speed.y * ECS_FIXED_DELTATIME;

  if (rb->damping > 0.f)
    BodyApplyDamping(rb);
}

void GravitySystem(ECS *ecs, Entity e) {
  RigidBody *rb = EcsGet(ecs, e, RigidBody);
  if (!(rb->type == RIGIDBODY_DYNAMIC && rb->gravity))
    return;

  Vector2 w = {0, 9.8f * rb->mass};
  BodyApplyForce(rb, w);
}
