# Software Renderer

A cross-platform software renderer implemented in C++ featuring advanced triangle rasterization with vertex color interpolation. This project demonstrates fundamental computer graphics concepts including barycentric coordinates, scanline algorithms, and smooth color blending.

![Software Renderer Demo](assets/demo.png)
*Gradient triangles with vertex color interpolation*

## ✨ Features

- **🎨 Vertex Color Interpolation** - Each triangle vertex can have its own RGB color that blends smoothly across the surface
- **⚡ Dual Rasterization Algorithms**
  - **Barycentric Coordinate Method**: Mathematically precise, handles all edge cases
  - **Scanline Algorithm**: Performance optimized for large triangles
- **🌈 Beautiful Gradient Effects** - RGB, warm/cool colors, grayscale, and pastel color schemes
- **🔧 Cross-Platform Build System** - Automatic SDL2 detection on macOS, Linux, Windows, and FreeBSD
- **📚 Comprehensive Documentation** - Detailed explanation of rasterization algorithms
- **🚀 Real-time Rendering** - Interactive demo with smooth 60fps rendering

## 🛠️ Technical Highlights

### Rasterization Algorithms

1. **Barycentric Coordinate Rasterization**
   - Uses mathematical precision for pixel-perfect rendering
   - Natural support for vertex attribute interpolation
   - Handles all triangle orientations seamlessly

2. **Scanline Rasterization**
   - Optimized for performance with cache-friendly memory access
   - Efficient processing of large triangles
   - Linear interpolation along triangle edges

### Color System
- Floating-point color mathematics for smooth gradients
- ARGB format support with full alpha channel
- Linear interpolation preventing color banding
- Perspective-correct color blending

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
git clone https://github.com/yourusername/Software-Renderer.git
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

- **Any Key** - Redraw the scene
- **ESC** - Exit the application
- **Mouse/Window** - Interactive events trigger redraws

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

## 🏗️ Build System

The project uses a sophisticated cross-platform Makefile that automatically detects SDL2 installations:

```bash
make           # Release build
make debug     # Debug build with symbols
make clean     # Clean build artifacts
make info      # Show build configuration
make help      # Show all available commands
```

### Supported Platforms

- ✅ **macOS** (Homebrew, MacPorts, system installations)
- ✅ **Linux** (pkg-config, Ubuntu/Debian, Fedora, Arch)
- ✅ **Windows** (MinGW, MSYS2)
- ✅ **FreeBSD** (pkg-config, ports)

## 📁 Project Structure

```
Software-Renderer/
├── README.md              # This file
├── RASTERIZATION.md       # Detailed algorithm documentation
├── Makefile              # Cross-platform build system
├── Makefile.windows      # Windows-specific configuration
├── src/
│   └── main.cpp          # Main renderer implementation
├── assets/               # Demo images and resources
└── build/                # Generated build artifacts
```

## 🔬 Algorithm Deep Dive

For detailed technical documentation on the rasterization algorithms, see [RASTERIZATION.md](RASTERIZATION.md).

### Barycentric Coordinate Method

```cpp
// Calculate barycentric weights for point (x,y)
float w0 = sign(x, y, x1, y1, x2, y2) / area;
float w1 = sign(x0, y0, x, y, x2, y2) / area;
float w2 = sign(x0, y0, x1, y1, x, y) / area;

// Interpolate color using weights
Color finalColor = c0 * w0 + c1 * w1 + c2 * w2;
```

### Vertex Color Interpolation

```cpp
// Define triangle with different colored vertices
pixelBuffer.fillTriangleGradient(
    100, 100, 0xFFFF0000,  // Red vertex
    200, 100, 0xFF00FF00,  // Green vertex
    150, 200, 0xFF0000FF   // Blue vertex
);
```

## 📊 Performance

| Triangle Size | Barycentric | Scanline | Winner |
|---------------|-------------|----------|---------|
| Small (< 100px²) | Fast | Fast | Tie |
| Medium (100-1000px²) | Good | Faster | Scanline |
| Large (> 1000px²) | Slow | Much Faster | Scanline |

## 🎓 Educational Value

This project demonstrates key computer graphics concepts:

- **Rasterization Theory** - Converting geometry to pixels
- **Barycentric Coordinates** - Mathematical foundations of interpolation
- **Linear Interpolation** - Smooth transitions between values
- **Color Mathematics** - RGB blending and color spaces
- **Performance Optimization** - Algorithm selection and cache efficiency
- **Cross-Platform Development** - Build system design

## 🚀 Future Enhancements

Potential extensions to explore:

- **Texture Mapping** - UV coordinate interpolation
- **Z-Buffer** - Depth testing for 3D rendering
- **Anti-Aliasing** - Multi-sampling for smoother edges
- **Lighting Models** - Phong/Blinn-Phong shading
- **3D Transformations** - Model-view-projection matrices
- **Clipping** - Viewport and frustum clipping
- **Animation** - Time-based effects and interpolation

## 🤝 Contributing

Contributions are welcome! Areas for improvement:

1. **Performance Optimizations** - SIMD instructions, threading
2. **Additional Algorithms** - Half-space rasterization, tile-based rendering
3. **Advanced Features** - Texture mapping, lighting, shadows
4. **Platform Support** - Additional OS/compiler combinations
5. **Documentation** - More examples, tutorials, benchmarks

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **SDL2** - Cross-platform multimedia library
- **Computer Graphics: Principles and Practice** - Theoretical foundation
- **Real-Time Rendering** - Advanced techniques and optimizations
- **scratchapixel.com** - Excellent graphics programming tutorials

## 📞 Contact

- **GitHub**: [@yourusername](https://github.com/yourusername)
- **Email**: your.email@example.com

---

**Built with ❤️ and lots of linear algebra**