#include "fractals.h"
#include <cmath>

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