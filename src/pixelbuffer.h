#pragma once

#include <vector>
#include <cstdint>
#include "utils.h"

class PixelBuffer {
private:
    std::vector<uint32_t> pixels;
    int width, height;
    
    // Color utility functions for vertex color interpolation
    struct Color {
        float r, g, b, a;
        
        Color(uint32_t argb = 0xFF000000);
        Color(float red, float green, float blue, float alpha = 1.0f);
        
        uint32_t toARGB() const;
        Color operator+(const Color& other) const;
        Color operator*(float scalar) const;
    };

public:
    PixelBuffer(int w, int h);
    
    void clear(uint32_t color = 0xFF000000);
    void setPixel(int x, int y, uint32_t color);
    uint32_t getPixel(int x, int y) const;
    
    const uint32_t* getData() const;
    int getWidth() const;
    int getHeight() const;
    
    // Basic drawing functions
    void drawLine(int x0, int y0, int x1, int y1, uint32_t color);
    void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
    void fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
    void fillTriangleBarycentric(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
    void drawTriangleWireframe(int x0, int y0, int x1, int y1, int x2, int y2, 
                              uint32_t wireframe_color, uint32_t fill_color, bool filled = true);
    void fillRectangle(int x, int y, int w, int h, uint32_t color);
    
    // Advanced triangle rendering with gradients
    void fillTriangleGradient(int x0, int y0, uint32_t color0,
                             int x1, int y1, uint32_t color1,
                             int x2, int y2, uint32_t color2);
    void fillTriangleGradientScanline(int x0, int y0, uint32_t color0,
                                     int x1, int y1, uint32_t color1,
                                     int x2, int y2, uint32_t color2);
    void fillTriangleRainbow(int x0, int y0, int x1, int y1, int x2, int y2);
    
    // 3D rendering functions
    std::pair<int, int> project3DTo2D(const Vec3& point, int screenWidth, int screenHeight);
    void render3DTriangle(const Triangle3D& triangle, int screenWidth, int screenHeight);
};