#include <SDL.h>
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <random>
#include <algorithm>

// Random number generator setup
std::random_device rd;
std::mt19937 rng(rd());

// Helper function to get random float in range
float randomFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}

// Helper function to get random int in range
int randomInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

// Helper function to get random color
uint32_t randomColor() {
    return 0xFF000000 | (randomInt(0, 255) << 16) | (randomInt(0, 255) << 8) | randomInt(0, 255);
}

// HSV to RGB color conversion - moved here before first usage
uint32_t hsvToRgb(float h, float s, float v) {
    float c = v * s;
    float x = c * (1 - abs(fmod(h / 60.0f, 2) - 1));
    float m = v - c;
    
    float r, g, b;
    if (h >= 0 && h < 60) {
        r = c; g = x; b = 0;
    } else if (h >= 60 && h < 120) {
        r = x; g = c; b = 0;
    } else if (h >= 120 && h < 180) {
        r = 0; g = c; b = x;
    } else if (h >= 180 && h < 240) {
        r = 0; g = x; b = c;
    } else if (h >= 240 && h < 300) {
        r = x; g = 0; b = c;
    } else {
        r = c; g = 0; b = x;
    }
    
    uint8_t ri = (uint8_t)((r + m) * 255);
    uint8_t gi = (uint8_t)((g + m) * 255);
    uint8_t bi = (uint8_t)((b + m) * 255);
    
    return 0xFF000000 | (ri << 16) | (gi << 8) | bi;
}

// Enhanced color utility functions for more vibrant visuals
uint32_t createRainbowColor(float t) {
    // Create smooth rainbow transitions
    float r = (sin(t * 6.28f) * 0.5f + 0.5f);
    float g = (sin(t * 6.28f + 2.09f) * 0.5f + 0.5f);  // 2π/3 offset
    float b = (sin(t * 6.28f + 4.19f) * 0.5f + 0.5f);  // 4π/3 offset
    
    return 0xFF000000 | 
           ((int)(r * 255) << 16) | 
           ((int)(g * 255) << 8) | 
           (int)(b * 255);
}

uint32_t createNeonColor(float intensity, float hueShift = 0) {
    // Create bright neon-like colors with saturation boost
    float hue = fmod(intensity * 360.0f + hueShift, 360.0f);
    return hsvToRgb(hue, 1.0f, std::min(1.0f, intensity * 1.5f)); // Boost brightness
}

uint32_t blendColors(uint32_t color1, uint32_t color2, float t) {
    // Smooth color blending
    t = std::max(0.0f, std::min(1.0f, t));
    
    uint8_t r1 = (color1 >> 16) & 0xFF, g1 = (color1 >> 8) & 0xFF, b1 = color1 & 0xFF;
    uint8_t r2 = (color2 >> 16) & 0xFF, g2 = (color2 >> 8) & 0xFF, b2 = color2 & 0xFF;
    
    uint8_t r = (uint8_t)(r1 + t * (r2 - r1));
    uint8_t g = (uint8_t)(g1 + t * (g2 - g1));
    uint8_t b = (uint8_t)(b1 + t * (b2 - b1));
    
    return 0xFF000000 | (r << 16) | (g << 8) | b;
}

// Fractal computation functions - OPTIMIZED FOR PERFORMANCE
float computeMandelbrot(float x, float y) {
    float cx = x, cy = y;
    float zx = 0, zy = 0;
    int maxIterations = 25; // Reduced from 50 for better performance
    
    for (int i = 0; i < maxIterations; i++) {
        float zx2 = zx * zx;
        float zy2 = zy * zy;
        if (zx2 + zy2 > 16.0f) { // Increased bailout for smoother gradients
            return i / (float)maxIterations;
        }
        float temp = zx2 - zy2 + cx;
        zy = 2 * zx * zy + cy;
        zx = temp;
    }
    return 1.0f;
}

float computeJulia(float x, float y, float cx, float cy) {
    float zx = x, zy = y;
    int maxIterations = 25; // Reduced from 50
    
    for (int i = 0; i < maxIterations; i++) {
        float zx2 = zx * zx;
        float zy2 = zy * zy;
        if (zx2 + zy2 > 16.0f) {
            return i / (float)maxIterations;
        }
        float temp = zx2 - zy2 + cx;
        zy = 2 * zx * zy + cy;
        zx = temp;
    }
    return 1.0f;
}

float computeBurningShip(float x, float y) {
    float cx = x, cy = y;
    float zx = 0, zy = 0;
    int maxIterations = 20; // Further reduced for this complex fractal
    
    for (int i = 0; i < maxIterations; i++) {
        float zx2 = zx * zx;
        float zy2 = zy * zy;
        if (zx2 + zy2 > 16.0f) {
            return i / (float)maxIterations;
        }
        float temp = zx2 - zy2 + cx;
        zy = abs(2 * zx * zy) + cy;
        zx = abs(temp);
    }
    return 1.0f;
}

