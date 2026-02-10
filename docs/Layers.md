# Layers

Layers control both rendering order and collision filtering between entities. Every entity has a layer attribute that determines its position in the render hierarchy and which other entities it can collide with.

## Creating Layers

Create layers in the order you want them to render. The first created layer renders first (background), the last renders last (foreground/overlay).

```C
// Create layers in rendering order (bottom to top)
AddLayer(ecs, "Background");  // Renders first
AddLayer(ecs, "Terrain");     // Renders second  
AddLayer(ecs, "Objects");     // Renders third
AddLayer(ecs, "Characters");  // Renders fourth
AddLayer(ecs, "UI");          // Renders last (overlay)
```

## Assigning Entities to Layers

Assign entities to layers using their names:

```C
Entity player = EcsEntity(ecs, "Player");
Entity enemy = EcsEntity(ecs, "Enemy");
Entity wall = EcsEntity(ecs, "Wall");

EntitySetLayer(ecs, player, "Characters");
EntitySetLayer(ecs, enemy, "Characters");  
EntitySetLayer(ecs, wall, "Terrain");
```

## Collision Filtering

By default, all layers can collide with each other. Use layer collision controls to create selective interactions.

### Disable Layer Pairs

Prevent collisions between specific layers:

```C
// Characters don't collide with other characters
LayerDisable(ecs, "Characters", "Characters");

// UI doesn't collide with anything (purely visual)
LayerDisable(ecs, "UI", "Background");
LayerDisable(ecs, "UI", "Terrain");
LayerDisable(ecs, "UI", "Objects");
LayerDisable(ecs, "UI", "Characters");
```

### Disable All Collisions for a Layer

Make a layer completely non-colliding:

```C
// Background and UI layers are purely visual
LayerDisableAll(ecs, "Background");
LayerDisableAll(ecs, "UI");
```

### Enable Layer Collisions

Re-enable collisions between specific layers:

```C
// Re-enable collisions if previously disabled
LayerEnable(ecs, "Objects", "Characters");
```

## Collision Checking

Check if two layers can collide (useful in custom collision systems):

```C
uint8_t layerA = EcsEntityData(world, entityA)->layer;
uint8_t layerB = EcsEntityData(world, entityB)->layer;

bool canCollide = LayerIncludes(ecs, layerA, layerB);
if (canCollide) {
    // Perform collision check
}
```


