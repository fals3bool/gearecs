# Registry

The Registry is the core of gearecs, providing low-level management of entities, components, and systems. It implements the fundamental ECS pattern:

- **Entities**: Unique identifiers representing game objects
- **Components**: Data containers (position, velocity, health, etc.)
- **Systems**: Functions that process entities with specific components

## Creating a Registry

```C
// Create a registry (uses dynamic arrays, no fixed limit)
ECS *world = EcsRegistry();

// When done, free all allocated memory
EcsFree(world);
```

- Entities use dynamic arrays that grow as needed (max 65536 due to uint16_t)
- Each registry manages its own memory pool
- Components are stored in dynamic arrays for cache efficiency

## Entity Management

### Creating Entities

```C
// Create a new entity with a tag name (nullable)
Entity player = EcsEntity(world, "player");
Entity enemy = EcsEntity(world, NULL);
```

### Finding Entities

```C
Entity foundPlayer = EntityFindByTag(world, "player");

if (foundPlayer != InvalidID) {
    // Entity exists
}
```

### Entity State Management

Every entity has two important flags:

- **Active**: Determines if the entity participates in system processing
- **Visible**: Determines if the entity is rendered (used by rendering systems)

```C
EntitySetActive(world, entity, true);   // Enable processing
EntitySetActive(world, entity, false);  // Disable processing

EntitySetVisible(world, entity, true);   // Show entity
EntitySetVisible(world, entity, false);  // Hide entity
```

```C
EntityData *ed = EcsEntityData(world, entity);
if (ed) {
    printf("Entity: %d", entity);
    printf("Tag: %s", ed->tag);
    printf("Active: %s", ed->active ? "Yes" : "No");
    printf("Visible: %s", ed->visible ? "Yes" : "No");
}
```

### Removing Entities

```C
EcsEntityFree(world, entity);
```


