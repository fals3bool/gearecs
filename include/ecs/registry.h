#ifndef ECS_REGISTRY_H
#define ECS_REGISTRY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @file registry.h
 * @brief Core ECS (Entity Component System) Registry for gearecs library
 *
 * This header provides the main ECS registry implementation for gearecs,
 * a lightweight C library for entity component system architecture.
 *
 * The ECS pattern consists of:
 * - **Entities**: Unique identifiers (IDs) representing game objects
 * - **Components**: Data containers that define entity properties
 * - **Systems**: Logic that processes entities with specific component
 * combinations
 *
 * This registry provides:
 * - EcsID type to identify entities and components
 * - User-friendly CRUD API for entities and components
 * - Component registration and management with automatic memory layout
 * - System execution layers (Update, FixedUpdate, Render, etc.)
 * - Signatures (bitmasks) for component filtering
 */

/**
 * An ECS Registry is used to centralize entities, components and systems.
 *
 * The registry acts as the main container for all ECS operations and manages
 * the memory and lifecycle of entities, components, and systems.
 *
 * Created with EcsRegistry() and must be freed with EcsFree().
 *
 * @see EcsRegistry()
 * @see EcsFree()
 * @see EcsWorld() for a pre-configured registry
 */
typedef struct Registry ECS;

/**
 * An ID is a unique identifier for entities and components.
 *
 * Uses uint16_t to support up to 65536 unique entities/components.
 * Used throughout the system for efficient memory usage and fast lookups.
 */
typedef uint16_t EcsID;

/**
 * Maximum ID value, never reached during a normal lifecycle.
 *
 * Returned by the api after an error.
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
 * Components define entities by storing data that represents specific aspects.
 *
 * A component represents one aspect of an entity (position, velocity, health,
 * etc.).
 *
 * Components must be registered before use with Component()
 *
 * @see Component() for registration
 * @see AddComponent() to attach component to entities
 * @see RemoveComponent() to remove component from entities
 */
typedef EcsID Component;

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
 * Script function type for entity-specific logic execution.
 *
 * Scripts are functions that operate on a single entity and are called
 * by systems during their respective execution phases.
 *
 * @param ecs The ECS registry pointer
 * @param entity The entity being processed
 *
 * @see System
 * @see EcsForEachEntity()
 */
typedef void (*Script)(ECS *, Entity);

/**
 * A system processes all entities that contain the desired components.
 *
 * Systems combine a script function with a component signature filter.
 * During execution, the system iterates over all entities and calls the
 * script function only for entities matching the signature.
 *
 * Systems are organized into layers (Update, FixedUpdate, Render, etc.)
 * and can target entities based on their active/visible state.
 *
 * @see System() macro to create systems
 * @see EcsLayer for system execution phases
 */
typedef struct {
  Script run;     ///< Function to execute for matching entities
  Signature mask; ///< Component signature filter
} System;

/**
 * Creates a new ECS registry with specified entity capacity.
 *
 * Allocates memory for entities, components, and systems. The registry
 * manages all ECS operations and must be freed with EcsFree() when done.
 *
 * @param max_entities Maximum number of entities that can be created
 * @return Pointer to new ECS registry, NULL on allocation failure
 *
 * @see EcsFree() to clean up registry
 */
ECS *EcsRegistry(uint16_t max_entities);

/**
 * Destroys an ECS registry and frees all allocated memory.
 *
 * Cleans up all entities, components, systems, and internal data structures.
 * All pointers to components become invalid after this call.
 *
 * @param ecs Registry to destroy
 *
 * @see ComponentDtor() for internal data cleanup
 */
void EcsFree(ECS *ecs);

// ######## //
//  ENTITY  //
// ######## //

/**
 * Creates a new entity in the registry.
 *
 * Returns a unique entity ID. Entity IDs are recycled after destruction
 * for memory efficiency. New entities start with no components.
 *
 * @param ecs Registry to create entity in
 * @return New entity ID
 *
 * @see EcsEntityFree() to destroy entities
 * @see AddComponent() to add components to entities
 */
Entity EcsEntity(ECS *ecs);

/**
 * Checks if an entity is still alive (not destroyed).
 *
 * @param ecs Registry containing the entity
 * @param e Entity ID to check
 * @return true if entity is alive, false if destroyed
 */
