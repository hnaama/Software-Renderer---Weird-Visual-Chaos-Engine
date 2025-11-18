#include "fractal_system.h"
#include "fractals.h"
#include "pixelbuffer.h"
#include <cmath>
#include <algorithm>

// Global fractal system instance for injection functions
FractalGameOfLifeSystem* g_fractalSystem = nullptr;

FractalGameOfLifeSystem::FractalGameOfLifeSystem(int w, int h) : width(w), height(h), time(0), fractalType(0), 
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

void FractalGameOfLifeSystem::initialize() {
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

void FractalGameOfLifeSystem::update(float deltaTime) {
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

void FractalGameOfLifeSystem::render(PixelBuffer& pixelBuffer) {
    int bufferWidth = pixelBuffer.getWidth();
    int bufferHeight = pixelBuffer.getHeight();
    
    for (int y = 0; y < bufferHeight && y < colorGrid.size(); y++) {
        for (int x = 0; x < bufferWidth && x < colorGrid[y].size(); x++) {
            pixelBuffer.setPixel(x, y, colorGrid[y][x]);
        }
    }
}

void FractalGameOfLifeSystem::resize(int newWidth, int newHeight) {
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

std::string FractalGameOfLifeSystem::getCurrentModeName() const {
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

void FractalGameOfLifeSystem::injectSpinner(int cx, int cy) {
    if (cx >= 1 && cx < width-1 && cy >= 1 && cy < height-1) {
        setCell(cx, cy, 1.0f);
        setCell(cx-1, cy, 1.0f);
        setCell(cx+1, cy, 1.0f);
        setCell(cx, cy-1, 1.0f);
        setCell(cx, cy+1, 1.0f);
    }
}

void FractalGameOfLifeSystem::injectGlider(int cx, int cy) {
    if (cx >= 2 && cx < width-2 && cy >= 2 && cy < height-2) {
        setCell(cx, cy, 1.0f);
        setCell(cx+1, cy+1, 1.0f);
        setCell(cx-1, cy+2, 1.0f);
        setCell(cx, cy+2, 1.0f);
        setCell(cx+1, cy+2, 1.0f);
    }
}

void FractalGameOfLifeSystem::injectExploder(int cx, int cy) {
    if (cx >= 2 && cx < width-2 && cy >= 2 && cy < height-2) {
        setCell(cx-1, cy-1, 1.0f);
        setCell(cx-1, cy, 1.0f);
        setCell(cx-1, cy+1, 1.0f);
        setCell(cx+1, cy-1, 1.0f);
        setCell(cx+1, cy+1, 1.0f);
    }
}

void FractalGameOfLifeSystem::injectChaosBlob(int cx, int cy) {
    if (cx >= 3 && cx < width-3 && cy >= 3 && cy < height-3) {
        for (int dy = -3; dy <= 3; dy++) {
            for (int dx = -3; dx <= 3; dx++) {
                if (dx*dx + dy*dy <= 9) {
                    float prob = 0.6f - (dx*dx + dy*dy) * 0.05f;
                    if (randomFloat(0, 1) < prob) {
                        setCell(cx + dx, cy + dy, 1.0f);
                    }
                }
            }
        }
    }
}

void FractalGameOfLifeSystem::injectEnergyVortex(int cx, int cy) {
    if (cx >= 4 && cx < width-4 && cy >= 4 && cy < height-4) {
        for (int r = 1; r <= 4; r++) {
            for (int angle = 0; angle < 360; angle += 45) {
                float rad = angle * M_PI / 180.0f;
                int x = cx + static_cast<int>(r * cos(rad + r * 0.5f));
                int y = cy + static_cast<int>(r * sin(rad + r * 0.5f));
                if (x >= 0 && x < width && y >= 0 && y < height) {
                    float energy = 1.0f - (r - 1) * 0.2f;
                    setCell(x, y, energy);
                }
            }
        }
        setCell(cx, cy, 1.0f);
    }
}

void FractalGameOfLifeSystem::setCell(int x, int y, float value) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        grid[y][x] = value;
    }
}

// Global injection function implementations
void injectSpinner(int cx, int cy) {
    if (g_fractalSystem) {
        g_fractalSystem->injectSpinner(cx, cy);
    }
}

void injectGlider(int cx, int cy) {
    if (g_fractalSystem) {
        g_fractalSystem->injectGlider(cx, cy);
    }
}

void injectExploder(int cx, int cy) {
    if (g_fractalSystem) {
        g_fractalSystem->injectExploder(cx, cy);
    }
}

void injectChaosBlob(int cx, int cy) {
    if (g_fractalSystem) {
        g_fractalSystem->injectChaosBlob(cx, cy);
    }
}

void injectEnergyVortex(int cx, int cy) {
    if (g_fractalSystem) {
        g_fractalSystem->injectEnergyVortex(cx, cy);
    }
}

void setCell(int x, int y, float value) {
    if (g_fractalSystem) {
        g_fractalSystem->setCell(x, y, value);
    }
}