#include "utils.h"
#include <cstdint>

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

// HSV to RGB color conversion
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

uint32_t createRainbowColor(float t) {
    float r = (sin(t * 6.28f) * 0.5f + 0.5f);
    float g = (sin(t * 6.28f + 2.09f) * 0.5f + 0.5f);
    float b = (sin(t * 6.28f + 4.19f) * 0.5f + 0.5f);
    
    return 0xFF000000 | 
           ((int)(r * 255) << 16) | 
           ((int)(g * 255) << 8) | 
           (int)(b * 255);
}

uint32_t createNeonColor(float intensity, float hueShift) {
    float hue = fmod(intensity * 360.0f + hueShift, 360.0f);
    return hsvToRgb(hue, 1.0f, std::min(1.0f, intensity * 1.5f));
}

uint32_t blendColors(uint32_t color1, uint32_t color2, float t) {
    t = std::max(0.0f, std::min(1.0f, t));
    
    uint8_t r1 = (color1 >> 16) & 0xFF, g1 = (color1 >> 8) & 0xFF, b1 = color1 & 0xFF;
    uint8_t r2 = (color2 >> 16) & 0xFF, g2 = (color2 >> 8) & 0xFF, b2 = color2 & 0xFF;
    
    uint8_t r = (uint8_t)(r1 + t * (r2 - r1));
    uint8_t g = (uint8_t)(g1 + t * (g2 - g1));
    uint8_t b = (uint8_t)(b1 + t * (b2 - b1));
    
    return 0xFF000000 | (r << 16) | (g << 8) | b;
}

// Vec3 implementations
Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

Vec3 Vec3::operator+(const Vec3& other) const {
    return Vec3(x + other.x, y + other.y, z + other.z);
}

Vec3 Vec3::operator-(const Vec3& other) const {
    return Vec3(x - other.x, y - other.y, z - other.z);
}

Vec3 Vec3::operator*(float scalar) const {
    return Vec3(x * scalar, y * scalar, z * scalar);
}

float Vec3::dot(const Vec3& other) const {
    return x * other.x + y * other.y + z * other.z;
}

Vec3 Vec3::cross(const Vec3& other) const {
    return Vec3(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    );
}

float Vec3::length() const {
    return sqrt(x * x + y * y + z * z);
}

Vec3 Vec3::normalize() const {
    float len = length();
    if (len > 0.001f) {
        return Vec3(x / len, y / len, z / len);
    }
    return Vec3(0, 0, 0);
}

// Matrix4x4 implementations
Matrix4x4::Matrix4x4() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
}

Matrix4x4 Matrix4x4::rotationX(float angle) {
    Matrix4x4 mat;
    float c = cos(angle);
    float s = sin(angle);
    mat.m[1][1] = c; mat.m[1][2] = -s;
    mat.m[2][1] = s; mat.m[2][2] = c;
    return mat;
}

Matrix4x4 Matrix4x4::rotationY(float angle) {
    Matrix4x4 mat;
    float c = cos(angle);
    float s = sin(angle);
    mat.m[0][0] = c;  mat.m[0][2] = s;
    mat.m[2][0] = -s; mat.m[2][2] = c;
    return mat;
}

Matrix4x4 Matrix4x4::rotationZ(float angle) {
    Matrix4x4 mat;
    float c = cos(angle);
    float s = sin(angle);
    mat.m[0][0] = c; mat.m[0][1] = -s;
    mat.m[1][0] = s; mat.m[1][1] = c;
    return mat;
}

Matrix4x4 Matrix4x4::translation(float x, float y, float z) {
    Matrix4x4 mat;
    mat.m[0][3] = x;
    mat.m[1][3] = y;
    mat.m[2][3] = z;
    return mat;
}

Matrix4x4 Matrix4x4::perspective(float fov, float aspect, float near, float far) {
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

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const {
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

Vec3 Matrix4x4::transform(const Vec3& v) const {
    float w = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3];
    if (abs(w) < 0.001f) w = 1.0f;
    
    return Vec3(
        (m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3]) / w,
        (m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3]) / w,
        (m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3]) / w
    );
}

// Triangle3D implementations
Triangle3D::Triangle3D(Vec3 v0, Vec3 v1, Vec3 v2, uint32_t c0, uint32_t c1, uint32_t c2) {
    vertices[0] = v0; vertices[1] = v1; vertices[2] = v2;
    colors[0] = c0; colors[1] = c1; colors[2] = c2;
}

Vec3 Triangle3D::getNormal() const {
    Vec3 edge1 = vertices[1] - vertices[0];
    Vec3 edge2 = vertices[2] - vertices[0];
    return edge1.cross(edge2).normalize();
}

Triangle3D Triangle3D::transform(const Matrix4x4& matrix) const {
    return Triangle3D(
        matrix.transform(vertices[0]),
        matrix.transform(vertices[1]),
        matrix.transform(vertices[2]),
        colors[0], colors[1], colors[2]
    );
}