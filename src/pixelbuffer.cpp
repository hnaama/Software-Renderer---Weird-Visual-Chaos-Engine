#include "pixelbuffer.h"
#include <algorithm>
#include <cstring>

// Color struct implementations
PixelBuffer::Color::Color(uint32_t argb) {
    a = ((argb >> 24) & 0xFF) / 255.0f;
    r = ((argb >> 16) & 0xFF) / 255.0f;
    g = ((argb >> 8) & 0xFF) / 255.0f;
    b = (argb & 0xFF) / 255.0f;
}

PixelBuffer::Color::Color(float red, float green, float blue, float alpha) 
    : r(red), g(green), b(blue), a(alpha) {}

uint32_t PixelBuffer::Color::toARGB() const {
    uint8_t ai = (uint8_t)(a * 255.0f);
    uint8_t ri = (uint8_t)(r * 255.0f);
    uint8_t gi = (uint8_t)(g * 255.0f);
    uint8_t bi = (uint8_t)(b * 255.0f);
    return (ai << 24) | (ri << 16) | (gi << 8) | bi;
}

PixelBuffer::Color PixelBuffer::Color::operator+(const Color& other) const {
    return Color(r + other.r, g + other.g, b + other.b, a + other.a);
}

PixelBuffer::Color PixelBuffer::Color::operator*(float scalar) const {
    return Color(r * scalar, g * scalar, b * scalar, a * scalar);
}

// PixelBuffer implementations
PixelBuffer::PixelBuffer(int w, int h) : width(w), height(h) {
    pixels.resize(w * h);
}

void PixelBuffer::clear(uint32_t color) {
    std::fill(pixels.begin(), pixels.end(), color);
}

void PixelBuffer::setPixel(int x, int y, uint32_t color) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        pixels[y * width + x] = color;
    }
}

uint32_t PixelBuffer::getPixel(int x, int y) const {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        return pixels[y * width + x];
    }
    return 0;
}

const uint32_t* PixelBuffer::getData() const { return pixels.data(); }
int PixelBuffer::getWidth() const { return width; }
int PixelBuffer::getHeight() const { return height; }

void PixelBuffer::drawLine(int x0, int y0, int x1, int y1, uint32_t color) {
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

void PixelBuffer::drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x0, y0, color);
}

void PixelBuffer::fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
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

void PixelBuffer::fillTriangleBarycentric(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
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

void PixelBuffer::drawTriangleWireframe(int x0, int y0, int x1, int y1, int x2, int y2, 
                              uint32_t wireframe_color, uint32_t fill_color, bool filled) {
    if (filled) {
        fillTriangleBarycentric(x0, y0, x1, y1, x2, y2, fill_color);
    }
    // Draw wireframe on top
    drawLine(x0, y0, x1, y1, wireframe_color);
    drawLine(x1, y1, x2, y2, wireframe_color);
    drawLine(x2, y2, x0, y0, wireframe_color);
}

void PixelBuffer::fillRectangle(int x, int y, int w, int h, uint32_t color) {
    for (int py = y; py < y + h; py++) {
        for (int px = x; px < x + w; px++) {
            setPixel(px, py, color);
        }
    }
}

void PixelBuffer::fillTriangleGradient(int x0, int y0, uint32_t color0,
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

void PixelBuffer::fillTriangleGradientScanline(int x0, int y0, uint32_t color0,
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

void PixelBuffer::fillTriangleRainbow(int x0, int y0, int x1, int y1, int x2, int y2) {
    fillTriangleGradient(
        x0, y0, 0xFFFF0000,  // Red vertex
        x1, y1, 0xFF00FF00,  // Green vertex  
        x2, y2, 0xFF0000FF   // Blue vertex
    );
}

std::pair<int, int> PixelBuffer::project3DTo2D(const Vec3& point, int screenWidth, int screenHeight) {
    // Simple perspective projection (assuming point is already in normalized device coordinates)
    int x = (int)((point.x + 1.0f) * 0.5f * screenWidth);
    int y = (int)((1.0f - point.y) * 0.5f * screenHeight); // Flip Y axis
    return {x, y};
}

void PixelBuffer::render3DTriangle(const Triangle3D& triangle, int screenWidth, int screenHeight) {
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