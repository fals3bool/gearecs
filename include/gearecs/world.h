#ifndef GEARECS_WORLD
#define GEARECS_WORLD

/**
 * @file world.h
 * @brief High-level ECS world management for gearecs library
 *
 * This header provides convenience functions for creating and managing
 * a complete ECS world with pre-configured systems and components.
 * It offers a simplified interface for common game development workflows
 * by handling the setup of the ECS registry, camera, and main game loop.
 *
 * The world management functions include:
 * - Pre-configured ECS registry creation with camera integration
 * - Automatic system registration for built-in functionality
 * - Main game loop handling with proper phase ordering
 * - Integration with raylib's window and rendering system
 */

#include <ecs/component.h>
#include <ecs/system.h>

/**
 * Creates a pre-configured ECS world with camera support.
 *
 * This function creates a complete ECS registry instance that is ready
 * for immediate use. It automatically registers all built-in components
 * and sets up the necessary infrastructure for a typical game application.
 *
 * The world includes support for:
 * - Built-in components (Transform2, RigidBody, Collider, Behaviour, etc.)
 * - Built-in systems (physics, rendering, collision, etc.)
 * - Camera integration for 2D rendering
 *
 * @param max_entities Maximum number of entities the world can support.
 * @param camera Raylib Camera2D structure for 2D rendering viewport.
 * @return Pointer to created ECS world, or NULL on failure.
 *
 * @note Entity {id:0} is the camera. Use GetComponent(world, 0, Camera2D) to
 * retrieve the camera component.
 *
 * Example:
 * ```c
 * Camera2D camera = {0};
 * camera.target = (Vector2){0, 0};
 * camera.offset = (Vector2){GetScreenWidth() / 2, GetScreenHeight() / 2};
 * camera.rotation = 0.0f;
 * camera.zoom = 1.0f;
 *
 * ECS *world = EcsWorld(4096, camera);
 * ```
 *
 * @see EcsLoop() for running the main game loop
 * @see EcsFree() for freeing the world when done
 * @see EcsRegistry() for creating a basic empty registry
 */
ECS *EcsWorld(uint16_t max_entities, Camera2D camera);

/**
 * Runs the main ECS game loop with proper phase ordering.
 *
 * This function implements the complete game loop that processes
 * all registered systems in their correct execution phases. It handles
 * the automatic calling of systems during their proper execution layer.
 *
 * The loop continues until the window is closed.
 *
 * @param world The ECS world to run, created with EcsWorld().
 *
 * @note This function blocks until the window is closed.
 *       For custom game loop control, consider manually calling
 *       the ECS system execution functions.
 *
 * @see EcsWorld() for creating the world
 * @see EcsLayer enum for available execution phases
 */
void EcsLoop(ECS *world);

#endif
