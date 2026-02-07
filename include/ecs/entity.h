#ifndef ECS_ENTITY_H
#define ECS_ENTITY_H

#include <stdbool.h>
#include <stdint.h>

/**
 * An ID is a unique identifier for entities and components.
 *
 * Uses uint16_t to support up to 65536 unique entities/components.
 * Used throughout the system for efficient memory usage and fast lookups.
 */
typedef uint16_t EcsID;

/**
 * Returned by the api after an error.
 *
 * ID = 65536. Registry never reaches this number during a normal lifecycle.
 */
#define InvalidID ((EcsID) - 1)

/**
 * An entity represents a general purpose object in the game world.
 *
 * Entities are defined by their components - an entity with no components
 * still exists but has no behavior or properties. Entities are lightweight
 * and can be created/destroyed frequently.
 *
 * @note Entity IDs are recycled after destruction for efficiency
 * @see Signature for component filtering
 * @see EcsEntity() to create entities
 * @see EcsEntityFree() to destroy an entity
 */
typedef EcsID Entity;

/**
 * Signatures are bitmasks that refer to sets of components.
 *
 * Used by systems to filter entities based on their component composition.
 * Each bit represents a component ID.
 *
 * Example: The signature 0b10011 (19) represents entities with components [0,
 * 1, 4].
 *
 * Supports up to 64 component types.
 *
 * @see EcsSignature() to create signatures from a list of components
 * @see Component() for component ID registration
 */
typedef uint64_t Signature;

/**
 * Entity metadata and state management component.
 *
 * Provides entity tagging, activity/visibility control, and component
 * signature information for entity management and system filtering.
 * 
 * @note This component is automatically added to entities created with EcsEntity()
 * @see EntitySetActive() to control activity state
 * @see EntitySetVisible() to control visibility state
 * @see EntityFindByTag() to find entities by tag
 */
typedef struct {
  Signature signature; ///< Component signature bitmask for system filtering
  bool active;         ///< Whether entity participates in Update systems
  bool visible;        ///< Whether entity participates in Render systems
  char *tag;          ///< Entity identifier string for lookup and debugging
} EntityData;

#endif
