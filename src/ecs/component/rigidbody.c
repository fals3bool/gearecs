#include <ecs/component.h>

void BodyApplyForce(RigidBody *rb, Vector2 force) {
  rb->acc.x += force.x / rb->mass;
  rb->acc.y += force.y / rb->mass;
}

void BodyApplyImpulse(RigidBody *rb, Vector2 impulse) {
  rb->speed.x += impulse.x / rb->mass;
  rb->speed.y += impulse.y / rb->mass;
}

void BodyApplyDamping(RigidBody *rb) {
  float fac = expf(-rb->damping * ECS_FIXED_DELTATIME);
  rb->speed.x *= fac;
  rb->speed.y *= fac;
}