float computeTricorn(float x, float y) {
    float cx = x, cy = y;
    float zx = 0, zy = 0;
    int maxIterations = 50;
    
    for (int i = 0; i < maxIterations; i++) {
        float zx2 = zx * zx;
        float zy2 = zy * zy;
        if (zx2 + zy2 > 4.0f) {
            return i / (float)maxIterations;
        }
        float temp = zx2 - zy2 + cx;
        zy = -2 * zx * zy + cy;
        zx = temp;
    }
    return 1.0f;
}

float computePhoenix(float x, float y) {
    float cx = x, cy = y;
    float zx = 0, zy = 0;
    float px = 0, py = 0;
    int maxIterations = 50;
    
    for (int i = 0; i < maxIterations; i++) {
        float zx2 = zx * zx;
        float zy2 = zy * zy;
        if (zx2 + zy2 > 4.0f) {
            return i / (float)maxIterations;
        }
        float temp = zx2 - zy2 + cx + 0.5f * px;
        float temp_y = 2 * zx * zy + cy + 0.5f * py;
        px = zx; py = zy;
        zx = temp; zy = temp_y;
    }
    return 1.0f;
}

float computeNova(float x, float y) {
    float zx = x, zy = y;
    int maxIterations = 50;
    
    for (int i = 0; i < maxIterations; i++) {
        float zx3 = zx * zx * zx - 3 * zx * zy * zy;
        float zy3 = 3 * zx * zx * zy - zy * zy * zy;
        
        // z^3 - 1 = 0 Newton's method
        float denominator = 9 * (zx * zx + zy * zy);
        if (denominator < 0.001f) return i / (float)maxIterations;
        
        float newZx = zx - (zx3 - zx) / denominator;
        float newZy = zy - zy3 / denominator;
        
        if (abs(newZx - zx) + abs(newZy - zy) < 0.001f) {
            return i / (float)maxIterations;
        }
        zx = newZx; zy = newZy;
    }
    return 1.0f;
}

float computePsychedelicWaves(float x, float y) {
    return (sin(x * 5.0f) * cos(y * 3.0f) + sin(x * y * 2.0f) + cos(x + y)) * 0.5f + 0.5f;
}

float computeStrangeAttractor(float x, float y) {
    float a = 1.4f, b = 0.3f;
    for (int i = 0; i < 10; i++) {
        float newX = 1 - a * x * x + y;
        float newY = b * x;
        x = newX; y = newY;
    }
    return fmod(sqrt(x * x + y * y), 1.0f);
}

float computeChaosFractal(float x, float y) {
    float result = 0;
    for (int i = 0; i < 5; i++) {
        result += sin(x * (i + 1) * 2.0f) * cos(y * (i + 1) * 1.5f) / (i + 1);
    }
    return fmod(abs(result), 1.0f);
}

// Game of Life injection functions - forward declarations
void injectSpinner(int cx, int cy);
void injectGlider(int cx, int cy);
void injectExploder(int cx, int cy);
void injectChaosBlob(int cx, int cy);
void injectEnergyVortex(int cx, int cy);

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

// Weird Visual Entity base class
struct WeirdEntity {
    Vec3 position;
    Vec3 velocity;
    Vec3 size;
    float rotation;
    float rotationSpeed;
    float life;
    float maxLife;
    std::vector<uint32_t> colors;
    int type;
    float morphTime;
    
    WeirdEntity(Vec3 pos) : position(pos), morphTime(0.0f) {
        velocity = Vec3(randomFloat(-2, 2), randomFloat(-2, 2), randomFloat(-1, 1));
        size = Vec3(randomFloat(0.1f, 0.8f), randomFloat(0.1f, 0.8f), randomFloat(0.1f, 0.8f));
        rotation = randomFloat(0, 2 * M_PI);
        rotationSpeed = randomFloat(-3, 3);
        maxLife = randomFloat(5.0f, 15.0f);
        life = maxLife;
        type = randomInt(0, 6); // Different entity types
        
        // Generate weird color palette
        int numColors = randomInt(3, 6);
        for (int i = 0; i < numColors; i++) {
            colors.push_back(randomColor());
        }
    }
    
    void update(float deltaTime, int screenWidth, int screenHeight) {
        life -= deltaTime;
        morphTime += deltaTime;
        
        // Update position with some chaos
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;
        position.z += velocity.z * deltaTime;
        
        // Weird physics - entities can bounce, wrap, or teleport
        if (randomFloat(0, 1) < 0.01f) { // 1% chance per frame for chaos
            switch (randomInt(0, 3)) {
                case 0: // Bounce
                    velocity.x *= -1.2f;
                    velocity.y *= -1.2f;
                    break;
                case 1: // Random teleport
                    position.x = randomFloat(-3, 3);
                    position.y = randomFloat(-2, 2);
                    break;
                case 2: // Speed up
                    velocity = velocity * randomFloat(1.5f, 2.0f);
                    break;
                case 3: // Change direction
                    velocity = Vec3(randomFloat(-3, 3), randomFloat(-3, 3), randomFloat(-1, 1));
                    break;
            }
        }
        
        // Wrap around screen
        if (position.x > 4) position.x = -4;
        if (position.x < -4) position.x = 4;
        if (position.y > 3) position.y = -3;
        if (position.y < -3) position.y = 3;
        
        // Update rotation
        rotation += rotationSpeed * deltaTime;
        
        // Morphing size
        float morphFactor = sin(morphTime * randomFloat(1, 3)) * 0.3f + 1.0f;
        size = size * morphFactor;
    }
    
