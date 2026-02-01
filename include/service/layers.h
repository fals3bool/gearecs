#ifndef SERVICE_LAYERS_H
#define SERVICE_LAYERS_H

#include <ecs/component.h>

/**
 * @file layers.h
 * @brief Layer management service for ECS used for collisions.
 * TODO: Use this layers for rendering too.
 *
 * This service provides a layer system that allows fine-grained control over
 * which entities can collide with each other. It supports up to 64 collision
 * layers and enables selective collision filtering for efficient physics
 * simulation.
 *
 * Features:
 * - Up to 64 collision layers per ECS instance
 * - Bitmask-based collision matrix for fast lookups
 * - Automatic creation of "default" and "gui" layers
 * - Runtime layer management and configuration
 * - Support for multiple ECS instances
 *
 * The layer system uses bitmasks where each bit represents whether a layer can
 * collide with another layer. This provides O(1) collision checks during
 * physics simulation.
 *
 * Usage:
 * 1. StartLayerService(ecs) - Initialize with default layers
 * 2. AddLayer(ecs, "player") - Create custom layers
 * 3. ColliderSetLayer(ecs, &collider, "player") - Assign colliders to layers
 * 4. EnableCollisionLayer(ecs, "player", "enemy") - Configure collisions
 * 5. DisableCollisionLayer(ecs, "enemy", "enemy") - Self collisions
 *
 * @note Must be started with StartLayerService() before use
 * @note Service must be stopped with StopLayerService() when done
 * @note By default every layer collides with anything, unless disabled.
 */

/**
 * @brief Initializes the layer service for an ECS instance.
 *
 * Sets up the layer management system and creates two default layers:
 * - "default": General purpose layer
 * - "gui": User interface elements layer (no collisions)
 *
 * @param ecs ECS instance to initialize the service for
 *
 * @see StopLayerService() to clean up when done
 */
void StartLayerService(ECS *ecs);

/**
 * @brief Stops and cleans up the layer service.
 *
 * Removes the ECS instance from the layer service registry and frees
 * any associated memory. All layer data for this ECS instance is lost.
 *
 * @param ecs ECS instance to stop the service for
 *
 * @see StartLayerService() to initialize the service
 */
void StopLayerService(ECS *ecs);

/**
 * @brief Adds a new layer to the ECS instance.
 *
 * Creates a new named layer. The layer is assigned the next available ID
 * (0-63).
 *
 * @param ecs ECS instance to add the layer to
 * @param name Name of the layer (must be unique per ECS instance)
 *
 * @see FindLayer() to get layer ID by name
 */
void AddLayer(ECS *ecs, char *name);

/**
 * @brief Finds the ID of a layer by its name.
 *
 * Searches for a layer with the specified name and returns its ID.
 * The ID can be used for direct layer operations and collision checks.
 *
 * Asserts if layer name is not found
 *
 * @param ecs ECS instance to search in
 * @param name Name of the layer to find
 * @return Layer ID (0-63) if found
 *
 * @see LayerIncludesLayer() for collision checking with layer IDs
 */
uint8_t FindLayer(ECS *ecs, char *name);

/**
 * @brief Assigns a collider to a specific collision layer.
 *
 * Sets the layer field of the collider component to the specified layer.
 * The collider will only collide with layers that have collision enabled.
 *
 * Layer must exist.
 *
 * @param ecs ECS instance containing the layers
 * @param collider Pointer to collider component to modify
 * @param layer Name of the layer to assign the collider to
 *
 * @see FindLayer() to check if layer exists
 */
void ColliderSetLayer(ECS *ecs, Collider *collider, char *layer);

/**
 * @brief Enables collision between two layers.
 *
 * Sets up bidirectional collision between the specified layers.
 * Colliders on either layer will detect collisions with colliders
 * on the other layer.
 *
 * Both layers must exist.
 *
 * @param ecs ECS instance containing the layers
 * @param layer1 Name of the first layer
 * @param layer2 Name of the second layer
 *
 * @see DisableCollisionLayer() to disable collisions
 * @see LayerIncludesLayer() to check if collisions are enabled
 */
void EnableCollisionLayer(ECS *ecs, char *layer1, char *layer2);

/**
 * @brief Disables collision between two layers.
 *
 * Removes bidirectional collision between the specified layers.
 * Colliders on either layer will no longer detect collisions with
 * colliders on the other layer.
 *
 * Both layers must exist.
 *
 * @param ecs ECS instance containing the layers
 * @param layer1 Name of the first layer
 * @param layer2 Name of the second layer
 *
 * @see EnableCollisionLayer() to enable collisions
 * @see CleanupCollisionLayer() to remove all collisions for a layer
 */
void DisableCollisionLayer(ECS *ecs, char *layer1, char *layer2);

/**
 * @brief Removes all collision rules for a specific layer.
 *
 * Clears the collision mask for the specified layer, removing all
 * collision relationships with other layers. Also removes this layer
 * from all other layers' collision masks.
 *
 * The layer still exists but no longer collides with anything
 *
 * @param ecs ECS instance containing the layer
 * @param layer Name of the layer to clean up
 *
 * @see EnableCollisionLayer() to re-establish specific collisions
 * @see DisableCollisionLayer() to disable specific layer pairs
 */
void CleanupCollisionLayer(ECS *ecs, char *layer);

/**
 * @brief Checks if two layers can collide with each other.
 *
 * Uses the layer IDs to check if collision is enabled between them.
 * This is the fast path for collision checking using pre-resolved IDs.
 *
 * True if any layer doesn't exist. Assumes it can collide with anything.
 *
 * @param ecs ECS instance containing the layers
 * @param layer1 ID of the first layer (0-63)
 * @param layer2 ID of the second layer (0-63)
 * @return true if the layers can collide, false otherwise
 *
 * @see FindLayer() to convert layer names to IDs
 */
bool LayerIncludesLayer(ECS *ecs, uint8_t layer1, uint8_t layer2);

/**
 * @brief Prints debug information about all collision layers.
 *
 * Outputs the name and collision mask for each layer in the ECS instance.
 * Useful for debugging collision configurations and layer relationships.
 *
 * @param ecs ECS instance to debug
 */
void DebugCollisionLayers(ECS *ecs);

#endif