bool EcsEntityIsAlive(ECS *ecs, Entity e);

/**
 * Destroys an entity and removes all its components.
 *
 * The entity ID becomes available for reuse by future EcsEntity() calls.
 * All components are properly removed with destructors if registered.
 *
 * @param ecs Registry containing the entity
 * @param e Entity ID to destroy
 *
 * @see EntityDestroy() for hierarchical destruction
 * @see EntityDestroyRecursive() for recursive destruction
 */
void EcsEntityFree(ECS *ecs, Entity e);

/**
 * Gets the current number of alive entities in the registry.
 *
 * @param ecs Registry to query
 * @return Count of active (non-destroyed) entities
 */
Entity EcsEntityCount(ECS *ecs);

/**
 * Executes a script function on every alive entity.
 *
 * Iterates over all entities and calls the provided script for each one.
 *
 * It differs from systems in that it executes logic without a component
 * signature.
 *
 * Systems are preffered for ECS standard architecture. This brings a
 * possibility for special cases.
 *
 * @param ecs Registry to iterate over
 * @param script Function to call for each entity
 */
void EcsForEachEntity(ECS *ecs, Script script);

// ######### //
//   STATE   //
// ######### //

/**
 * Sets whether an entity is active for system processing.
 *
 * Inactive entities are ignored by Update/FixedUpdate/LateUpdate systems
 * but still participate in Render systems unless visibility is also disabled.
 *
 * @param ecs Registry containing the entity
 * @param e Entity to modify
 * @param active boolean
 *
 * @see EntityIsActive() to check state
 * @see EntitySetVisible() for render control
 */
void EntitySetActive(ECS *ecs, Entity e, bool active);

/**
 * Checks if an entity is active for Update systems.
 *
 * @param ecs Registry containing the entity
 * @param e Entity to check
 * @return true if active, false otherwise
 */
bool EntityIsActive(ECS *ecs, Entity e);

/**
 * Sets whether an entity is visible for rendering.
 *
 * Invisible entities are ignored by Render and Gui systems but still
 * participate in Update systems unless activity is also disabled.
 *
 * @param ecs Registry containing the entity
 * @param e Entity to modify
 * @param visible boolean
 *
 * @see EntityIsVisible() to check state
 * @see EntitySetActive() for update control
 */
void EntitySetVisible(ECS *ecs, Entity e, bool visible);

/**
 * Checks if an entity is visible for Render systems.
 *
 * @param ecs Registry containing the entity
 * @param e Entity to check
 * @return true if visible, false otherwise
 */
bool EntityIsVisible(ECS *ecs, Entity e);

// ########### //
//  COMPONENT  //
// ########### //

/**
 * Registers a new component.
 *
 * Components must be registered before they can be used with entities.
 * The macro automatically handles component naming and size calculation.
 *
 * @param ecs Registry to register component in
 * @param C Component type name (struct type)
 * @return Component ID: EcsID
 *
 * Example: Component(world, Position);
 */
#define Component(ecs, C) EcsRegisterComponent(ecs, #C, sizeof(C), NULL)

/**
 * Registers a new component with destructor function for component cleanup.
 *
 * Useful for components that allocate dynamic memory or need cleanup.
 *
 * @param ecs Registry containing the component
 * @param C Component type name
 * @param dtor Destructor function pointer
 *
 * @note When creating a cleanup function, never free the component itself
 *
 * Example: ComponentDynamic(world, TextureData, freeTexture);
 */
