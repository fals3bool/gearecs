#include <ecs/component.h>
#include <ecs/system.h>

void TransformColliderSystem(ECS *ecs, Entity e) {
  Transform2 *t = GetComponent(ecs, e, Transform2);
  Collider *c = GetComponent(ecs, e, Collider);

  float angle = t->rotation;
  for (uint8_t i = 0; i < c->vertices; i++) {
    c->vx[i].x =
        c->md[i].x * cosf(angle) - c->md[i].y * sinf(angle) + t->position.x;
    c->vx[i].y =
        c->md[i].x * sinf(angle) + c->md[i].y * cosf(angle) + t->position.y;
  }
  c->overlap = false;
}

void DebugColliderSystem(ECS *ecs, Entity e) {
  Collider *col = GetComponent(ecs, e, Collider);
  for (uint8_t i = 0; i < col->vertices; i++) {
    Vector2 p = col->vx[i];
    Vector2 q = col->vx[(i + 1) % col->vertices];
    DrawLineEx(p, q, 2, col->overlap ? RED : SKYBLUE);
  }
}

// COLLISIONS

static uint8_t SatProj(Collider *ca, Collider *cb, float *min_distance,
                       Vector2 *axis) {
  for (uint8_t i = 0; i < ca->vertices; i++) {
    uint8_t j = (i + 1) % ca->vertices;

    float edge_x = ca->vx[j].x - ca->vx[i].x;
    float edge_y = ca->vx[j].y - ca->vx[i].y;

    float normal_x = -edge_y;
    float normal_y = edge_x;

    float length_sq = normal_x * normal_x + normal_y * normal_y;
    if (length_sq == 0.0f)
      continue;

    float inv_length = 1.0f / sqrtf(length_sq);
    normal_x *= inv_length;
    normal_y *= inv_length;

    float min_r1 = INFINITY, max_r1 = -INFINITY;
    Vector2 *v_a = ca->vx;
    for (uint8_t p = 0; p < ca->vertices; p++) {
      float projection = v_a[p].x * normal_x + v_a[p].y * normal_y;
      if (projection < min_r1)
        min_r1 = projection;
      if (projection > max_r1)
        max_r1 = projection;
    }

    float min_r2 = INFINITY, max_r2 = -INFINITY;
    Vector2 *v_b = cb->vx;
    for (uint8_t p = 0; p < cb->vertices; p++) {
      float projection = v_b[p].x * normal_x + v_b[p].y * normal_y;
      if (projection < min_r2)
        min_r2 = projection;
      if (projection > max_r2)
        max_r2 = projection;
    }

    // Check for separation - early exit if no overlap
    float overlap = (max_r1 < max_r2 ? max_r1 : max_r2) -
                    (min_r1 > min_r2 ? min_r1 : min_r2);
    if (overlap <= 0.0f)
      return false;

    // Track minimum overlap and axis
    if (overlap < *min_distance) {
      *min_distance = overlap;
      axis->x = normal_x;
      axis->y = normal_y;
    }
  }
  return true;
}

uint8_t CollisionSat(Transform2 *ta, Collider *ca, Transform2 *tb, Collider *cb,
                     Collision *output) {
  float distance = INFINITY;
  Vector2 proj = {0, 0};

  if (!SatProj(ca, cb, &distance, &proj))
    return false;

  if (!SatProj(cb, ca, &distance, &proj))
    return false;

  Vector2 delta = {tb->position.x - ta->position.x,
                   tb->position.y - ta->position.y};

  if (delta.x * proj.x + delta.y * proj.y < 0.0f) {
    proj.x = -proj.x;
    proj.y = -proj.y;
  }

  output->normal = Vector2Normalize(proj);
  output->distance = distance;

  return true;
}

void ResolveCollision(Collision *input, Transform2 *ta, RigidBody *ra,
                      Transform2 *tb, RigidBody *rb) {
  float invmassA = (ra && ra->type == BodyDynamic) ? ra->invmass : 0;
  float invmassB = (rb && rb->type == BodyDynamic) ? rb->invmass : 0;
  if (invmassA + invmassB == 0)
    return;

  float deltaMagnitude = input->distance / (invmassA + invmassB);
  Vector2 delta = Vector2Scale(input->normal, deltaMagnitude);
  ta->position = Vector2Subtract(ta->position, Vector2Scale(delta, invmassA));
  tb->position = Vector2Add(tb->position, Vector2Scale(delta, invmassB));

  Vector2 deltaSpeed = Vector2Subtract(rb ? rb->speed : (Vector2){0, 0},
                                       ra ? ra->speed : (Vector2){0, 0});
  float speedAlongNormal = Vector2DotProduct(deltaSpeed, input->normal);
  if (speedAlongNormal > 0) // rigidbodies separated
    return;

  float e = 0;
  float impulseMagnitute = -(1 + e) * speedAlongNormal / (invmassA + invmassB);
  Vector2 impulse = Vector2Scale(input->normal, impulseMagnitute);
  if (ra && ra->type == BodyDynamic)
    ra->speed = Vector2Subtract(ra->speed, Vector2Scale(impulse, invmassA));
  if (rb && rb->type == BodyDynamic)
    rb->speed = Vector2Add(rb->speed, Vector2Scale(impulse, invmassB));
}

void HandleCollisionEvents(ECS *ecs, Entity self, Entity other,
                           Collision *collision) {
  CollisionListener *listener_a = GetComponent(ecs, self, CollisionListener);
  if (listener_a) {
    CollisionEvent event = {self, other, *collision};
    listener_a->OnCollision(ecs, &event);
  }

  CollisionListener *listener_b = GetComponent(ecs, other, CollisionListener);
  if (listener_b) {
    CollisionEvent event = {
        other, self, {Vector2Negate(collision->normal), collision->distance}};
    listener_b->OnCollision(ecs, &event);
  }
}

void CollisionSystem(ECS *ecs, Entity self) {
  Transform2 *ta = GetComponent(ecs, self, Transform2);
  Collider *ca = GetComponent(ecs, self, Collider);
  RigidBody *ra = GetComponent(ecs, self, RigidBody);
  uint8_t la = EcsEntityData(ecs, self)->layer;

  Signature mask = EcsSignature(ecs, Transform2, Collider);
  for (Entity other = self + 1; other < EcsEntityCount(ecs); ++other) {
    if (!EcsHasComponents(ecs, other, mask))
      continue;
    if (!EntityIsActive(ecs, other))
      continue;

    Transform2 *tb = GetComponent(ecs, other, Transform2);
    Collider *cb = GetComponent(ecs, other, Collider);
    RigidBody *rb = GetComponent(ecs, other, RigidBody);
    uint8_t lb = EcsEntityData(ecs, other)->layer;

    Collision collision;
    uint8_t overlap =
        LayerIncludes(ecs, la, lb) && CollisionSat(ta, ca, tb, cb, &collision);
    if (overlap)
      HandleCollisionEvents(ecs, self, other, &collision);

    ca->overlap |= overlap;
    cb->overlap |= overlap;
    overlap &= ca->solid && cb->solid;

    if (overlap)
      ResolveCollision(&collision, ta, ra, tb, rb);
  }
}
