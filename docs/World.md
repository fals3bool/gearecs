# World

The World is gearecs's high-level API that provides a complete game development environment. It builds upon the Registry by:

- Automatically registering all built-in components (Transform2, RigidBody, Collider, Sprite, etc.)
- Registering all built-in systems (physics, collision, rendering, etc.)
- Creating a camera entity with proper components for 2D rendering
- Providing a complete, optimized game loop with `EcsLoop()`

## Registry vs World

- **Registry**: Low-level ECS management, manual system registration and execution
- **World**: High-level wrapper with pre-registered components and systems and automatic game loop

Use Registry when you need:
- Full control over component/system registration
- Custom game loop implementation

Use World when you need:
- Quick setup with built-in functionality
- Standard game loop structure 
- Ready-to-use components and systems 

## World Usage

```C
#include <gearecs.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Getting Started with Gearecs");
    Camera2D camera = {0};
    camera.offset = (Vector2){SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
    camera.zoom = 1.f;

    ECS *world = EcsWorld(camera);
    
    // Your game logic here
    
    // Run the game loop (blocks until window should close)
    EcsLoop(world);
    
    // Cleanup
    EcsFree(world);
    CloseWindow();
    return 0;
}
```


