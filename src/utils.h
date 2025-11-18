#pragma once

#include <cmath>
#include <random>
#include <algorithm>

// Random number generator setup
extern std::random_device rd;
extern std::mt19937 rng;

// Helper functions
float randomFloat(float min, float max);
int randomInt(int min, int max);
uint32_t randomColor();

// Color utility functions
uint32_t hsvToRgb(float h, float s, float v);
uint32_t createRainbowColor(float t);
uint32_t createNeonColor(float intensity, float hueShift = 0);
uint32_t blendColors(uint32_t color1, uint32_t color2, float t);

// 3D Vector class
struct Vec3 {
    float x, y, z;
    
    Vec3(float x = 0, float y = 0, float z = 0);
    
    Vec3 operator+(const Vec3& other) const;
    Vec3 operator-(const Vec3& other) const;
    Vec3 operator*(float scalar) const;
    
    float dot(const Vec3& other) const;
    Vec3 cross(const Vec3& other) const;
    float length() const;
    Vec3 normalize() const;
};

// 4x4 Matrix class
struct Matrix4x4 {
    float m[4][4];
    
    Matrix4x4();
    
    static Matrix4x4 rotationX(float angle);
    static Matrix4x4 rotationY(float angle);
    static Matrix4x4 rotationZ(float angle);
    static Matrix4x4 translation(float x, float y, float z);
    static Matrix4x4 perspective(float fov, float aspect, float near, float far);
    
    Matrix4x4 operator*(const Matrix4x4& other) const;
    Vec3 transform(const Vec3& v) const;
};

// 3D Triangle structure
struct Triangle3D {
    Vec3 vertices[3];
    uint32_t colors[3];
    
    Triangle3D(Vec3 v0, Vec3 v1, Vec3 v2, uint32_t c0, uint32_t c1, uint32_t c2);
    
    Vec3 getNormal() const;
    Triangle3D transform(const Matrix4x4& matrix) const;
};