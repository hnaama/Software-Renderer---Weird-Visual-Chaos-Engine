# Software Renderer - Weird Visual Chaos Engine

A cross-platform software renderer implemented in C++ featuring advanced 3D graphics, triangle rasterization, and a unique **Weird Visual Chaos System** that generates bizarre, randomized visuals that are different every time you run it.

<!-- ![Software Renderer Demo](assets/demo.png) -->
*3D rotating triangles with dynamic lighting and weird visual entities*

## ✨ Features

### 🎨 Core Rendering System
- **🔺 Advanced Triangle Rasterization** - Dual algorithms with vertex color interpolation
- **🌐 Full 3D Graphics Pipeline** - Complete 3D transformation and projection system
- **💡 Dynamic Lighting** - Directional lighting with surface normal calculations
- **🎭 Perspective Projection** - True 3D depth perception with proper perspective division
- **🔄 3D Transformations** - Rotation, translation, scaling with 4x4 matrices

### 🌪️ Weird Visual Chaos System
- **🎲 Randomized Entity Generation** - 7 different types of bizarre visual entities
- **🔮 Unique Every Launch** - Hardware-seeded randomization ensures no two runs are identical
- **🌈 Dynamic Color Palettes** - Each entity gets randomly generated color schemes
- **⚡ Chaotic Physics** - Entities can randomly bounce, teleport, speed up, or change direction
- **🔄 Morphing Geometry** - Real-time shape deformation using mathematical functions
- **💫 Screen Effects** - Random streaks, dots, and rectangles for added chaos

### 🚀 Performance & Quality
- **⚡ Dual Rasterization Algorithms** - Barycentric and scanline methods
- **🎯 Backface Culling** - Optimized rendering of only visible triangles
- **🔄 Real-time Animation** - Smooth 60fps rendering with time-based animation
- **🌈 Beautiful Gradients** - Floating-point color mathematics for smooth blending

## 🛠️ Technical Highlights

### 3D Graphics Pipeline

1. **3D Vector Mathematics**
   - Complete Vec3 class with dot product, cross product, normalization
   - Robust vector operations for 3D calculations

2. **4x4 Transformation Matrices**
   - Rotation matrices for X, Y, Z axes
   - Translation and perspective projection matrices
   - Matrix multiplication for complex transformations

3. **Perspective Projection**
   - Field-of-view based projection
   - Proper depth perception and perspective division
   - Screen space conversion with aspect ratio handling

### Weird Visual Entity Types

1. **🌟 Spiky Stars** - Morphing star shapes with changing spike counts
2. **🫧 Morphing Blobs** - Organic shapes that pulse and deform using noise functions
3. **🌿 Fractal Spikes** - Recursive triangular patterns that branch fractally
4. **🎀 Twisted Ribbons** - Helical ribbon shapes twisting through 3D space
5. **⚪ Pulsing Orbs** - Spherical shapes constructed from triangular rings
6. **💥 Chaotic Fragments** - Exploding triangle pieces that scatter dynamically
7. **🔷 Weird Polyhedrons** - Complex 3D shapes with morphing vertices

### Animation System
- **Time-based Animation** - Frame-rate independent motion using delta time
- **Complex Transformation Chains** - Multiple rotations combined through matrix multiplication
- **Oscillating Motion** - Sine and cosine functions for organic movement patterns
- **Randomized Behaviors** - 1% chance per frame for chaos events

## 🚀 Quick Start

### Prerequisites

**macOS:**
```bash
brew install sdl2
```

**Ubuntu/Debian:**
```bash
sudo apt-get install libsdl2-dev
```

**Fedora/CentOS:**
```bash
sudo dnf install SDL2-devel
```

**Arch Linux:**
```bash
sudo pacman -S sdl2
```