    bool isDead() const {
        return life <= 0;
    }
    
    std::vector<Triangle3D> generateTriangles() const {
        std::vector<Triangle3D> triangles;
        float lifeFactor = life / maxLife;
        
        switch (type) {
            case 0: // Spiky Star
                generateSpikyTriangles(triangles, lifeFactor);
                break;
            case 1: // Morphing Blob
                generateBlobTriangles(triangles, lifeFactor);
                break;
            case 2: // Fractal Spikes
                generateFractalTriangles(triangles, lifeFactor);
                break;
            case 3: // Twisted Ribbon
                generateRibbonTriangles(triangles, lifeFactor);
                break;
            case 4: // Pulsing Orb
                generateOrbTriangles(triangles, lifeFactor);
                break;
            case 5: // Chaotic Fragments
                generateFragmentTriangles(triangles, lifeFactor);
                break;
            default: // Weird Polyhedron
                generatePolyhedronTriangles(triangles, lifeFactor);
                break;
        }
        
        return triangles;
    }
    
private:
    void generateSpikyTriangles(std::vector<Triangle3D>& triangles, float lifeFactor) const {
        int spikes = 8 + (int)(sin(morphTime) * 4);
        for (int i = 0; i < spikes; i++) {
            float angle = (i / (float)spikes) * 2 * M_PI + rotation;
            float innerRadius = size.x * 0.3f;
            float outerRadius = size.x * (1.0f + sin(morphTime * 2 + i) * 0.5f);
            
            Vec3 center = position;
            Vec3 inner1 = center + Vec3(cos(angle) * innerRadius, sin(angle) * innerRadius, 0);
            Vec3 outer = center + Vec3(cos(angle + 0.1f) * outerRadius, sin(angle + 0.1f) * outerRadius, sin(morphTime + i) * 0.2f);
            Vec3 inner2 = center + Vec3(cos(angle + 0.2f) * innerRadius, sin(angle + 0.2f) * innerRadius, 0);
            
            uint32_t color1 = colors[i % colors.size()];
            uint32_t color2 = colors[(i + 1) % colors.size()];
            uint32_t color3 = colors[(i + 2) % colors.size()];
            
            triangles.push_back(Triangle3D(inner1, outer, inner2, color1, color2, color3));
        }
    }
    
    void generateBlobTriangles(std::vector<Triangle3D>& triangles, float lifeFactor) const {
        int segments = 12;
        for (int i = 0; i < segments; i++) {
            float angle1 = (i / (float)segments) * 2 * M_PI;
            float angle2 = ((i + 1) / (float)segments) * 2 * M_PI;
            
            float noise1 = sin(morphTime * 2 + angle1 * 3) * 0.3f + 1.0f;
            float noise2 = sin(morphTime * 2 + angle2 * 3) * 0.3f + 1.0f;
            
            Vec3 center = position;
            Vec3 p1 = center + Vec3(cos(angle1) * size.x * noise1, sin(angle1) * size.y * noise1, 0);
            Vec3 p2 = center + Vec3(cos(angle2) * size.x * noise2, sin(angle2) * size.y * noise2, 0);
            Vec3 p3 = center + Vec3(0, 0, size.z * sin(morphTime + i));
            
            uint32_t c1 = colors[i % colors.size()];
            uint32_t c2 = colors[(i + 1) % colors.size()];
            uint32_t c3 = colors[(i + 2) % colors.size()];
            
            triangles.push_back(Triangle3D(center, p1, p2, c1, c2, c3));
            triangles.push_back(Triangle3D(p1, p2, p3, c1, c2, c3));
        }
    }
    
    void generateFractalTriangles(std::vector<Triangle3D>& triangles, float lifeFactor) const {
        // Generate fractal-like recursive triangles
        generateFractalLevel(triangles, position, size.x, 0, 3);
    }
    
    void generateFractalLevel(std::vector<Triangle3D>& triangles, Vec3 center, float scale, int level, int maxLevel) const {
        if (level >= maxLevel) return;
        
        float angleOffset = rotation + level * 0.7f + morphTime;
        for (int i = 0; i < 3; i++) {
            float angle = (i / 3.0f) * 2 * M_PI + angleOffset;
            Vec3 offset = Vec3(cos(angle) * scale, sin(angle) * scale, sin(morphTime + level) * scale * 0.3f);
            Vec3 newCenter = center + offset;
            
            // Create triangle at this level
            Vec3 p1 = newCenter + Vec3(cos(angle) * scale * 0.3f, sin(angle) * scale * 0.3f, 0);
            Vec3 p2 = newCenter + Vec3(cos(angle + 2.1f) * scale * 0.3f, sin(angle + 2.1f) * scale * 0.3f, 0);
            Vec3 p3 = newCenter + Vec3(0, 0, scale * 0.5f);
            
            uint32_t color = colors[(level + i) % colors.size()];
            triangles.push_back(Triangle3D(p1, p2, p3, color, color, color));
            
            // Recurse
            generateFractalLevel(triangles, newCenter, scale * 0.6f, level + 1, maxLevel);
        }
    }
    