#define ComponentDynamic(ecs, C, dtor)                                         \
  EcsRegisterComponent(ecs, #C, sizeof(C), dtor)

/**
 * Adds a component to an entity with initialization values.
 *
 * Creates and initializes a component instance for the specified entity.
 * Supports struct initialization syntax and pre-defined instances.
 *
 * @param ecs Registry containing the entity
 * @param entity Entity to add component to
 * @param C Component type name
 * @param ... Initialization value
 *
 * Example: AddComponent(world, player, Position, {10.0f, 20.0f});
 */
#define AddComponent(ecs, entity, C, ...)                                      \
  do {                                                                         \
    C _tmp = __VA_ARGS__;                                                      \
    EcsAddComponent(ecs, entity, EcsCID(ecs, #C), &_tmp);                      \
  } while (0)

/**
 * Retrieves a component from an entity.
 *
 * Returns a typed pointer to the component data. Returns NULL if the
 * entity doesn't have the component or the component doesn't exist.
 *
 * @param ecs Registry containing the entity
 * @param entity Entity to get component from
 * @param C Component type name
 * @return Typed pointer to component data, or NULL if not found
 *
 * Example: Position *pos = GetComponent(world, player, Position);
 */
#define GetComponent(ecs, entity, C)                                           \
  (C *)EcsGetComponent(ecs, entity, EcsCID(ecs, #C))

/**
 * Removes a component from an entity.
 *
 * Calls the component destructor if registered, then clears the component
 * data from the entity. Entity no longer has this component type.
 *
 * @param ecs Registry containing the entity
 * @param entity Entity to remove component from
 * @param C Component type name
 *
 * @see ComponentDtor()
 */
#define RemoveComponent(ecs, entity, C)                                        \
  EcsRemoveComponent(ecs, entity, EcsCID(ecs, #C))

/**
 * Gets the component ID of a specific component.
 *
 * @param ecs Registry to query
 * @param C Component type name
 * @return Component ID: EcsID
 */
#define ComponentID(ecs, C) EcsCID(ecs, #C)

/**
 * Registers a component type manually.
 *
 * Low-level function used by the Component() macro. Registers component
 * metadata and allocates storage for all entities with a destructor function
 * for cleanup.
 *
 * @param ecs Registry to register component in
 * @param name Component type name for identification
 * @param size Size of component structure in bytes
 * @param dtor A destructor function
 * @return Component ID: EcsID
 */
Component EcsRegisterComponent(ECS *ecs, char *name, size_t size,
                               void (*dtor)(void *));

/**
 * Adds component data to an entity using raw data pointer.
 *
 * Low-level function used by the AddComponent() macros. Copies component
 * data from the provided pointer to the entity's component storage.
 *
 * @param ecs Registry containing the entity
 * @param e Entity to add component to
 * @param id Component ID from EcsRegisterComponent()
 * @param data Pointer to component data to copy
 */
void EcsAddComponent(ECS *ecs, Entity e, Component id, void *data);

/**
 * Removes a component from an entity using component ID.
 *
 * Low-level function used by the RemoveComponent() macro. Calls destructor
 * if registered and clears component data.
 *
 * @param ecs Registry containing the entity
 * @param e Entity to remove component from
 * @param id Component ID to remove
 */
void EcsRemoveComponent(ECS *ecs, Entity e, Component id);

/**
 * Gets component data from an entity using component ID.
 *
 * Low-level function used by the GetComponent() macro. Returns pointer
 * to component data or NULL if not found.
 *
 * @param ecs Registry containing the entity
 * @param e Entity to get component from
 * @param id Component ID to retrieve
 * @return Pointer to component data, or NULL if not found
 */
void *EcsGetComponent(ECS *ecs, Entity e, Component id);

/**
 * @brief Checks if an entity has the specified component.
 *
 * Used by the ecs core before removing or cleaning up.
 *
 * @param ecs Registry containing the entity
 * @param e Entity to check
 * @param id Component ID
 * @return true if entity has the component, false otherwise.
 */
bool EcsHasComponent(ECS *ecs, Entity e, Component id);

/**
 * @brief Checks if an entity has all components specified in a signature.
 *
 * Used by systems to filter entities. Returns true only if the entity
 * has ALL components in the signature mask.
 *
 * @param ecs Registry containing the entity
 * @param e Entity to check
 * @param mask Component signature to test against
 * @return true if entity has all components, false otherwise
 */
bool EcsHasComponents(ECS *ecs, Entity e, Signature mask);

/**
 * Finds component ID by component name.
 *
 * Performs linear search through registered components. Returns component
 * count if name not found (invalid ID).
 *
 * @param ecs Registry to search
 * @param name Component type name to find
 * @return Component ID, or max number of components if not found.
 */
Component EcsCID(ECS *ecs, char *name);

// ######### //
//  SYSTEMS  //
// ######### //

/**
 * Fixed timestep for physics and consistent gameplay.
 *
 * Used by FixedUpdate systems to ensure deterministic physics simulation
 * regardless of frame rate variations.
 */
#define FIXED_DELTATIME 1.f / 60.f

/**
 * System execution layers define when systems run in the game loop.
 *
 * Systems are organized into logical phases that run in order:
 * 1. Start - Runs once at initialization
 * 2. Update - Runs every frame for gameplay logic
 * 3. LateUpdate - Runs after Update for post-processing
 * 4. FixedUpdate - Runs at fixed interval for physics
 * 5. Render - Runs for drawing operations
 * 6. Gui - Runs for UI overlay rendering
 */
typedef enum {
  EcsOnStart = 0,   ///< Initialization systems (run once)
  EcsOnUpdate,      ///< Frame-dependent gameplay logic
  EcsOnLateUpdate,  ///< Post-update processing
  EcsOnFixedUpdate, ///< Fixed timestep physics/consistent logic
  EcsOnRender,      ///< Rendering operations
  EcsOnGui,         ///< UI and overlay rendering
  EcsTotalLayers    ///< Total number of system layers
} EcsLayer;

/**
 * Creates a component signature from component type names.
 *
 * Macro that converts a comma-separated list of component names into
 * a signature bitmask for system filtering.
 *
 * @param ecs Registry containing registered components
 * @param ... Component type names (comma-separated, no quotes)
 * @return Signature bitmask
 *
 * Example: EcsSignature(ecs, Position, Speed, Health)
 */
#define EcsSignature(ecs, ...) EcsSignatureImpl(ecs, #__VA_ARGS__)

/**
 * Implements signature creation from string.
 *
 * Low-level function that parses component names from a string and
 * builds the corresponding signature bitmask.
 *
 * @param ecs Registry containing registered components
 * @param str String containing comma-separated component names
 * @return Signature bitmask
 */
Signature EcsSignatureImpl(ECS *ecs, const char *str);

/**
 * Creates a system that processes entities with specific components.
 *
 * Macro that combines a script function with component filtering and
 * assigns it to a specific execution layer. The system will only run
 * on entities that have ALL specified components.
 *
 * @note The maximum component number per system is 8.
 *
 * @param ecs Registry to add system to
 * @param script Function to execute for matching entities
 * @param layer Execution layer (EcsOnUpdate, etc.)
 * @param ... Component types required (comma-separated)
 *
 * Example: System(ecs, MovePlayer, EcsOnUpdate, Position, Velocity);
 *
 * @see EcsRunSystems()
 * @see EcsAddSystem() to manually add a system with more than 8 components.
 */
#define System(ecs, script, layer, ...)                                        \
  EcsAddSystem(ecs, script, layer, EcsSignature(ecs, __VA_ARGS__))

/**
 * Creates a global system that processes all entities regardless of components.
 *
 * Useful for systems that need to run on all entities without component
 * filtering, like debug rendering or global state updates.
 *
 * @param ecs Registry to add system to
 * @param script Function to execute for all entities
 * @param layer Execution layer
 *
 * Example: SystemGlobal(ecs, DebugDraw, EcsOnRender);
 */
#define SystemGlobal(ecs, script, layer) EcsAddSystem(ecs, script, layer, 0);

/**
 * Adds a system to the registry with explicit parameters.
 *
 * Low-level function used by the System() macros. Registers a system
 * with the specified script, layer, and component signature.
 *
 * @param ecs Registry to add system to
 * @param s Script function to execute
 * @param ly Execution layer
 * @param mask Component signature for entity filtering
 */
void EcsAddSystem(ECS *ecs, Script s, EcsLayer ly, Signature mask);

/**
 * Runs all systems in a specific execution layer.
 *
 * Iterates through all systems in the layer and executes them on entities
 * that match their component signatures and current active/visible state.
 *
 * Update systems (Start, Update, LateUpdate, FixedUpdate) only run on
 * active entities. Render systems (Render, Gui) only run on visible entities.
 *
 * @param ecs Registry to run systems in
 * @param ly Execution layer to run
 *
 * Example: EcsRunSystems(ecs, EcsOnUpdate); // Run all update systems
 */
void EcsRunSystems(ECS *ecs, EcsLayer ly);

#endif
