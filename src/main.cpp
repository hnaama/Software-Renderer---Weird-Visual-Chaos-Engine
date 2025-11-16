#include <SDL.h>
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>

// 3D Vector and Matrix classes for 3D transformations
struct Vec3 {
    float x, y, z;
    
    Vec3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
    
    Vec3 operator+(const Vec3& other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }
    
    Vec3 operator-(const Vec3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }
    
    Vec3 operator*(float scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }
    
    float dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    
    Vec3 cross(const Vec3& other) const {
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
    
    float length() const {
        return sqrt(x * x + y * y + z * z);
    }
    
    Vec3 normalize() const {
        float len = length();
        if (len > 0.001f) {
            return Vec3(x / len, y / len, z / len);
        }
        return Vec3(0, 0, 0);
    }
};

struct Matrix4x4 {
    float m[4][4];
    
    Matrix4x4() {
        // Initialize as identity matrix
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                m[i][j] = (i == j) ? 1.0f : 0.0f;
            }
        }
    }
    
    // Create rotation matrix around X axis
    static Matrix4x4 rotationX(float angle) {
        Matrix4x4 mat;
        float c = cos(angle);
        float s = sin(angle);
        mat.m[1][1] = c; mat.m[1][2] = -s;
        mat.m[2][1] = s; mat.m[2][2] = c;
        return mat;
    }
    
    // Create rotation matrix around Y axis
    static Matrix4x4 rotationY(float angle) {
        Matrix4x4 mat;
        float c = cos(angle);
        float s = sin(angle);
        mat.m[0][0] = c;  mat.m[0][2] = s;
        mat.m[2][0] = -s; mat.m[2][2] = c;
        return mat;
    }
    
    // Create rotation matrix around Z axis
    static Matrix4x4 rotationZ(float angle) {
        Matrix4x4 mat;
        float c = cos(angle);
        float s = sin(angle);
        mat.m[0][0] = c; mat.m[0][1] = -s;
        mat.m[1][0] = s; mat.m[1][1] = c;
        return mat;
    }
    
    // Create translation matrix
    static Matrix4x4 translation(float x, float y, float z) {
        Matrix4x4 mat;
        mat.m[0][3] = x;
        mat.m[1][3] = y;
        mat.m[2][3] = z;
        return mat;
    }
    
    // Create perspective projection matrix
    static Matrix4x4 perspective(float fov, float aspect, float near, float far) {
        Matrix4x4 mat;
        float tanHalfFov = tan(fov / 2.0f);
        
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                mat.m[i][j] = 0.0f;
            }
        }
        
        mat.m[0][0] = 1.0f / (aspect * tanHalfFov);
        mat.m[1][1] = 1.0f / tanHalfFov;
        mat.m[2][2] = -(far + near) / (far - near);
        mat.m[2][3] = -(2.0f * far * near) / (far - near);
        mat.m[3][2] = -1.0f;
        
        return mat;
    }
    
    // Matrix multiplication
    Matrix4x4 operator*(const Matrix4x4& other) const {
        Matrix4x4 result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result.m[i][j] = 0;
                for (int k = 0; k < 4; k++) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }
    
    // Transform a 3D point
    Vec3 transform(const Vec3& v) const {
        float w = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3];
        if (abs(w) < 0.001f) w = 1.0f; // Avoid division by zero
        
        return Vec3(
            (m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3]) / w,
            (m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3]) / w,
            (m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3]) / w
        );
    }
};

// 3D Triangle structure
struct Triangle3D {
    Vec3 vertices[3];
    uint32_t colors[3];
    
    Triangle3D(Vec3 v0, Vec3 v1, Vec3 v2, uint32_t c0, uint32_t c1, uint32_t c2) {
        vertices[0] = v0; vertices[1] = v1; vertices[2] = v2;
        colors[0] = c0; colors[1] = c1; colors[2] = c2;
    }
    
    // Calculate triangle normal for lighting
    Vec3 getNormal() const {
        Vec3 edge1 = vertices[1] - vertices[0];
        Vec3 edge2 = vertices[2] - vertices[0];
        return edge1.cross(edge2).normalize();
    }
    