    void generateRibbonTriangles(std::vector<Triangle3D>& triangles, float lifeFactor) const {
        int segments = 20;
        for (int i = 0; i < segments - 1; i++) {
            float t1 = i / (float)segments;
            float t2 = (i + 1) / (float)segments;
            
            // Create twisted ribbon path
            Vec3 p1 = position + Vec3(
                cos(t1 * 4 * M_PI + morphTime) * size.x,
                sin(t1 * 2 * M_PI + morphTime) * size.y,
                (t1 - 0.5f) * size.z * 2
            );
            Vec3 p2 = position + Vec3(
                cos(t2 * 4 * M_PI + morphTime) * size.x,
                sin(t2 * 2 * M_PI + morphTime) * size.y,
                (t2 - 0.5f) * size.z * 2
            );
            
            // Ribbon width
            Vec3 width1 = Vec3(sin(t1 * 6 + morphTime) * 0.1f, cos(t1 * 6 + morphTime) * 0.1f, 0);
            Vec3 width2 = Vec3(sin(t2 * 6 + morphTime) * 0.1f, cos(t2 * 6 + morphTime) * 0.1f, 0);
            
            triangles.push_back(Triangle3D(
                p1 + width1, p1 - width1, p2 + width2,
                colors[i % colors.size()], colors[(i+1) % colors.size()], colors[(i+2) % colors.size()]
            ));
            triangles.push_back(Triangle3D(
                p1 - width1, p2 - width2, p2 + width2,
                colors[i % colors.size()], colors[(i+1) % colors.size()], colors[(i+2) % colors.size()]
            ));
        }
    }
    
    void generateOrbTriangles(std::vector<Triangle3D>& triangles, float lifeFactor) const {
        int rings = 6;
        int segments = 8;
        
        for (int ring = 0; ring < rings; ring++) {
            float ringHeight = (ring / (float)rings - 0.5f) * size.z * 2;
            float ringRadius = sqrt(1.0f - pow(ringHeight / size.z, 2)) * size.x;
            ringRadius *= (1.0f + sin(morphTime * 2 + ring) * 0.3f); // Pulsing
            
            for (int seg = 0; seg < segments; seg++) {
                float angle1 = (seg / (float)segments) * 2 * M_PI + rotation;
                float angle2 = ((seg + 1) / (float)segments) * 2 * M_PI + rotation;
                
                Vec3 p1 = position + Vec3(cos(angle1) * ringRadius, sin(angle1) * ringRadius, ringHeight);
                Vec3 p2 = position + Vec3(cos(angle2) * ringRadius, sin(angle2) * ringRadius, ringHeight);
                Vec3 center = position + Vec3(0, 0, ringHeight);
                
                triangles.push_back(Triangle3D(center, p1, p2, 
                    colors[ring % colors.size()], 
                    colors[(ring + seg) % colors.size()], 
                    colors[(ring + seg + 1) % colors.size()]));
            }
        }
    }
    
    void generateFragmentTriangles(std::vector<Triangle3D>& triangles, float lifeFactor) const {
        int fragments = 15;
        for (int i = 0; i < fragments; i++) {
            Vec3 offset = Vec3(
                sin(morphTime + i) * size.x * (1 + i * 0.1f),
                cos(morphTime * 1.3f + i) * size.y * (1 + i * 0.1f),
                sin(morphTime * 0.7f + i) * size.z
            );
            
            Vec3 center = position + offset;
            float fragSize = size.x * 0.2f * (1.0f - i * 0.05f);
            
            // Random triangle orientation
            float angle = rotation + i * 0.8f + morphTime;
            Vec3 p1 = center + Vec3(cos(angle) * fragSize, sin(angle) * fragSize, 0);
            Vec3 p2 = center + Vec3(cos(angle + 2.1f) * fragSize, sin(angle + 2.1f) * fragSize, 0);
            Vec3 p3 = center + Vec3(cos(angle + 4.2f) * fragSize, sin(angle + 4.2f) * fragSize, fragSize);
            
            triangles.push_back(Triangle3D(p1, p2, p3, 
                colors[i % colors.size()], 
                colors[(i + 1) % colors.size()], 
                colors[(i + 2) % colors.size()]));
        }
    }
    