**Windows:**
Download SDL2 development libraries from [libsdl.org](https://www.libsdl.org/download-2.0.php)

### Building

```bash
git clone https://github.com/hnaama/Software-Renderer.git
cd Software-Renderer
make
```

### Running

```bash
make run
```

Or directly:
```bash
./build/software_renderer
```

## 🎮 Controls

- **ESC** - Exit the application
- **Window Events** - Resizing or exposing triggers redraws
- **Continuous Animation** - No user input needed, runs automatically at 60fps

## 🎪 What You'll See

### Weird Visual Entities
The renderer spawns between 5-25 bizarre entities that:
- **Move randomly** across the screen with chaotic physics
- **Morph continuously** using mathematical functions
- **Change colors** with unique palettes per entity
- **Live and die** with random lifespans (5-15 seconds)
- **Teleport randomly** or wrap around screen edges

### Chaos Effects
- **10% chance per frame** for random streaks across screen
- **5% chance per frame** for random dots or rectangles
- **Randomly shifting background** colors
- **Unpredictable entity behaviors** (bouncing, speed changes, direction changes)

### 3D Transformations
- **Rotation around all axes** - X (pitch), Y (yaw), Z (roll)
- **Complex motion patterns** - Tumbling, oscillating, wobbling
- **Dynamic lighting** - Brightness changes based on surface orientation
- **Perspective depth** - Objects appear closer/farther with realistic perspective

## 📖 Demo Scenes

The renderer showcases various gradient triangle effects:

1. **RGB Triangle** - Red, Green, Blue vertices blending together
2. **Warm Colors** - Red, Yellow, Orange color scheme
3. **Cool Colors** - Blue, Cyan, Purple gradients
4. **Large Rainbow Triangle** - Demonstrates performance with big triangles
5. **Scanline vs Barycentric** - Algorithm comparison
6. **Grayscale Gradient** - White to black transitions
7. **Overlapping Triangles** - Layered rendering effects
8. **Pastel Colors** - Soft, subtle color blending

## 📁 Project Structure

```
Software-Renderer/
├── README.md              # This file
├── RASTERIZATION.md       # Detailed algorithm documentation (includes 3D section)
├── Makefile              # Cross-platform build system
├── Makefile.windows      # Windows-specific configuration
├── src/
│   └── main.cpp          # Complete 3D renderer with chaos system
├── assets/               # Demo images and resources
└── build/                # Generated build artifacts
```

## 🔬 Algorithm Deep Dive

For detailed technical documentation on both 2D rasterization and 3D transformation algorithms, see [RASTERIZATION.md](RASTERIZATION.md).

### 3D Rotation Example

```cpp
// Create complex transformation
Matrix4x4 transform = Matrix4x4::translation(-1.5f, 0.0f, 0.0f) *
                     Matrix4x4::rotationX(rotation_time * 1.2f) *
                     Matrix4x4::rotationY(rotation_time * 0.8f) *
                     Matrix4x4::rotationZ(rotation_time * 0.5f) *
                     projection;
```

### Weird Entity Generation

```cpp
// Each entity generates unique triangular geometry
std::vector<Triangle3D> generateTriangles() const {
    switch (type) {
        case 0: return generateSpikyTriangles();
        case 1: return generateBlobTriangles();
        case 2: return generateFractalTriangles();
        // ... 4 more weird types
    }
}
```

### Randomized Chaos

```cpp
// Hardware-seeded randomization
std::random_device rd;
std::mt19937 rng(rd());

// Random colors, positions, behaviors
uint32_t randomColor() {
    return 0xFF000000 | (randomInt(0, 255) << 16) | 
           (randomInt(0, 255) << 8) | randomInt(0, 255);
}
```

## 📊 Performance

| Feature | Implementation | Performance |
|---------|---------------|-------------|
| 3D Transformations | 4x4 matrix math | ~60fps with 20+ entities |
| Triangle Rasterization | Dual algorithms | Optimized for entity count |
| Lighting Calculations | Per-triangle normals | Real-time dynamic lighting |
| Random Generation | Hardware entropy | Unique every launch |
| Memory Management | Smart pointers | Automatic entity cleanup |

## 🎓 Educational Value

This project demonstrates advanced computer graphics concepts:

### 3D Graphics
- **Homogeneous Coordinates** - 4D math for 3D transformations
- **Matrix Mathematics** - Rotation, translation, projection matrices
- **Perspective Projection** - Converting 3D to 2D with depth
- **Surface Normals** - Cross products for lighting calculations
- **Backface Culling** - Performance optimization techniques

### Procedural Generation
- **Randomization Systems** - Hardware entropy and pseudo-random generators
- **Fractal Geometry** - Recursive triangle generation
- **Parametric Shapes** - Mathematical functions for organic forms
- **Chaos Theory** - Unpredictable but deterministic behaviors

### Software Engineering
- **Object-Oriented Design** - Entity systems and polymorphism
- **Memory Management** - RAII and smart pointers
- **Real-time Systems** - Frame-rate independent animation
- **Cross-Platform Development** - Portable C++ and build systems

## 🚀 Future Enhancements

Potential extensions to explore:

### Graphics Features
- **Texture Mapping** - UV coordinate interpolation for images
- **Shadow Mapping** - Dynamic shadows from light sources
- **Post-Processing** - Screen-space effects and filters
- **Particle Systems** - Point-based visual effects

### Chaos System
- **Sound Generation** - Procedural audio to match visuals
- **Interactive Chaos** - Mouse/keyboard influence on entities
- **Saved Configurations** - Ability to replay interesting patterns
- **Network Sync** - Shared chaos across multiple instances

### Performance
- **Multi-threading** - Parallel entity updates and rendering
- **SIMD Instructions** - Vectorized mathematics
- **GPU Compute** - OpenCL/CUDA acceleration for chaos calculations
- **Level-of-Detail** - Adaptive quality based on distance

## 🎨 Artistic Applications

The weird visual chaos system can be used for:

- **Generative Art** - Each run produces unique artistic output
- **VJ Performances** - Live visuals for music events
- **Meditation Apps** - Hypnotic, ever-changing patterns
- **Screensavers** - Beautiful, non-repetitive displays
- **Creative Coding** - Learning platform for procedural graphics

## 🤝 Contributing

Contributions are welcome! Areas for improvement:

1. **New Entity Types** - Create additional weird visual behaviors
2. **Chaos Algorithms** - Implement different randomization methods
3. **Performance Optimization** - SIMD, threading, GPU acceleration
4. **Interactive Features** - User control over chaos parameters
5. **Documentation** - Video tutorials, algorithm explanations

## 🎉 Try It Now!

Each time you run the program, you'll see:
- **Completely different** entity spawn patterns
- **Unique color combinations** never seen before
- **Unpredictable movement** behaviors
- **Organic morphing** patterns
- **Chaotic screen effects**

**No two launches are ever the same!** 🎲

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **SDL2** - Cross-platform multimedia library
- **Linear Algebra** - Mathematical foundation for 3D graphics
- **Chaos Theory** - Inspiration for unpredictable visual systems
- **Procedural Generation** - Techniques for infinite variety
- **Real-Time Graphics** - Performance optimization methods

---

**Built with ❤️, linear algebra, and a healthy dose of chaos** 🌪️✨