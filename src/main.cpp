#include <SDL.h>
#include <iostream>
#include <memory>
#include <cstring>

// Include our modularized headers
#include "utils.h"
#include "fractals.h"
#include "pixelbuffer.h"
#include "weird_entities.h"
#include "fractal_system.h"

int main(int argc, char** argv) {
    std::cout << "Starting SDL initialization..." << std::flush;
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return -1;
    }
    std::cout << "SDL initialized successfully\n" << std::flush;

    // Get display information for fullscreen
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0) {
        std::cerr << "SDL_GetCurrentDisplayMode failed: " << SDL_GetError() << "\n";
        SDL_Quit();
        return -1;
    }
    
    std::cout << "Display resolution: " << displayMode.w << "x" << displayMode.h << "\n" << std::flush;

    // Use display resolution for fullscreen, or default for windowed
    int WINDOW_WIDTH = displayMode.w;
    int WINDOW_HEIGHT = displayMode.h;
    bool isFullscreen = true;  // Start in fullscreen mode
    
    // For windowed mode (can be toggled later)
    const int WINDOWED_WIDTH = 800;
    const int WINDOWED_HEIGHT = 600;

    std::cout << "Creating fullscreen window..." << std::flush;
    SDL_Window* window = SDL_CreateWindow(
        "Software Renderer - Weird Visual Chaos Engine",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP
    );

    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        SDL_Quit();
        return -1;
    }
    std::cout << "Fullscreen window created (" << WINDOW_WIDTH << "x" << WINDOW_HEIGHT << ")\n" << std::flush;

    std::cout << "Creating renderer..." << std::flush;
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    std::cout << "Renderer created with VSync enabled\n" << std::flush;

    std::cout << "Creating texture..." << std::flush;
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!texture) {
        std::cerr << "SDL_CreateTexture failed: " << SDL_GetError() << "\n";
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    std::cout << "Texture created (" << WINDOW_WIDTH << "x" << WINDOW_HEIGHT << ")\n" << std::flush;

    // Create our pixel buffer with current resolution
    std::cout << "Creating pixel buffer..." << std::flush;
    PixelBuffer pixelBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
    std::cout << "Pixel buffer created\n" << std::flush;

    std::cout << "Software Renderer initialized in fullscreen!\n";
    std::cout << "Current resolution: " << WINDOW_WIDTH << "x" << WINDOW_HEIGHT << "\n" << std::flush;
    std::cout << "Controls:\n";
    std::cout << "  ESC - Exit\n";
    std::cout << "  F11 - Toggle fullscreen/windowed\n";
    std::cout << "  F - Toggle fullscreen/windowed\n";
    std::cout << "  M - Toggle between Weird Chaos and Fractal/Game of Life modes\n";
    std::cout << "  SPACE - Force chaos injection (in fractal mode)\n";
    std::cout << "  R - Reset current mode\n" << std::flush;

    bool running = true;
    bool needsRedraw = true;
    SDL_Event e;
    
    // Animation variables
    float rotation_time = 0.0f;
    uint32_t last_time = SDL_GetTicks();
    
    std::cout << "Creating visual systems..." << std::flush;
    
    // Create weird visual manager
    WeirdVisualManager weirdVisualManager;

    // Create fractal/game of life system
    FractalGameOfLifeSystem fractalSystem(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    // Set global pointer for injection functions
    g_fractalSystem = &fractalSystem;

    // Mode toggle: true=Weird Chaos, false=Fractal/Game of Life
    bool isWeirdChaosMode = true;

    std::cout << "Initialized dual-mode system!\n";
    std::cout << "Starting in Weird Chaos Mode\n" << std::flush;

    // Function to toggle fullscreen
    auto toggleFullscreen = [&]() {
        if (isFullscreen) {
            // Switch to windowed mode
            SDL_SetWindowFullscreen(window, 0);
            SDL_SetWindowSize(window, WINDOWED_WIDTH, WINDOWED_HEIGHT);
            SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
            
            // Recreate texture for new size
            SDL_DestroyTexture(texture);
            texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                      SDL_TEXTUREACCESS_STREAMING,
                                      WINDOWED_WIDTH, WINDOWED_HEIGHT);
            
            // Recreate pixel buffer for new size
            pixelBuffer = PixelBuffer(WINDOWED_WIDTH, WINDOWED_HEIGHT);
            
            // Resize fractal system to match new resolution
            fractalSystem.resize(WINDOWED_WIDTH, WINDOWED_HEIGHT);
            
            WINDOW_WIDTH = WINDOWED_WIDTH;
            WINDOW_HEIGHT = WINDOWED_HEIGHT;
            isFullscreen = false;
            
            std::cout << "Switched to windowed mode (" << WINDOW_WIDTH << "x" << WINDOW_HEIGHT << ")\n" << std::flush;
        } else {
            // Switch to fullscreen mode
            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
            
            // Recreate texture for new size
            SDL_DestroyTexture(texture);
            texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                      SDL_TEXTUREACCESS_STREAMING,
                                      displayMode.w, displayMode.h);
            
            // Recreate pixel buffer for new size
            pixelBuffer = PixelBuffer(displayMode.w, displayMode.h);
            
            // Resize fractal system to match new resolution
            fractalSystem.resize(displayMode.w, displayMode.h);
            
            WINDOW_WIDTH = displayMode.w;
            WINDOW_HEIGHT = displayMode.h;
            isFullscreen = true;
            
            std::cout << "Switched to fullscreen mode (" << WINDOW_WIDTH << "x" << WINDOW_HEIGHT << ")\n" << std::flush;
        }
        needsRedraw = true;
    };

    // Function to toggle modes
    auto toggleMode = [&]() {
        isWeirdChaosMode = !isWeirdChaosMode;
        if (isWeirdChaosMode) {
            std::cout << "Switched to Weird Chaos Mode - 3D entities with chaotic physics\n" << std::flush;
        } else {
            std::cout << "Switched to Fractal/Game of Life Mode - Current: " << fractalSystem.getCurrentModeName() << "\n" << std::flush;
        }
        needsRedraw = true;
    };

    // Function to draw the scene
    auto drawScene = [&]() {
        std::cout << "\n=== DRAWING SCENE (" << WINDOW_WIDTH << "x" << WINDOW_HEIGHT << ") ===\n" << std::flush;
        
        // Update animation time
        uint32_t current_time = SDL_GetTicks();
        float delta_time = (current_time - last_time) / 1000.0f;
        last_time = current_time;
        rotation_time += delta_time;
        
        if (isWeirdChaosMode) {
            std::cout << "Mode: Weird Chaos - Rendering 3D entities\n" << std::flush;
            
            // Clear with a randomly shifting dark background
            uint32_t bgColor = 0xFF000000 | 
                              (randomInt(5, 25) << 16) | 
                              (randomInt(5, 25) << 8) | 
                              randomInt(5, 25);
            pixelBuffer.clear(bgColor);
            
            // Update weird visual entities
            weirdVisualManager.update(delta_time);
            
            // Set up perspective projection matrix with current aspect ratio
            float fov = 45.0f * M_PI / 180.0f; // 45 degrees in radians
            float aspect = (float)WINDOW_WIDTH / WINDOW_HEIGHT;
            Matrix4x4 projection = Matrix4x4::perspective(fov, aspect, 0.1f, 100.0f);
            
            // Render weird visual entities first (background layer)
            auto weirdTriangles = weirdVisualManager.getAllTriangles();
            std::cout << "Rendering " << weirdTriangles.size() << " weird triangles from " << weirdVisualManager.getEntityCount() << " entities...\n" << std::flush;
            
            for (const auto& triangle : weirdTriangles) {
                // Transform weird triangle
                Matrix4x4 weirdTransform = projection;
                Triangle3D transformed = triangle.transform(weirdTransform);
                
                // Check if triangle is facing towards camera
                Vec3 normal = transformed.getNormal();
                if (normal.z > 0) {
                    pixelBuffer.render3DTriangle(transformed, WINDOW_WIDTH, WINDOW_HEIGHT);
                }
            }
            
            // Add some chaos background effects (scale with resolution)
            if (randomFloat(0, 1) < 0.1f) { // 10% chance per frame
                // Random streaks across screen
                int numStreaks = randomInt(1, 5);
                for (int i = 0; i < numStreaks; i++) {
                    pixelBuffer.drawLine(
                        randomInt(0, WINDOW_WIDTH), randomInt(0, WINDOW_HEIGHT),
                        randomInt(0, WINDOW_WIDTH), randomInt(0, WINDOW_HEIGHT),
                        randomColor()
                    );
                }
            }
            
            // Occasionally add screen-wide effects (scale with resolution)
            if (randomFloat(0, 1) < 0.05f) { // 5% chance per frame
                switch (randomInt(0, 2)) {
                    case 0: // Random dots
                        for (int i = 0; i < randomInt(50, 200); i++) {
                            pixelBuffer.setPixel(randomInt(0, WINDOW_WIDTH), randomInt(0, WINDOW_HEIGHT), randomColor());
                        }
                        break;
                    case 1: // Random rectangles
                        for (int i = 0; i < randomInt(3, 8); i++) {
                            int maxSize = std::min(WINDOW_WIDTH, WINDOW_HEIGHT) / 20; // Scale with resolution
                            int x = randomInt(0, WINDOW_WIDTH - maxSize);
                            int y = randomInt(0, WINDOW_HEIGHT - maxSize);
                            pixelBuffer.fillRectangle(x, y, randomInt(10, maxSize), randomInt(10, maxSize), randomColor());
                        }
                        break;
                }
            }
        } else {
            // Fractal/Game of Life mode
            std::cout << "Mode: Fractal/Game of Life - Current: " << fractalSystem.getCurrentModeName() << "\n" << std::flush;
            pixelBuffer.clear(0xFF000000); // Black background
            fractalSystem.update(delta_time);
            fractalSystem.render(pixelBuffer);
        }
        
        std::cout << "=== SCENE DRAWING COMPLETE ===\n\n" << std::flush;
    };

    std::cout << "About to call drawScene...\n" << std::flush;
    drawScene();
    std::cout << "drawScene completed, about to render...\n" << std::flush;
    
    // Initial render
    void* texturePixels;
    int pitch;
    std::cout << "Locking texture...\n" << std::flush;
    SDL_LockTexture(texture, NULL, &texturePixels, &pitch);
    
    std::cout << "Copying pixel data...\n" << std::flush;
    memcpy(texturePixels, pixelBuffer.getData(), WINDOW_WIDTH * WINDOW_HEIGHT * 4);
    
    std::cout << "Unlocking texture...\n" << std::flush;
    SDL_UnlockTexture(texture);

    std::cout << "Rendering to screen...\n" << std::flush;
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    std::cout << "Initial render complete - you should now see shapes on screen!\n" << std::flush;
    
    std::cout << "Entering main loop (press ESC to exit, F11 or F to toggle fullscreen, M to toggle modes)...\n" << std::flush;
    
    std::cout << "Entering main event loop...\n" << std::flush;
    
    while (running) {
        // Process events
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                    
                case SDL_KEYDOWN:
                    switch (e.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            running = false;
                            break;
                            
                        case SDLK_F11:
                        case SDLK_f:
                            toggleFullscreen();
                            break;
                            
                        case SDLK_m:
                            toggleMode();
                            break;
                            
                        case SDLK_SPACE:
                            // Force chaos injection in fractal mode
                            if (!isWeirdChaosMode) {
                                fractalSystem.initialize(); // Reinitialize with new random patterns
                                std::cout << "Chaos injected! New pattern: " << fractalSystem.getCurrentModeName() << "\n" << std::flush;
                            }
                            needsRedraw = true;
                            break;
                            
                        case SDLK_r:
                            // Reset everything
                            if (isWeirdChaosMode) {
                                weirdVisualManager = WeirdVisualManager();
                                std::cout << "Weird entities reset!\n" << std::flush;
                            } else {
                                fractalSystem.initialize();
                                std::cout << "Fractal system reset!\n" << std::flush;
                            }
                            needsRedraw = true;
                            break;
                    }
                    break;
                    
                case SDL_WINDOWEVENT:
                    if (e.window.event == SDL_WINDOWEVENT_RESIZED || 
                        e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                        needsRedraw = true;
                    }
                    break;
            }
        }
        
        // Continuous animation - always redraw for fluid motion
        needsRedraw = true;
        
        if (needsRedraw) {
            drawScene();
            
            // Lock texture and copy pixel data
            void* texturePixels;
            int pitch;
            if (SDL_LockTexture(texture, NULL, &texturePixels, &pitch) == 0) {
                memcpy(texturePixels, pixelBuffer.getData(), WINDOW_WIDTH * WINDOW_HEIGHT * 4);
                SDL_UnlockTexture(texture);
                
                // Render to screen
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, texture, NULL, NULL);
                SDL_RenderPresent(renderer);
            }
            
            needsRedraw = false;
        }
        
        // Small delay to prevent excessive CPU usage while maintaining smooth animation
        SDL_Delay(1); // ~1000 FPS cap, but VSync will limit to monitor refresh rate
    }

    std::cout << "Cleaning up...\n" << std::flush;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    std::cout << "Software Renderer terminated successfully!\n" << std::flush;
    return 0;
}