    void generatePolyhedronTriangles(std::vector<Triangle3D>& triangles, float lifeFactor) const {
        // Create weird polyhedron with morphing vertices
        std::vector<Vec3> vertices;
        
        // Generate base vertices of a deformed polyhedron
        for (int i = 0; i < 8; i++) {
            float angle = (i / 8.0f) * 2 * M_PI;
            float radius = size.x * (1.0f + sin(morphTime * 3 + i) * 0.4f);
            float height = (i % 2 == 0 ? size.z : -size.z) * (1.0f + cos(morphTime * 2 + i) * 0.3f);
            
            vertices.push_back(position + Vec3(
                cos(angle + rotation) * radius,
                sin(angle + rotation) * radius,
                height
            ));
        }
        
        // Connect vertices in weird ways
        std::vector<std::tuple<int, int, int>> faces = {
            {0, 1, 2}, {2, 3, 4}, {4, 5, 6}, {6, 7, 0},
            {0, 2, 4}, {4, 6, 0}, {1, 3, 5}, {5, 7, 1},
            {0, 1, 7}, {1, 2, 3}, {3, 4, 5}, {5, 6, 7}
        };
        
        for (auto& face : faces) {
            int i1 = std::get<0>(face);
            int i2 = std::get<1>(face);
            int i3 = std::get<2>(face);
            
            triangles.push_back(Triangle3D(
                vertices[i1], vertices[i2], vertices[i3],
                colors[i1 % colors.size()], 
                colors[i2 % colors.size()], 
                colors[i3 % colors.size()]
            ));
        }
    }
};

// Weird Visual Manager
class WeirdVisualManager {
private:
    std::vector<std::unique_ptr<WeirdEntity>> entities;
    float spawnTimer;
    float spawnInterval;
    int maxEntities;
    
public:
    WeirdVisualManager() : spawnTimer(0), spawnInterval(randomFloat(0.5f, 2.0f)), maxEntities(randomInt(8, 20)) {}
    
    void update(float deltaTime) {
        // Update existing entities
        for (auto& entity : entities) {
            entity->update(deltaTime, 800, 600);
        }
        
        // Remove dead entities
        entities.erase(
            std::remove_if(entities.begin(), entities.end(),
                [](const std::unique_ptr<WeirdEntity>& entity) {
                    return entity->isDead();
                }),
            entities.end()
        );
        
        // Spawn new entities
        spawnTimer += deltaTime;
        if (spawnTimer >= spawnInterval && entities.size() < maxEntities) {
            spawnTimer = 0;
            spawnInterval = randomFloat(0.3f, 2.5f); // Random spawn intervals
            
            Vec3 spawnPos = Vec3(
                randomFloat(-3, 3),
                randomFloat(-2, 2),
                randomFloat(-6, -2)
            );
            
            entities.push_back(std::make_unique<WeirdEntity>(spawnPos));
        }
        
        // Occasionally change max entities for chaos
        if (randomFloat(0, 1) < 0.01f) {
            maxEntities = randomInt(5, 25);
        }
    }
    
    std::vector<Triangle3D> getAllTriangles() const {
        std::vector<Triangle3D> allTriangles;
        
        for (const auto& entity : entities) {
            auto entityTriangles = entity->generateTriangles();
            allTriangles.insert(allTriangles.end(), entityTriangles.begin(), entityTriangles.end());
        }
        
        return allTriangles;
    }
    
    size_t getEntityCount() const {
        return entities.size();
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

// EXTREME HALLUCINOGENIC FRACTAL/GAME OF LIFE SYSTEM
class FractalGameOfLifeSystem {
private:
    int width, height;
    std::vector<std::vector<float>> grid;
    std::vector<std::vector<float>> nextGrid;
    std::vector<std::vector<float>> energyGrid;
    std::vector<std::vector<float>> velocityX, velocityY;
    std::vector<std::vector<uint32_t>> colorGrid;
    std::vector<std::vector<float>> trailGrid;
    float time;
    int fractalType;
    float zoomLevel;
    Vec3 center;
    float warpIntensity;
    float colorShift;
    float pulseSpeed;
    float chaosLevel;
    bool isTripping;
    std::vector<Vec3> attractors;
    
public:
    FractalGameOfLifeSystem(int w, int h) : width(w), height(h), time(0), fractalType(0), 
        zoomLevel(1.0f), center(0, 0, 0), warpIntensity(1.0f), colorShift(0), 
        pulseSpeed(1.0f), chaosLevel(0.5f), isTripping(false) {
        
        grid.resize(height, std::vector<float>(width, 0.0f));
        nextGrid.resize(height, std::vector<float>(width, 0.0f));
        energyGrid.resize(height, std::vector<float>(width, 0.0f));
        velocityX.resize(height, std::vector<float>(width, 0.0f));
        velocityY.resize(height, std::vector<float>(width, 0.0f));
        colorGrid.resize(height, std::vector<uint32_t>(width, 0xFF000000));
        trailGrid.resize(height, std::vector<float>(width, 0.0f));
        
        initialize();
    }
    
    void initialize() {
        // Seed with psychedelic patterns
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                float noise1 = sin(x * 0.1f) * cos(y * 0.08f);
                float noise2 = sin(x * 0.03f + y * 0.05f) * 0.5f;
                grid[y][x] = (noise1 + noise2 + randomFloat(-0.5f, 0.5f)) > 0 ? randomFloat(0.3f, 1.0f) : 0.0f;
                energyGrid[y][x] = randomFloat(0, 0.5f);
                trailGrid[y][x] = 0.0f;
            }
        }
        
        fractalType = randomInt(0, 8);
        zoomLevel = randomFloat(0.05f, 5.0f);
        center = Vec3(randomFloat(-3, 3), randomFloat(-3, 3), 0);
        warpIntensity = randomFloat(0.5f, 3.0f);
        pulseSpeed = randomFloat(0.5f, 3.0f);
        chaosLevel = randomFloat(0.2f, 1.0f);
        
        attractors.clear();
        int numAttractors = randomInt(2, 6);
        for (int i = 0; i < numAttractors; i++) {
            attractors.push_back(Vec3(randomFloat(-2, 2), randomFloat(-2, 2), randomFloat(-1, 1)));
        }
    }
    
