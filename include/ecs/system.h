#ifndef ECS_SYSTEM_H
#define ECS_SYSTEM_H

/**
 * @file system.h
 * @brief Built-in systems for gearecs
 *
 * This header provides a comprehensive set of ready-to-use system functions
 * that work with the built-in components. These systems implement common
 * game engine functionality including:
 *
 * - Script execution and behavior management
 * - Entity hierarchy and transform propagation
 * - Collision detection and response
 * - Physics simulation (forces, gravity, damping)
 * - Sprite rendering and visualization
 * - Debug utilities
 */

#include <ecs/registry.h>

// ########### //
//  BEHAVIOUR  //
// ########### //

/**
 * System that runs Start layer scripts.
 *
 * Executes scripts registered with AddScript() for the Update layer.
 * Runs on all active entities with Behaviour components.
 *
 * Required components: Behaviour
 *
 * Usage: System(ecs, BehaviourStartSystem, EcsOnStart, Behaviour)
 */
void BehaviourStartSystem(ECS *ecs, Entity e);

/**
 * System that runs Update layer scripts.
 *
 * Executes scripts registered with AddScript() for the Update layer.
 * Runs on all active entities with Behaviour components.
 *
 * Required components: Behaviour
 *
 * Usage: System(ecs, BehaviourUpdateSystem, EcsOnUpdate, Behaviour)
 */
void BehaviourUpdateSystem(ECS *ecs, Entity e);

/**
 * System that runs LateUpdate layer scripts.
 *
 * Executes scripts registered with AddScript() for the LateUpdate layer.
 * Runs on all active entities with Behaviour components.
 *
 * Required components: Behaviour
 *
 * Usage: System(ecs, BehaviourLateSystem, EcsOnLateUpdate, Behaviour)
 */
void BehaviourLateSystem(ECS *ecs, Entity e);

/**
 * System that runs FixedUpdate layer scripts.
 *
 * Executes scripts registered with AddScript() for the FixedUpdate layer.
 * Runs on all active entities with Behaviour components.
 *
 * Required components: Behaviour
 *
 * Usage: System(ecs, BehaviourFixedSystem, EcsOnFixedUpdate, Behaviour)
 */
void BehaviourFixedSystem(ECS *ecs, Entity e);

/**
 * System that runs Render layer scripts.
 *
 * Executes scripts registered with AddScript() for the Render layer.
 * Runs on all visible entities with Behaviour components.
 *
 * Required components: Behaviour
 *
 * Usage: System(ecs, BehaviourRenderSystem, EcsOnRender, Behaviour)
 */
void BehaviourRenderSystem(ECS *ecs, Entity e);

/**
 * System that runs GUI layer scripts.
 *
 * Executes scripts registered with AddScript() for the Gui layer.
 * Runs on all visible entities with Behaviour components.
 *
 * Required components: Behaviour
 *
 * Usage: System(ecs, BehaviourGuiSystem, EcsOnGui, Behaviour)
 */
void BehaviourGuiSystem(ECS *ecs, Entity e);

// ################ //
//  TRANSFORM PLUS  //
// ################ //

/**
 * System that propagates hierarchical transforms.
 *
 * Updates world-space transforms for entities in a hierarchy by applying
 * parent transforms to child transforms. Must run before any systems
 * that depend on world-space positions.
 *
 * Required components: Transform2, Parent
 *
 * Usage: System(ecs, HierarchyTransform, EcsOnUpdate, Transform2, Parent)
 */
void HierarchyTransformSystem(ECS *ecs, Entity e);

/**
 * System that updates collider positions based on transforms.
 *
 * Synchronizes collider vertex positions with entity transform. Must run
 * after transform updates but before collision detection.
 *
 * Required components: Transform2, Collider
 *
 * Usage: System(ecs, TransformColliderSystem, EcsOnUpdate, Transform2,
 * Collider)
 */
void TransformColliderSystem(ECS *ecs, Entity e);

// ########### //
//  COLLISION  //
// ########### //

/**
 * System that performs collision detection and response.
 *
 * Detects collisions between entities with colliders and generates
 * collision events for entities with CollisionListener components.
 * Handles solid vs trigger, collision layers and event handlers.
 *
 * Required components: Collider, Transform2
 * Optional: CollisionListener (for event handling)
 * Optional: RigidBody (for newton laws based resolution)
 *
 * Usage: System(ecs, CollisionSystem, EcsOnUpdate, Transform2, Collider)
 */
void CollisionSystem(ECS *ecs, Entity e);

/**
 * System that renders debug visualization for colliders.
 *
 * Draws collider outlines, bounding boxes, and collision information.
 * Useful for debugging collision setup and issues.
 *
 * Required components: Collider, Transform2
 *
 * Usage: System(ecs, DebugColliderSystem, EcsOnRender, Collider, Transform2)
 */
void DebugColliderSystem(ECS *ecs, Entity e);

// ######### //
//  PHYSICS  //
// ######### //

/**
 * System that updates rigid body physics simulation.
 *
 * Integrates forces, applies damping, and updates velocities and positions
 * for entities with RigidBody components. Runs at fixed timestep for
 * consistent physics regardless of frame rate.
 *
 * Required components: RigidBody, Transform2
 *
 * Usage: System(ecs, PhysicsSystem, EcsOnFixedUpdate, RigidBody, Transform2)
 */
void PhysicsSystem(ECS *ecs, Entity e);

/**
 * System that applies gravity to rigid bodies.
 *
 * Applies downward gravitational force to entities with RigidBody components
 * that have gravity enabled. Runs before PhysicsSystem.
 *
 * Required components: RigidBody
 *
 * Usage: System(ecs, GravitySystem, EcsOnFixedUpdate, RigidBody)
 */
void GravitySystem(ECS *ecs, Entity e);

// ########### //
//  RENDERING  //
// ########### //

/**
 * System that renders sprite components.
 *
 * Draws textures for entities with Sprite components using their
 * Transform2 for positioning. Supports source rectangles and color tinting.
 *
 * Required components: Sprite, Transform2
 *
 * Usage: System(ecs, SpriteSystem, EcsOnRender, Sprite, Transform2)
 */
void SpriteSystem(ECS *ecs, Entity e);

#endif
