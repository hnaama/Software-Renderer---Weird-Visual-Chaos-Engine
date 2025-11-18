#include "weird_entities.h"
#include <cmath>
#include <algorithm>
#include <tuple>

WeirdEntity::WeirdEntity(Vec3 pos) : position(pos), morphTime(0.0f) {
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

void WeirdEntity::update(float deltaTime, int screenWidth, int screenHeight) {
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

bool WeirdEntity::isDead() const {
    return life <= 0;
}

std::vector<Triangle3D> WeirdEntity::generateTriangles() const {
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

void WeirdEntity::generateSpikyTriangles(std::vector<Triangle3D>& triangles, float lifeFactor) const {
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

void WeirdEntity::generateBlobTriangles(std::vector<Triangle3D>& triangles, float lifeFactor) const {
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

void WeirdEntity::generateFractalTriangles(std::vector<Triangle3D>& triangles, float lifeFactor) const {
    generateFractalLevel(triangles, position, size.x, 0, 3);
}

void WeirdEntity::generateFractalLevel(std::vector<Triangle3D>& triangles, Vec3 center, float scale, int level, int maxLevel) const {
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

void WeirdEntity::generateRibbonTriangles(std::vector<Triangle3D>& triangles, float lifeFactor) const {
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

void WeirdEntity::generateOrbTriangles(std::vector<Triangle3D>& triangles, float lifeFactor) const {
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

void WeirdEntity::generateFragmentTriangles(std::vector<Triangle3D>& triangles, float lifeFactor) const {
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

void WeirdEntity::generatePolyhedronTriangles(std::vector<Triangle3D>& triangles, float lifeFactor) const {
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

// WeirdVisualManager implementations
WeirdVisualManager::WeirdVisualManager() : spawnTimer(0), spawnInterval(randomFloat(0.5f, 2.0f)), maxEntities(randomInt(8, 20)) {}

void WeirdVisualManager::update(float deltaTime) {
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

std::vector<Triangle3D> WeirdVisualManager::getAllTriangles() const {
    std::vector<Triangle3D> allTriangles;
    
    for (const auto& entity : entities) {
        auto entityTriangles = entity->generateTriangles();
        allTriangles.insert(allTriangles.end(), entityTriangles.begin(), entityTriangles.end());
    }
    
    return allTriangles;
}

size_t WeirdVisualManager::getEntityCount() const {
    return entities.size();
}