    void update(float deltaTime) {
        time += deltaTime * pulseSpeed;
        
        // Much more frequent state changes for maximum chaos
        if (randomFloat(0, 1) < 0.05f) { // 5% chance per frame instead of 0.8%
            isTripping = !isTripping;
            if (isTripping) {
                warpIntensity = randomFloat(5.0f, 15.0f); // Extreme warp
                pulseSpeed = randomFloat(3.0f, 8.0f);     // Super fast
                chaosLevel = randomFloat(0.9f, 1.0f);     // Maximum chaos
                fractalType = randomInt(0, 12);           // More fractal types
                zoomLevel *= randomFloat(0.3f, 3.0f);     // Rapid zoom changes
            } else {
                warpIntensity = randomFloat(2.0f, 6.0f);  // Still intense when not tripping
                pulseSpeed = randomFloat(1.5f, 4.0f);     // Faster baseline
                chaosLevel = randomFloat(0.6f, 0.9f);     // High baseline chaos
            }
        }
        
        // Constantly shifting parameters for maximum hallucination
        colorShift += deltaTime * randomFloat(2.0f, 8.0f);
        zoomLevel *= 1.0f + sin(time * 2.0f) * 0.1f * chaosLevel;
        
        // Move center in chaotic patterns
        center.x += sin(time * 1.7f + colorShift) * chaosLevel * 0.05f;
        center.y += cos(time * 1.3f + colorShift * 0.7f) * chaosLevel * 0.05f;
        center.z += sin(time * 0.9f) * chaosLevel * 0.02f;
        
        // Update attractors for dynamic field effects
        for (auto& attractor : attractors) {
            attractor.x += sin(time * randomFloat(1.0f, 3.0f)) * chaosLevel * 0.1f;
            attractor.y += cos(time * randomFloat(1.0f, 3.0f)) * chaosLevel * 0.1f;
            attractor.z += sin(time * randomFloat(0.5f, 2.0f)) * chaosLevel * 0.05f;
        }
        
        // Extreme game of life with multiple rule sets
        for (int y = 1; y < height - 1; y++) {
            for (int x = 1; x < width - 1; x++) {
                float current = grid[y][x];
                
                // Multiple overlapping neighborhood calculations for chaos
                float neighbors1 = 0, neighbors2 = 0, neighbors3 = 0;
                
                // Standard neighbors
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        if (dx == 0 && dy == 0) continue;
                        neighbors1 += grid[y + dy][x + dx];
                    }
                }
                
                // Extended neighbors (2-cell radius) for more complex patterns
                for (int dy = -2; dy <= 2; dy++) {
                    for (int dx = -2; dx <= 2; dx++) {
                        if (dx == 0 && dy == 0) continue;
                        if (y + dy >= 0 && y + dy < height && x + dx >= 0 && x + dx < width) {
                            neighbors2 += grid[y + dy][x + dx] * 0.3f;
                        }
                    }
                }
                
                // Diagonal-only neighbors for extra patterns
                for (int i = -2; i <= 2; i++) {
                    for (int j = -2; j <= 2; j++) {
                        if (abs(i) == abs(j) && i != 0) {
                            if (y + i >= 0 && y + i < height && x + j >= 0 && x + j < width) {
                                neighbors3 += grid[y + i][x + j] * 0.5f;
                            }
                        }
                    }
                }
                
                // Combine all neighbor calculations with chaos
                float totalNeighbors = neighbors1 + neighbors2 * chaosLevel + neighbors3 * sin(time + x * 0.1f);
                
                // Multiple rule sets that change dynamically
                float newValue = current;
                int ruleSet = (int)(time * 2.0f + x * 0.1f + y * 0.08f) % 6;
                
                switch (ruleSet) {
                    case 0: // Classic Conway with chaos
                        if (current > 0.1f) {
                            newValue = (totalNeighbors >= 2.0f && totalNeighbors <= 3.5f) ? current * 1.1f : current * 0.8f;
                        } else {
                            newValue = (totalNeighbors >= 2.8f && totalNeighbors <= 3.2f) ? randomFloat(0.5f, 1.0f) : 0;
                        }
                        break;
                        
                    case 1: // High-life rules
                        if (current > 0.1f) {
                            newValue = (totalNeighbors >= 2.0f && totalNeighbors <= 3.0f) ? current * 1.05f : current * 0.9f;
                        } else {
                            newValue = (totalNeighbors >= 3.5f && totalNeighbors <= 4.0f) ? randomFloat(0.3f, 0.8f) : 0;
                        }
                        break;
                        
                    case 2: // Seeds - explosive growth
                        newValue = (totalNeighbors >= 2.0f) ? randomFloat(0.4f, 1.2f) : current * 0.95f;
                        break;
                        
                    case 3: // Day & Night - inverted
                        if (current > 0.1f) {
                            newValue = (totalNeighbors >= 3.0f && totalNeighbors <= 4.0f) ? current * 1.2f : current * 0.7f;
                        } else {
                            newValue = (totalNeighbors >= 3.0f && totalNeighbors <= 4.0f) ? randomFloat(0.6f, 1.0f) : 0;
                        }
                        break;
                        
                    case 4: { // Continuous life - smooth transitions
                        float smoothFactor = sin(totalNeighbors * 0.5f + time) * 0.5f + 0.5f;
                        newValue = current * 0.9f + smoothFactor * chaosLevel * 0.3f;
                        break;
                    }
                        
                    case 5: { // Chaos mode - pure randomness influenced by neighbors
                        newValue = current * 0.8f + randomFloat(0, totalNeighbors * 0.2f * chaosLevel);
                        break;
                    }
                }
                
                // Add fractal influences to the cellular automaton
                float fx = (x - width * 0.5f) / (width * 0.5f) * zoomLevel + center.x;
                float fy = (y - height * 0.5f) / (height * 0.5f) * zoomLevel + center.y;
                
                // Apply extreme warp distortion
                float warpX = fx + sin(time * 2.0f + fy * 3.0f) * warpIntensity * 0.5f;
                float warpY = fy + cos(time * 1.5f + fx * 2.0f) * warpIntensity * 0.5f;
                
                float fractalValue = 0;
                switch (fractalType % 9) {
                    case 0: fractalValue = computeMandelbrot(warpX, warpY); break;
                    case 1: fractalValue = computeJulia(warpX, warpY, sin(time * 0.5f), cos(time * 0.7f)); break;
                    case 2: fractalValue = computeBurningShip(warpX, warpY); break;
                    case 3: fractalValue = computeTricorn(warpX, warpY); break;
                    case 4: fractalValue = computePhoenix(warpX, warpY); break;
                    case 5: fractalValue = computeNova(warpX, warpY); break;
                    case 6: fractalValue = computePsychedelicWaves(warpX, warpY); break;
                    case 7: fractalValue = computeStrangeAttractor(warpX, warpY); break;
                    case 8: fractalValue = computeChaosFractal(warpX, warpY); break;
                }
                
                // Blend cellular automaton with fractal
                newValue = newValue * 0.6f + fractalValue * 0.4f * chaosLevel;
                
                // Add attractor influences
                for (const auto& attractor : attractors) {
                    float dx = fx - attractor.x;
                    float dy = fy - attractor.y;
                    float distance = sqrt(dx * dx + dy * dy) + 0.001f;
                    float influence = (1.0f / distance) * 0.1f * chaosLevel;
                    newValue += influence * sin(time * 3.0f + distance * 10.0f);
                }
                
                // Velocity field for fluid-like motion
                float velInfluence = sin(time * 2.0f + fx * 5.0f) * cos(time * 1.7f + fy * 4.0f);
                velocityX[y][x] = velocityX[y][x] * 0.95f + velInfluence * chaosLevel * 0.1f;
                velocityY[y][x] = velocityY[y][x] * 0.95f + cos(time * 1.3f + fx * 3.0f) * chaosLevel * 0.1f;
                
                // Apply velocity to position for fluid motion
                newValue += (velocityX[y][x] + velocityY[y][x]) * 0.2f;
                
                // Energy accumulation for explosive effects
                energyGrid[y][x] += abs(newValue - current) * 0.5f;
                if (energyGrid[y][x] > randomFloat(0.8f, 1.5f)) {
                    newValue += randomFloat(0.5f, 1.0f); // Energy explosion
                    energyGrid[y][x] = 0;
                    
                    // Create energy wave around explosion
                    for (int dy = -3; dy <= 3; dy++) {
                        for (int dx = -3; dx <= 3; dx++) {
                            if (y + dy >= 0 && y + dy < height && x + dx >= 0 && x + dx < width) {
                                float dist = sqrt(dx * dx + dy * dy);
                                if (dist > 0.1f) {
                                    energyGrid[y + dy][x + dx] += 0.3f / dist;
                                }
                            }
                        }
                    }
                }
                
                // Trail effects for motion blur
                trailGrid[y][x] = std::max(trailGrid[y][x] * 0.92f, newValue * 0.3f);
                
                // Clamp and add noise
                newValue = std::max(0.0f, std::min(2.0f, newValue));
                if (randomFloat(0, 1) < 0.02f * chaosLevel) {
                    newValue += randomFloat(-0.5f, 0.5f);
                }
                
                nextGrid[y][x] = newValue;
                
                // Generate psychedelic colors
                float intensity = newValue + trailGrid[y][x];
                float hue = fmod(intensity * 180.0f + colorShift + fx * 50.0f + fy * 30.0f + time * 100.0f, 360.0f);
                float saturation = 0.8f + sin(time * 3.0f + intensity * 5.0f) * 0.2f;
                float brightness = std::min(1.0f, intensity * (0.5f + sin(time * 4.0f) * 0.3f));
                
                // Add rainbow cycling and strobe effects
                if (isTripping) {
                    hue += sin(time * 10.0f + x * 0.2f) * 60.0f;
                    saturation = 1.0f;
                    brightness *= (0.7f + sin(time * 15.0f + y * 0.3f) * 0.3f);
                }
                
                colorGrid[y][x] = hsvToRgb(hue, saturation, brightness);
            }
        }
        
        // Swap grids
        grid.swap(nextGrid);
        
        // Randomly inject chaos patterns
        if (randomFloat(0, 1) < 0.1f * chaosLevel) {
            int cx = randomInt(10, width - 10);
            int cy = randomInt(10, height - 10);
            int patternType = randomInt(0, 5);
            
            switch (patternType) {
                case 0: injectSpinner(cx, cy); break;
                case 1: injectGlider(cx, cy); break;
                case 2: injectExploder(cx, cy); break;
                case 3: injectChaosBlob(cx, cy); break;
                case 4: injectEnergyVortex(cx, cy); break;
            }
        }
        
        // Occasionally completely randomize fractal parameters
        if (randomFloat(0, 1) < 0.03f) {
            fractalType = randomInt(0, 12);
            zoomLevel = randomFloat(0.01f, 10.0f);
            center = Vec3(randomFloat(-5, 5), randomFloat(-5, 5), randomFloat(-2, 2));
            warpIntensity = randomFloat(0.5f, 20.0f);
            
            // Add new attractors randomly
            if (randomFloat(0, 1) < 0.5f) {
                attractors.push_back(Vec3(randomFloat(-3, 3), randomFloat(-3, 3), randomFloat(-1, 1)));
                if (attractors.size() > 8) {
                    attractors.erase(attractors.begin());
                }
            }
        }
    }

    void render(PixelBuffer& pixelBuffer) {
        int width = pixelBuffer.getWidth();
        int height = pixelBuffer.getHeight();
        
        for (int y = 0; y < height && y < colorGrid.size(); y++) {
            for (int x = 0; x < width && x < colorGrid[y].size(); x++) {
                pixelBuffer.setPixel(x, y, colorGrid[y][x]);
            }
        }
    }
    
    void resize(int newWidth, int newHeight) {
        width = newWidth;
        height = newHeight;
        
        grid.resize(height, std::vector<float>(width, 0.0f));
        nextGrid.resize(height, std::vector<float>(width, 0.0f));
        energyGrid.resize(height, std::vector<float>(width, 0.0f));
        velocityX.resize(height, std::vector<float>(width, 0.0f));
        velocityY.resize(height, std::vector<float>(width, 0.0f));
        colorGrid.resize(height, std::vector<uint32_t>(width, 0xFF000000));
        trailGrid.resize(height, std::vector<float>(width, 0.0f));
        
        initialize();
    }
    
    std::string getCurrentModeName() const {
        switch (fractalType) {
            case 0: return "Hallucinogenic Game of Life";
            case 1: return "Psychedelic Mandelbrot";
            case 2: return "Trippy Julia Set";
            case 3: return "Warping Cellular Automata";
            case 4: return "Fractal Fluid Dynamics";
            case 5: return "Lorenz Attractor";
            case 6: return "Psychedelic Wave";
            case 7: return "Chaos Field";
            case 8: return "Quantum Fractal";
            default: return "Unknown";
        }
    }
};