    // Transform triangle by matrix
    Triangle3D transform(const Matrix4x4& matrix) const {
        return Triangle3D(
            matrix.transform(vertices[0]),
            matrix.transform(vertices[1]),
            matrix.transform(vertices[2]),
            colors[0], colors[1], colors[2]
        );
    }
};

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

    // New method: Project 3D point to 2D screen coordinates
    std::pair<int, int> project3DTo2D(const Vec3& point, int screenWidth, int screenHeight) {
        // Simple perspective projection (assuming point is already in normalized device coordinates)
        int x = (int)((point.x + 1.0f) * 0.5f * screenWidth);
        int y = (int)((1.0f - point.y) * 0.5f * screenHeight); // Flip Y axis
        return {x, y};
    }
    
    // New method: Render 3D triangle with depth checking
    void render3DTriangle(const Triangle3D& triangle, int screenWidth, int screenHeight) {
        // Project 3D vertices to 2D screen coordinates
        auto p0 = project3DTo2D(triangle.vertices[0], screenWidth, screenHeight);
        auto p1 = project3DTo2D(triangle.vertices[1], screenWidth, screenHeight);
        auto p2 = project3DTo2D(triangle.vertices[2], screenWidth, screenHeight);
        
        // Calculate lighting based on triangle normal (simple directional light)
        Vec3 lightDir = Vec3(0.3f, -0.5f, -0.7f).normalize();
        Vec3 normal = triangle.getNormal();
        float lightIntensity = std::max(0.2f, -normal.dot(lightDir)); // Clamp to avoid pure black
        
        // Apply lighting to colors
        auto applyLighting = [lightIntensity](uint32_t color) -> uint32_t {
            uint8_t a = (color >> 24) & 0xFF;
            uint8_t r = (uint8_t)(((color >> 16) & 0xFF) * lightIntensity);
            uint8_t g = (uint8_t)(((color >> 8) & 0xFF) * lightIntensity);
            uint8_t b = (uint8_t)((color & 0xFF) * lightIntensity);
            return (a << 24) | (r << 16) | (g << 8) | b;
        };
        
        uint32_t litColor0 = applyLighting(triangle.colors[0]);
        uint32_t litColor1 = applyLighting(triangle.colors[1]);
        uint32_t litColor2 = applyLighting(triangle.colors[2]);
        
        // Render the triangle with gradient colors
        fillTriangleGradient(
            p0.first, p0.second, litColor0,
            p1.first, p1.second, litColor1,
            p2.first, p2.second, litColor2
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
    
    // Animation variables
    float rotation_time = 0.0f;
    uint32_t last_time = SDL_GetTicks();
    
    std::cout << "Creating drawScene function..." << std::flush;
    
    // Function to draw the scene
    auto drawScene = [&]() {
        std::cout << "\n=== DRAWING 3D ROTATING TRIANGLES DEMO ===\n" << std::flush;
        
        // Clear with dark background to make colors pop
        pixelBuffer.clear(0xFF101010);
        std::cout << "Buffer cleared with dark background\n" << std::flush;
        
        // Set up perspective projection matrix
        float fov = 45.0f * M_PI / 180.0f; // 45 degrees in radians
        float aspect = (float)WINDOW_WIDTH / WINDOW_HEIGHT;
        Matrix4x4 projection = Matrix4x4::perspective(fov, aspect, 0.1f, 100.0f);
        
        // Create base triangles in 3D space
        std::vector<Triangle3D> triangles;
        
        // Triangle 1: RGB triangle rotating around Y axis
        triangles.push_back(Triangle3D(
            Vec3(0.0f, 0.5f, -3.0f),   // Top vertex
            Vec3(-0.5f, -0.5f, -3.0f), // Bottom left
            Vec3(0.5f, -0.5f, -3.0f),  // Bottom right
            0xFFFF0000, 0xFF00FF00, 0xFF0000FF // Red, Green, Blue
        ));
        
        // Triangle 2: Warm colors triangle rotating around X axis
        triangles.push_back(Triangle3D(
            Vec3(0.0f, 0.4f, -2.5f),
            Vec3(-0.4f, -0.4f, -2.5f),
            Vec3(0.4f, -0.4f, -2.5f),
            0xFFFF0000, 0xFFFFFF00, 0xFFFF8000 // Red, Yellow, Orange
        ));
        
        // Triangle 3: Cool colors triangle rotating around Z axis
        triangles.push_back(Triangle3D(
            Vec3(0.0f, 0.3f, -4.0f),
            Vec3(-0.3f, -0.3f, -4.0f),
            Vec3(0.3f, -0.3f, -4.0f),
            0xFF0000FF, 0xFF00FFFF, 0xFF8000FF // Blue, Cyan, Purple
        ));
        
        // Triangle 4: Large triangle rotating around multiple axes
        triangles.push_back(Triangle3D(
            Vec3(0.0f, 0.8f, -5.0f),
            Vec3(-0.8f, -0.8f, -5.0f),
            Vec3(0.8f, -0.8f, -5.0f),
            0xFFFF0000, 0xFF00FF00, 0xFF0000FF // RGB
        ));
        
        // Triangle 5: Small fast-spinning triangle
        triangles.push_back(Triangle3D(
            Vec3(0.0f, 0.25f, -1.5f),
            Vec3(-0.25f, -0.25f, -1.5f),
            Vec3(0.25f, -0.25f, -1.5f),
            0xFFFF00FF, 0xFF00FF00, 0xFF0080FF // Magenta, Green, Light Blue
        ));
        
        // Triangle 6: Oscillating triangle
        triangles.push_back(Triangle3D(
            Vec3(0.0f, 0.4f, -3.5f),
            Vec3(-0.4f, -0.4f, -3.5f),
            Vec3(0.4f, -0.4f, -3.5f),
            0xFFFFFFFF, 0xFF808080, 0xFF000000 // White, Gray, Black
        ));
        
        // Triangle 7: Tumbling triangle (rotates around all axes)
        triangles.push_back(Triangle3D(
            Vec3(0.0f, 0.35f, -2.8f),
            Vec3(-0.35f, -0.35f, -2.8f),
            Vec3(0.35f, -0.35f, -2.8f),
            0xFFFF4040, 0xFF40FF40, 0xFF4040FF // Light RGB
        ));
        
        // Triangle 8: Wobbling triangle with complex motion
        triangles.push_back(Triangle3D(
            Vec3(0.0f, 0.3f, -6.0f),
            Vec3(-0.3f, -0.3f, -6.0f),
            Vec3(0.3f, -0.3f, -6.0f),
            0xFFFFB6C1, 0xFFB6FFB6, 0xFFB6B6FF // Pastel colors
        ));
        
        // Create transformation matrices for each triangle
        std::vector<Matrix4x4> transformations;
        
        // Transform 1: Y-axis rotation (clockwise)
        Matrix4x4 trans1 = Matrix4x4::translation(-2.5f, 1.0f, 0.0f) *
                           Matrix4x4::rotationY(rotation_time * 1.0f) *
                           projection;
        transformations.push_back(trans1);
        
        // Transform 2: X-axis rotation (counter-clockwise, faster)
        Matrix4x4 trans2 = Matrix4x4::translation(0.0f, 1.0f, 0.0f) *
                           Matrix4x4::rotationX(-rotation_time * 1.5f) *
                           projection;
        transformations.push_back(trans2);
        
        // Transform 3: Z-axis rotation (slow clockwise)
        Matrix4x4 trans3 = Matrix4x4::translation(2.5f, 1.0f, 0.0f) *
                           Matrix4x4::rotationZ(rotation_time * 0.7f) *
                           projection;
        transformations.push_back(trans3);
        
        // Transform 4: Multiple axes rotation (very slow)
        Matrix4x4 trans4 = Matrix4x4::translation(0.0f, -1.0f, 0.0f) *
                           Matrix4x4::rotationY(-rotation_time * 0.3f) *
                           Matrix4x4::rotationX(rotation_time * 0.2f) *
                           projection;
        transformations.push_back(trans4);
        
        // Transform 5: Fast spinning around Y and Z axes
        Matrix4x4 trans5 = Matrix4x4::translation(-3.0f, -0.5f, 0.0f) *
                           Matrix4x4::rotationY(rotation_time * 2.0f) *
                           Matrix4x4::rotationZ(rotation_time * 1.5f) *
                           projection;
        transformations.push_back(trans5);
        
        // Transform 6: Oscillating rotation (sine wave)
        float osc_angle = sin(rotation_time * 1.5f) * 0.5f;
        Matrix4x4 trans6 = Matrix4x4::translation(3.0f, -0.5f, 0.0f) *
                           Matrix4x4::rotationX(osc_angle) *
                           Matrix4x4::rotationY(osc_angle * 0.7f) *
                           projection;
        transformations.push_back(trans6);
        
        // Transform 7: Tumbling motion (all three axes)
        Matrix4x4 trans7 = Matrix4x4::translation(-1.5f, 0.0f, 0.0f) *
                           Matrix4x4::rotationX(rotation_time * 1.2f) *
                           Matrix4x4::rotationY(rotation_time * 0.8f) *
                           Matrix4x4::rotationZ(rotation_time * 0.5f) *
                           projection;
        transformations.push_back(trans7);
        
        // Transform 8: Complex wobble motion
        float wobble_x = sin(rotation_time * 2.0f) * 0.3f;
        float wobble_y = cos(rotation_time * 1.7f) * 0.3f;
        float wobble_z = sin(rotation_time * 0.9f) * cos(rotation_time * 1.1f) * 0.2f;
        Matrix4x4 trans8 = Matrix4x4::translation(1.5f, 0.0f, 0.0f) *
                           Matrix4x4::rotationX(wobble_x) *
                           Matrix4x4::rotationY(wobble_y) *
                           Matrix4x4::rotationZ(wobble_z) *
                           projection;
        transformations.push_back(trans8);
        
        // Render all triangles
        for (size_t i = 0; i < triangles.size() && i < transformations.size(); i++) {
            std::cout << "Rendering 3D triangle " << (i + 1) << "...\n" << std::flush;
            
            // Transform triangle to screen space
            Triangle3D transformed = triangles[i].transform(transformations[i]);
            
            // Check if triangle is facing towards camera (backface culling)
            Vec3 normal = transformed.getNormal();
            if (normal.z > 0) { // Triangle facing camera
                pixelBuffer.render3DTriangle(transformed, WINDOW_WIDTH, WINDOW_HEIGHT);
            }
        }
        
        // Add some additional 3D triangles for a more complex scene
        
        // Spinning pyramid faces (4 triangles forming a pyramid)
        Vec3 pyramid_center = Vec3(0.0f, 0.0f, -7.0f);
        float pyramid_size = 0.6f;
        
        // Pyramid base vertices
        Vec3 base1 = pyramid_center + Vec3(-pyramid_size, -pyramid_size, pyramid_size);
        Vec3 base2 = pyramid_center + Vec3(pyramid_size, -pyramid_size, pyramid_size);
        Vec3 base3 = pyramid_center + Vec3(pyramid_size, -pyramid_size, -pyramid_size);
        Vec3 base4 = pyramid_center + Vec3(-pyramid_size, -pyramid_size, -pyramid_size);
        Vec3 apex = pyramid_center + Vec3(0.0f, pyramid_size, 0.0f);
        
        // Create pyramid rotation
        Matrix4x4 pyramid_rot = Matrix4x4::rotationY(rotation_time * 0.5f) *
                               Matrix4x4::rotationX(rotation_time * 0.3f) *
                               projection;
        
        // Pyramid faces
        std::vector<Triangle3D> pyramid_faces = {
            Triangle3D(apex, base1, base2, 0xFFFF6666, 0xFFFF0000, 0xFFFF3333), // Red face
            Triangle3D(apex, base2, base3, 0xFF66FF66, 0xFF00FF00, 0xFF33FF33), // Green face
            Triangle3D(apex, base3, base4, 0xFF6666FF, 0xFF0000FF, 0xFF3333FF), // Blue face
            Triangle3D(apex, base4, base1, 0xFFFFFF66, 0xFFFFFF00, 0xFFFFFF33)  // Yellow face
        };
        
        for (auto& face : pyramid_faces) {
            Triangle3D transformed_face = face.transform(pyramid_rot);
            Vec3 normal = transformed_face.getNormal();
            if (normal.z > 0) {
                pixelBuffer.render3DTriangle(transformed_face, WINDOW_WIDTH, WINDOW_HEIGHT);
            }
        }
        
        std::cout << "=== 3D ROTATING TRIANGLES DEMO COMPLETE ===\n\n" << std::flush;
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
        // Update animation time
        uint32_t current_time = SDL_GetTicks();
        float delta_time = (current_time - last_time) / 1000.0f;
        last_time = current_time;
        rotation_time += delta_time;
        
        // Continuous animation - always redraw
        needsRedraw = true;
        
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
            drawScene();
            
            SDL_LockTexture(texture, NULL, &texturePixels, &pitch);
            memcpy(texturePixels, pixelBuffer.getData(), WINDOW_WIDTH * WINDOW_HEIGHT * 4);
            SDL_UnlockTexture(texture);

            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
            
            needsRedraw = false;
        }
        
        SDL_Delay(16); // ~60 FPS
    }

    std::cout << "Cleaning up SDL resources...\n" << std::flush;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    std::cout << "Program exiting normally\n" << std::flush;
    return 0;
}
