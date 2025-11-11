#include <SDL.h>
#include <iostream>
#include <vector>
#include <memory>

class PixelBuffer {
private:
    std::vector<uint32_t> pixels;
    int width, height;

public:
    PixelBuffer(int w, int h) : width(w), height(h) {
        pixels.resize(w * h);
    }

    void clear(uint32_t color = 0xFF000000) {
        std::fill(pixels.begin(), pixels.end(), color);
    }

    void setPixel(int x, int y, uint32_t color) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            pixels[y * width + x] = color;
        }
    }

    uint32_t getPixel(int x, int y) const {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            return pixels[y * width + x];
        }
        return 0;
    }

    const uint32_t* getData() const { return pixels.data(); }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    void drawLine(int x0, int y0, int x1, int y1, uint32_t color) {
        // Bresenham's line algorithm
        int dx = abs(x1 - x0);
        int dy = abs(y1 - y0);
        int sx = x0 < x1 ? 1 : -1;
        int sy = y0 < y1 ? 1 : -1;
        int err = dx - dy;

        int x = x0, y = y0;
        while (true) {
            setPixel(x, y, color);
            
            if (x == x1 && y == y1) break;
            
            int e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                x += sx;
            }
            if (e2 < dx) {
                err += dx;
                y += sy;
            }
        }
    }

    void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
        drawLine(x0, y0, x1, y1, color);
        drawLine(x1, y1, x2, y2, color);
        drawLine(x2, y2, x0, y0, color);
    }
};

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return -1;
    }

    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    SDL_Window* window = SDL_CreateWindow(
        "Software Renderer",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

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

    // Create our pixel buffer
    PixelBuffer pixelBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);

    std::cout << "Software Renderer initialized!\n";
    std::cout << "Window size: " << WINDOW_WIDTH << "x" << WINDOW_HEIGHT << "\n";

    bool running = true;
    bool needsRedraw = true; // Flag to track if we need to redraw
    SDL_Event e;
    
    // Function to draw the scene
    auto drawScene = [&]() {
        // Clear the pixel buffer with dark blue
        pixelBuffer.clear(0xFF2A3D50);  // ARGB format

        // Demo: Draw lines and triangles instead of random pixels
        
        // Draw some lines in different colors
        pixelBuffer.drawLine(100, 100, 700, 100, 0xFFFF0000); // Red horizontal line
        pixelBuffer.drawLine(100, 100, 100, 500, 0xFF00FF00); // Green vertical line
        pixelBuffer.drawLine(100, 100, 700, 500, 0xFF0000FF); // Blue diagonal line
        
        // Draw a triangle
        pixelBuffer.drawTriangle(400, 150, 300, 350, 500, 350, 0xFFFFFF00); // Yellow triangle
        
        // Draw a more complex shape - a house outline
        pixelBuffer.drawTriangle(200, 200, 150, 250, 250, 250, 0xFFFF00FF); // Roof (magenta)
        pixelBuffer.drawLine(150, 250, 150, 350, 0xFFFFFFFF); // Left wall (white)
        pixelBuffer.drawLine(250, 250, 250, 350, 0xFFFFFFFF); // Right wall (white)
        pixelBuffer.drawLine(150, 350, 250, 350, 0xFFFFFFFF); // Bottom wall (white)
        pixelBuffer.drawLine(180, 300, 220, 300, 0xFF00FFFF); // Door top (cyan)
        pixelBuffer.drawLine(180, 300, 180, 350, 0xFF00FFFF); // Door left (cyan)
        pixelBuffer.drawLine(220, 300, 220, 350, 0xFF00FFFF); // Door right (cyan)
    };
    
    // Draw the initial scene
    drawScene();
    
    while (running) {
        // Wait for events instead of constantly polling
        SDL_WaitEvent(&e);
        
        do {
            switch (e.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                    
                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_ESCAPE) {
                        running = false;
                    } else {
                        // Any other key press triggers a redraw (for future interactive features)
                        needsRedraw = true;
                    }
                    break;
                    
                case SDL_WINDOWEVENT:
                    if (e.window.event == SDL_WINDOWEVENT_EXPOSED ||
                        e.window.event == SDL_WINDOWEVENT_RESIZED) {
                        needsRedraw = true;
                    }
                    break;
                    
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEMOTION:
                    // Future: Add interactive drawing here
                    needsRedraw = true;
                    break;
            }
        } while (SDL_PollEvent(&e)); // Process all queued events
        
        // Only render if we need to
        if (needsRedraw) {
            drawScene();
            
            // Update the texture with our pixel buffer
            void* texturePixels;
            int pitch;
            SDL_LockTexture(texture, NULL, &texturePixels, &pitch);
            memcpy(texturePixels, pixelBuffer.getData(), WINDOW_WIDTH * WINDOW_HEIGHT * 4);
            SDL_UnlockTexture(texture);

            // Render to screen
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
            
            needsRedraw = false; // Reset the flag
        }
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