// Implementation of the missing injection functions
void injectSpinner(int cx, int cy) {
    // Create a simple spinner pattern in the grid
    // This is a placeholder - you would need to access the FractalGameOfLifeSystem's grid
    // For now, just a stub implementation
}

void injectGlider(int cx, int cy) {
    // Create a glider pattern in the grid
    // This is a placeholder - you would need to access the FractalGameOfLifeSystem's grid
    // For now, just a stub implementation
}

void injectExploder(int cx, int cy) {
    // Create an exploder pattern in the grid
    // This is a placeholder - you would need to access the FractalGameOfLifeSystem's grid
    // For now, just a stub implementation
}

void injectChaosBlob(int cx, int cy) {
    // Create a chaos blob pattern in the grid
    // This is a placeholder - you would need to access the FractalGameOfLifeSystem's grid
    // For now, just a stub implementation
}

void injectEnergyVortex(int cx, int cy) {
    // Create an energy vortex pattern in the grid
    // This is a placeholder - you would need to access the FractalGameOfLifeSystem's grid
    // For now, just a stub implementation
}

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
    std::cout << "  M - Toggle between Weird Chaos and Fractal/Game of Life modes\n" << std::flush;

    bool running = true;
    bool needsRedraw = true;
    SDL_Event e;
    
    // Animation variables
    float rotation_time = 0.0f;
    uint32_t last_time = SDL_GetTicks();
    
    std::cout << "Creating drawScene function..." << std::flush;
    
    // Create weird visual manager
    WeirdVisualManager weirdVisualManager;

    // Create fractal/game of life system
    FractalGameOfLifeSystem fractalSystem(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Mode toggle
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
