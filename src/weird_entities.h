#pragma once

#include <vector>
#include <memory>
#include "utils.h"

// Forward declaration
class Triangle3D;

// Weird Visual Entity class
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
    
    WeirdEntity(Vec3 pos);
    
    void update(float deltaTime, int screenWidth, int screenHeight);
    bool isDead() const;
    std::vector<Triangle3D> generateTriangles() const;

private:
    void generateSpikyTriangles(std::vector<Triangle3D>& triangles, float lifeFactor) const;
    void generateBlobTriangles(std::vector<Triangle3D>& triangles, float lifeFactor) const;
    void generateFractalTriangles(std::vector<Triangle3D>& triangles, float lifeFactor) const;
    void generateFractalLevel(std::vector<Triangle3D>& triangles, Vec3 center, float scale, int level, int maxLevel) const;
    void generateRibbonTriangles(std::vector<Triangle3D>& triangles, float lifeFactor) const;
    void generateOrbTriangles(std::vector<Triangle3D>& triangles, float lifeFactor) const;
    void generateFragmentTriangles(std::vector<Triangle3D>& triangles, float lifeFactor) const;
    void generatePolyhedronTriangles(std::vector<Triangle3D>& triangles, float lifeFactor) const;
};

// Weird Visual Manager
class WeirdVisualManager {
private:
    std::vector<std::unique_ptr<WeirdEntity>> entities;
    float spawnTimer;
    float spawnInterval;
    int maxEntities;
    
public:
    WeirdVisualManager();
    
    void update(float deltaTime);
    std::vector<Triangle3D> getAllTriangles() const;
    size_t getEntityCount() const;
};