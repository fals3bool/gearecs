#ifndef GEARECS_GAMEOBJECT
#define GEARECS_GAMEOBJECT

/**
 * @file gameobject.h
 * @brief High-level game object creation utilities for gearecs
 * 
 * This header provides convenience functions for creating common game objects
 * with standard component configurations. These utilities simplify common
 * entity creation patterns and provide a more "game engine" style API
 * on top of the ECS foundation.
 * 
 * The GameObject function creates entities with commonly used components
 * pre-configured, reducing boilerplate code for standard game objects.
 */

#include <ecs/component.h>

/**
 * Creates a basic game object with standard components.
 * 
 * Creates an entity with EntityData and Transform2 components already
 * attached and configured. The entity is active, visible, and positioned
 * at the origin (0, 0) with default scale and rotation.
 * 
 * This provides a convenient starting point for most game objects,
 * eliminating the need to manually add these basic components.
 * 
 * @param ecs Registry to create the game object in
 * @param tag String tag for entity identification (can be NULL)
 * @return Entity ID of the created game object
 * 
 * @see EntityData for tag-based entity lookup
 * @see Transform2 for positioning and movement
 * @see AddComponent() to add additional components
 */
Entity GameObject(ECS *ecs, char *tag);

#endif
