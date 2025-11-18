#pragma once

#include <vector>
#include <string>
#include "utils.h"

// Forward declaration
class PixelBuffer;

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
    FractalGameOfLifeSystem(int w, int h);
    
    void initialize();
    void update(float deltaTime);
    void render(PixelBuffer& pixelBuffer);
    void resize(int newWidth, int newHeight);
    
    std::string getCurrentModeName() const;
    
    // Pattern injection methods
    void injectSpinner(int cx, int cy);
    void injectGlider(int cx, int cy);
    void injectExploder(int cx, int cy);
    void injectChaosBlob(int cx, int cy);
    void injectEnergyVortex(int cx, int cy);
    void setCell(int x, int y, float value);
    
private:
    void generateFractalLevel(std::vector<Triangle3D>& triangles, Vec3 center, float scale, int level, int maxLevel) const;
};

// Global fractal system instance for injection functions
extern FractalGameOfLifeSystem* g_fractalSystem;

// Global injection function declarations
void injectSpinner(int cx, int cy);
void injectGlider(int cx, int cy);
void injectExploder(int cx, int cy);
void injectChaosBlob(int cx, int cy);
void injectEnergyVortex(int cx, int cy);
void setCell(int x, int y, float value);