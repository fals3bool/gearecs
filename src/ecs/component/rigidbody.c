#include <ecs/component.h>

void ApplyForce(RigidBody *rb, Vector2 force) {
  rb->acc.x += force.x / rb->mass;
  rb->acc.y += force.y / rb->mass;
}

void ApplyImpulse(RigidBody *rb, Vector2 impulse) {
  rb->speed.x += impulse.x / rb->mass;
  rb->speed.y += impulse.y / rb->mass;
}

void ApplyDamping(RigidBody *rb) {
  float fac = expf(-rb->damping * FIXED_DELTATIME);
  rb->speed.x *= fac;
  rb->speed.y *= fac;
}
