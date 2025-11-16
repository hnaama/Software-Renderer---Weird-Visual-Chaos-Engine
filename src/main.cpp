#include <SDL.h>
#include <iostream>
#include <vector>
#include <memory>

class PixelBuffer {
private:
    std::vector<uint32_t> pixels;
    int width, height;

    // Color utility functions for vertex color interpolation
    struct Color {
        float r, g, b, a;
        
        Color(uint32_t argb = 0xFF000000) {
            a = ((argb >> 24) & 0xFF) / 255.0f;
            r = ((argb >> 16) & 0xFF) / 255.0f;
            g = ((argb >> 8) & 0xFF) / 255.0f;
            b = (argb & 0xFF) / 255.0f;
        }
        
        Color(float red, float green, float blue, float alpha = 1.0f) 
            : r(red), g(green), b(blue), a(alpha) {}
        
        uint32_t toARGB() const {
            uint8_t ai = (uint8_t)(a * 255.0f);
            uint8_t ri = (uint8_t)(r * 255.0f);
            uint8_t gi = (uint8_t)(g * 255.0f);
            uint8_t bi = (uint8_t)(b * 255.0f);
            return (ai << 24) | (ri << 16) | (gi << 8) | bi;
        }
        
        Color operator+(const Color& other) const {
            return Color(r + other.r, g + other.g, b + other.b, a + other.a);
        }
        
        Color operator*(float scalar) const {
            return Color(r * scalar, g * scalar, b * scalar, a * scalar);
        }
    };

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

    void fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
        // Sort vertices by Y coordinate (top to bottom)
        if (y0 > y1) { std::swap(x0, x1); std::swap(y0, y1); }
        if (y0 > y2) { std::swap(x0, x2); std::swap(y0, y2); }
        if (y1 > y2) { std::swap(x1, x2); std::swap(y1, y2); }

        // Handle degenerate cases (flat triangles or lines)
        if (y0 == y2) return; // All points on same horizontal line

        // Calculate slopes for triangle edges
        auto drawScanline = [&](int y, int x_start, int x_end) {
            if (x_start > x_end) std::swap(x_start, x_end);
            for (int x = x_start; x <= x_end; x++) {
                setPixel(x, y, color);
            }
        };

        // Fill the triangle using scanline algorithm
        for (int y = y0; y <= y2; y++) {
            // Calculate intersection points with triangle edges
            float x_left, x_right;
            
            // Left edge: always from top (x0,y0) to bottom (x2,y2)
            if (y2 != y0) {
                x_left = x0 + (float)(x2 - x0) * (y - y0) / (y2 - y0);
            } else {
                x_left = x0;
            }
            
            // Right edge: depends on whether we're in upper or lower part
            if (y <= y1) {
                // Upper part: from (x0,y0) to (x1,y1)
                if (y1 != y0) {
                    x_right = x0 + (float)(x1 - x0) * (y - y0) / (y1 - y0);
                } else {
                    x_right = x0;
                }
            } else {
                // Lower part: from (x1,y1) to (x2,y2)
                if (y2 != y1) {
                    x_right = x1 + (float)(x2 - x1) * (y - y1) / (y2 - y1);
                } else {
                    x_right = x1;
                }
            }
            
            drawScanline(y, (int)x_left, (int)x_right);
        }
    }

    void fillTriangleBarycentric(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
        // Calculate bounding box
        int min_x = std::min({x0, x1, x2});
        int max_x = std::max({x0, x1, x2});
        int min_y = std::min({y0, y1, y2});
        int max_y = std::max({y0, y1, y2});

        // Clamp to screen bounds
        min_x = std::max(0, min_x);
        max_x = std::min(width - 1, max_x);
        min_y = std::max(0, min_y);
        max_y = std::min(height - 1, max_y);

        // Helper function to calculate barycentric coordinates
        auto sign = [](int x0, int y0, int x1, int y1, int x2, int y2) -> float {
            return (x0 - x2) * (y1 - y2) - (x1 - x2) * (y0 - y2);
        };

        // Calculate triangle area (used for barycentric coordinates)
        float area = sign(x0, y0, x1, y1, x2, y2);
        if (std::abs(area) < 0.001f) return; // Degenerate triangle

        // For each pixel in bounding box
        for (int y = min_y; y <= max_y; y++) {
            for (int x = min_x; x <= max_x; x++) {
                // Calculate barycentric coordinates
                float w0 = sign(x, y, x1, y1, x2, y2) / area;
                float w1 = sign(x0, y0, x, y, x2, y2) / area;
                float w2 = sign(x0, y0, x1, y1, x, y) / area;

                // Check if point is inside triangle
                if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                    setPixel(x, y, color);
                }
            }
        }
    }

    void drawTriangleWireframe(int x0, int y0, int x1, int y1, int x2, int y2, 
                              uint32_t wireframe_color, uint32_t fill_color, bool filled = true) {
        if (filled) {
            fillTriangleBarycentric(x0, y0, x1, y1, x2, y2, fill_color);
        }
        // Draw wireframe on top
        drawLine(x0, y0, x1, y1, wireframe_color);
        drawLine(x1, y1, x2, y2, wireframe_color);
        drawLine(x2, y2, x0, y0, wireframe_color);
    }

    void fillRectangle(int x, int y, int w, int h, uint32_t color) {
        for (int py = y; py < y + h; py++) {
            for (int px = x; px < x + w; px++) {
                setPixel(px, py, color);
            }
        }
    }

    // New method: Fill triangle with vertex color interpolation using barycentric coordinates
    void fillTriangleGradient(int x0, int y0, uint32_t color0,
                             int x1, int y1, uint32_t color1,
                             int x2, int y2, uint32_t color2) {
        // Calculate bounding box
        int min_x = std::min({x0, x1, x2});
        int max_x = std::max({x0, x1, x2});
        int min_y = std::min({y0, y1, y2});
        int max_y = std::max({y0, y1, y2});

        // Clamp to screen bounds
        min_x = std::max(0, min_x);
        max_x = std::min(width - 1, max_x);
        min_y = std::max(0, min_y);
        max_y = std::min(height - 1, max_y);

        // Convert colors to floating point for interpolation
        Color c0(color0), c1(color1), c2(color2);

        // Helper function to calculate barycentric coordinates
        auto sign = [](int x0, int y0, int x1, int y1, int x2, int y2) -> float {
            return (x0 - x2) * (y1 - y2) - (x1 - x2) * (y0 - y2);
        };

        // Calculate triangle area (used for barycentric coordinates)
        float area = sign(x0, y0, x1, y1, x2, y2);
        if (std::abs(area) < 0.001f) return; // Degenerate triangle

        // For each pixel in bounding box
        for (int y = min_y; y <= max_y; y++) {
            for (int x = min_x; x <= max_x; x++) {
                // Calculate barycentric coordinates
                float w0 = sign(x, y, x1, y1, x2, y2) / area;
                float w1 = sign(x0, y0, x, y, x2, y2) / area;
                float w2 = sign(x0, y0, x1, y1, x, y) / area;

                // Check if point is inside triangle
                if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                    // Interpolate color using barycentric coordinates
                    Color interpolatedColor = c0 * w0 + c1 * w1 + c2 * w2;
                    setPixel(x, y, interpolatedColor.toARGB());
                }
            }
        }
    }

    // New method: Fill triangle with smooth color transitions using scanline algorithm
    void fillTriangleGradientScanline(int x0, int y0, uint32_t color0,
                                     int x1, int y1, uint32_t color1,
                                     int x2, int y2, uint32_t color2) {
        // Sort vertices by Y coordinate, keeping colors aligned
        if (y0 > y1) { 
            std::swap(x0, x1); std::swap(y0, y1); std::swap(color0, color1);
        }
        if (y0 > y2) { 
            std::swap(x0, x2); std::swap(y0, y2); std::swap(color0, color2);
        }
        if (y1 > y2) { 
            std::swap(x1, x2); std::swap(y1, y2); std::swap(color1, color2);
        }

        if (y0 == y2) return; // Degenerate triangle

        Color c0(color0), c1(color1), c2(color2);

        // Helper to interpolate between two points (position and color)
        auto lerp = [](float a, float b, float t) -> float {
            return a + t * (b - a);
        };

        auto lerpColor = [&](const Color& ca, const Color& cb, float t) -> Color {
            return Color(
                lerp(ca.r, cb.r, t),
                lerp(ca.g, cb.g, t), 
                lerp(ca.b, cb.b, t),
                lerp(ca.a, cb.a, t)
            );
        };

        // Fill the triangle using scanline algorithm with color interpolation
        for (int y = y0; y <= y2; y++) {
            float t_main = (float)(y - y0) / (y2 - y0);
            
            // Left edge: always from top to bottom
            float x_left = lerp(x0, x2, t_main);
            Color color_left = lerpColor(c0, c2, t_main);
            
            // Right edge: depends on whether we're in upper or lower part
            float x_right;
            Color color_right;
            
            if (y <= y1) {
                // Upper part: from (x0,y0) to (x1,y1)
                if (y1 != y0) {
                    float t_upper = (float)(y - y0) / (y1 - y0);
                    x_right = lerp(x0, x1, t_upper);
                    color_right = lerpColor(c0, c1, t_upper);
                } else {
                    x_right = x0;
                    color_right = c0;
                }
            } else {
                // Lower part: from (x1,y1) to (x2,y2)
                if (y2 != y1) {
                    float t_lower = (float)(y - y1) / (y2 - y1);
                    x_right = lerp(x1, x2, t_lower);
                    color_right = lerpColor(c1, c2, t_lower);
                } else {
                    x_right = x1;
                    color_right = c1;
                }
            }
            
            // Ensure left is actually left
            if (x_left > x_right) {
                std::swap(x_left, x_right);
                std::swap(color_left, color_right);
            }
            
            // Draw horizontal line with color interpolation
            int x_start = (int)x_left;
            int x_end = (int)x_right;
            
            for (int x = x_start; x <= x_end; x++) {
                if (x_end != x_start) {
                    float t_horizontal = (float)(x - x_start) / (x_end - x_start);
                    Color finalColor = lerpColor(color_left, color_right, t_horizontal);
                    setPixel(x, y, finalColor.toARGB());
                } else {
                    setPixel(x, y, color_left.toARGB());
                }
            }
        }
    }

    // New method: Create rainbow triangle (each vertex a different primary color)
    void fillTriangleRainbow(int x0, int y0, int x1, int y1, int x2, int y2) {
        fillTriangleGradient(
            x0, y0, 0xFFFF0000,  // Red vertex
            x1, y1, 0xFF00FF00,  // Green vertex  
            x2, y2, 0xFF0000FF   // Blue vertex
        );
    }
};

int main(int argc, char** argv) {
    std::cout << "Starting SDL initialization..." << std::flush;
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return -1;
    }
    std::cout << "SDL initialized successfully\n" << std::flush;

    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    std::cout << "Creating window..." << std::flush;
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
    std::cout << "Window created\n" << std::flush;

    std::cout << "Creating renderer..." << std::flush;
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    std::cout << "Renderer created\n" << std::flush;

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
    std::cout << "Texture created\n" << std::flush;

    // Create our pixel buffer
    std::cout << "Creating pixel buffer..." << std::flush;
    PixelBuffer pixelBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
    std::cout << "Pixel buffer created\n" << std::flush;

    std::cout << "Software Renderer initialized!\n";
    std::cout << "Window size: " << WINDOW_WIDTH << "x" << WINDOW_HEIGHT << "\n" << std::flush;

    bool running = true;
    bool needsRedraw = true;
    SDL_Event e;
    
    std::cout << "Creating drawScene function..." << std::flush;
    
    // Function to draw the scene
    auto drawScene = [&]() {
        std::cout << "\n=== DRAWING GRADIENT TRIANGLES DEMO ===\n" << std::flush;
        
        // Clear with dark background to make colors pop
        pixelBuffer.clear(0xFF202020);
        std::cout << "Buffer cleared with dark background\n" << std::flush;
        
        // Demo 1: RGB Triangle (Red-Green-Blue vertices)
        std::cout << "Drawing RGB gradient triangle...\n" << std::flush;
        pixelBuffer.fillTriangleGradient(
            100, 100, 0xFFFF0000,  // Top vertex: Red
            50,  200, 0xFF00FF00,  // Bottom left: Green  
            150, 200, 0xFF0000FF   // Bottom right: Blue
        );
        
        // Demo 2: Warm colors triangle (Red-Yellow-Orange)
        std::cout << "Drawing warm colors triangle...\n" << std::flush;
        pixelBuffer.fillTriangleGradient(
            300, 80,  0xFFFF0000,  // Top: Red
            250, 180, 0xFFFFFF00,  // Bottom left: Yellow
            350, 180, 0xFFFF8000   // Bottom right: Orange
        );
        
        // Demo 3: Cool colors triangle (Blue-Cyan-Purple)
        std::cout << "Drawing cool colors triangle...\n" << std::flush;
        pixelBuffer.fillTriangleGradient(
            500, 100, 0xFF0000FF,  // Top: Blue
            450, 200, 0xFF00FFFF,  // Bottom left: Cyan
            550, 200, 0xFF8000FF   // Bottom right: Purple
        );
        
        // Demo 4: Large rainbow triangle using convenience method
        std::cout << "Drawing large rainbow triangle...\n" << std::flush;
        pixelBuffer.fillTriangleRainbow(
            400, 250,  // Top center
            200, 450,  // Bottom left
            600, 450   // Bottom right
        );
        
        // Demo 5: Scanline gradient triangle (different algorithm)
        std::cout << "Drawing scanline gradient triangle...\n" << std::flush;
        pixelBuffer.fillTriangleGradientScanline(
            150, 300, 0xFFFF00FF,  // Magenta
            100, 400, 0xFF00FF00,  // Green
            200, 400, 0xFF0080FF   // Light Blue
        );
        
        // Demo 6: Grayscale gradient triangle
        std::cout << "Drawing grayscale gradient triangle...\n" << std::flush;
        pixelBuffer.fillTriangleGradient(
            650, 300, 0xFFFFFFFF,  // White
            600, 400, 0xFF808080,  // Gray
            700, 400, 0xFF000000   // Black
        );
        
        // Demo 7: Multiple overlapping transparent-ish triangles
        std::cout << "Drawing overlapping colored triangles...\n" << std::flush;
        pixelBuffer.fillTriangleGradient(
            80, 480, 0xFFFF4040,   // Light red
            40, 550, 0xFF40FF40,   // Light green
            120, 550, 0xFF4040FF   // Light blue
        );
        
        // Demo 8: Pastel colors triangle
        std::cout << "Drawing pastel colors triangle...\n" << std::flush;
        pixelBuffer.fillTriangleGradient(
            600, 480, 0xFFFFB6C1,  // Light pink
            550, 550, 0xFFB6FFB6,  // Light green
            650, 550, 0xFFB6B6FF   // Light blue
        );
        
        std::cout << "=== GRADIENT TRIANGLES DEMO COMPLETE ===\n\n" << std::flush;
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
    
    std::cout << "Entering main loop (press ESC to exit, any other key to redraw)...\n" << std::flush;
    
    while (running) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    std::cout << "Quit event received\n" << std::flush;
                    running = false;
                    break;
                    
                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_ESCAPE) {
                        std::cout << "Escape pressed, exiting\n" << std::flush;
                        running = false;
                    } else {
                        std::cout << "Key pressed, redrawing...\n" << std::flush;
                        needsRedraw = true;
                    }
                    break;
                    
                case SDL_WINDOWEVENT:
                    if (e.window.event == SDL_WINDOWEVENT_EXPOSED ||
                        e.window.event == SDL_WINDOWEVENT_RESIZED) {
                        std::cout << "Window event, redrawing...\n" << std::flush;
                        needsRedraw = true;
                    }
                    break;
            }
        }
        
        if (needsRedraw) {
            std::cout << "Performing redraw...\n" << std::flush;
            drawScene();
            
            SDL_LockTexture(texture, NULL, &texturePixels, &pitch);
            memcpy(texturePixels, pixelBuffer.getData(), WINDOW_WIDTH * WINDOW_HEIGHT * 4);
            SDL_UnlockTexture(texture);

            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
            
            needsRedraw = false;
            std::cout << "Redraw complete\n" << std::flush;
        }
        
        SDL_Delay(16);
    }

    std::cout << "Cleaning up SDL resources...\n" << std::flush;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    std::cout << "Program exiting normally\n" << std::flush;
    return 0;
}
