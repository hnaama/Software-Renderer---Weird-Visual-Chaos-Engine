# Software Renderer - Because Modern GPUs Are For Quitters

Oh look, another genius decided to reinvent the wheel by building a software renderer in 2025. Because apparently, the thousands of GPU engineers at NVIDIA and AMD just didn't get the memo that *real* programmers render triangles with their bare CPUs. Welcome to my delightfully masochistic 3D graphics engine that proudly ignores 30 years of hardware acceleration progress.

<!-- ![Software Renderer Demo](assets/demo.png) -->
*Look at these beautiful triangles! Your GPU is crying right now.*

## 🛠️ Technical "Achievements" (AKA Reinventing Linear Algebra)

### 3D Graphics Pipeline (The Hard Way™)

1. **3D Vector Mathematics (Because std::vector Wasn't Confusing Enough)**
   - Hand-rolled Vec3 class because who needs GLM, right?
   - Dot products, cross products, and normalization - just like your high school math teacher warned you about
   - Now with 100% more manual memory management!

2. **4x4 Transformation Matrices (The Freshman CS Student Special)**
   - Rotation matrices that would make Euler weep
   - Matrix multiplication written from scratch because libraries are for weaklings
   - Perspective projection that occasionally works as intended

3. **Perspective Projection (AKA "Making 3D Look 2D The Hard Way")**
   - Field-of-view calculations that definitely didn't come from Wikipedia
   - Depth perception for people who miss the good old days of software rendering
   - Screen space conversion that's totally not copied from a graphics textbook

### Animation System (Chaos, But Make It "Artistic")
- **Time-based Animation** - Because frame drops are a feature, not a bug
- **Complex Transformation Chains** - Matrix multiplication so nested it needs therapy
- **Oscillating Motion** - Sine waves everywhere because trigonometry is *apparently* fun
- **Randomized Behaviors** - 1% chaos chance because predictability is overrated

## 🚀 Quick Start (Good Luck With That)

### Prerequisites (Time to Break Your Package Manager)

**macOS (For the Hipsters):**
```bash
brew install sdl2  # The only library we'll admit to using
```

**Ubuntu/Debian (For the Practical People):**
```bash
sudo apt-get install libsdl2-dev  # At least it's free
```

**Fedora/CentOS (For the Rebels):**
```bash
sudo dnf install SDL2-devel  # Red Hat, more like Red Flag
```

**Arch Linux (BTW I Use Arch):**
```bash
sudo pacman -S sdl2  # Because of course you use Arch
```

**Windows (May God Have Mercy On Your Soul):**
Download SDL2 from [libsdl.org](https://www.libsdl.org/download-2.0.php) and prepare for DLL hell

### Building (Cross Your Fingers)

```bash
git clone https://github.com/hnaama/Software-Renderer---Weird-Visual-Chaos-Engine.git
cd Software-Renderer
make  # Pray to the compiler gods
```

### Running (The Moment of Truth)

```bash
make run  # If it doesn't segfault, you win!
```

Or if you're feeling brave:
```bash
./build/software_renderer  # YOLO
```

## 🎮 Controls (What Little Control You Have)

- **ESC** - Escape from this beautiful disaster
- **Window Events** - Resize the window and watch everything break gracefully
- **Continuous Animation** - Sit back and watch your CPU cry at 60fps

## 🎪 What You'll Witness (Prepare Your Eyes)

### "Weird Visual Entities" (AKA Mathematical Nightmares)
The renderer spawns 5-25 digital abominations that:
- **Move randomly** like they're having existential crises
- **Morph continuously** using functions that would make Lovecraft proud
- **Change colors** because consistency is for boring people
- **Live and die** with lifespans shorter than a fruit fly's attention span
- **Teleport randomly** because physics is just a suggestion

### Chaos Effects (Embrace The Madness)
- **10% chance per frame** for random streaks (because why not?)
- **5% chance per frame** for spontaneous geometric shapes
- **Randomly shifting backgrounds** that change faster than your mood
- **Unpredictable behaviors** that would make quantum physicists jealous

### 3D Transformations (Linear Algebra Revenge)
- **Rotation around all axes** - X, Y, Z, and your sanity
- **Complex motion patterns** that defy the laws of good taste
- **Dynamic lighting** calculated pixel by pixel like it's 1995
- **Perspective depth** that occasionally works correctly

### 3D Rotation Example (Behold, The Matrix Math)

```cpp
// Transformation matrix multiplication - because simple is overrated
Matrix4x4 transform = Matrix4x4::translation(-1.5f, 0.0f, 0.0f) *
                     Matrix4x4::rotationX(rotation_time * 1.2f) *
                     Matrix4x4::rotationY(rotation_time * 0.8f) *
                     Matrix4x4::rotationZ(rotation_time * 0.5f) *
                     projection;
```

### Weird Entity Generation (Procedural Nightmare Factory)

```cpp
// Seven different ways to generate triangular chaos
std::vector<Triangle3D> generateTriangles() const {
    switch (type) {
        case 0: return generateSpikyTriangles();    // For the edgelords
        case 1: return generateBlobTriangles();     // Organic and unsettling
        case 2: return generateFractalTriangles();  // Recursion until stack overflow
        // ... 4 more varieties of geometric horror
    }
}
```

### Randomized Chaos (Because Determinism Is Dead)

```cpp
// True randomness from your hardware's quantum fluctuations
std::random_device rd;  // The universe's RNG
std::mt19937 rng(rd()); // Mersenne Twister because we're fancy

// Random colors that definitely don't hurt your eyes
uint32_t randomColor() {
    return 0xFF000000 | (randomInt(0, 255) << 16) | 
           (randomInt(0, 255) << 8) | randomInt(0, 255);
}
```

## 🎓 "Educational" Value (What You'll Learn)

This project teaches you why graphics cards exist:

### 3D Graphics (The Manual Labor Edition)
- **Homogeneous Coordinates** - 4D math for 3D problems because why not overcomplicate things
- **Matrix Mathematics** - Linear algebra your professor said you'd never use
- **Perspective Projection** - Making flat screens pretend to be 3D since forever
- **Surface Normals** - Cross products that cross your eyes
- **Backface Culling** - Optimization techniques from the stone age

### Procedural Generation (Algorithmic Art Therapy)
- **Randomization Systems** - Entropy from actual hardware randomness
- **Fractal Geometry** - Infinite complexity from simple rules
- **Parametric Shapes** - Math functions pretending to be art
- **Chaos Theory** - Predictable unpredictability (it's a thing)

### Software Engineering (Character Building Exercises)
- **Object-Oriented Design** - Classes and inheritance, oh my!
- **Memory Management** - Manual memory allocation like it's 1985
- **Real-time Systems** - 60fps or your money back*
- **Cross-Platform Development** - Write once, debug everywhere

*Money back guarantee void in all territories

## 🚀 Future "Enhancements" (Ways to Make This Worse)

Because apparently we haven't suffered enough:

### Graphics Features (More Ways to Torture Your CPU)
- **Texture Mapping** - Because flat colors are too mainstream
- **Shadow Mapping** - Real-time shadows calculated one pixel at a time
- **Post-Processing** - Instagram filters for your triangles
- **Particle Systems** - Ten thousand individual points because why not

### Chaos System (Maximum Entropy Mode)
- **Sound Generation** - Procedural audio to assault your ears too
- **Interactive Chaos** - Let users make it worse
- **Saved Configurations** - Preserve the exact moment everything broke
- **Network Sync** - Share the chaos across multiple victims

### Performance (Oxymoron Alert)
- **Multi-threading** - Spread the suffering across all CPU cores
- **SIMD Instructions** - Vectorized inefficiency
- **GPU Compute** - Admit defeat and use the graphics card anyway
- **Level-of-Detail** - Adaptive quality for adaptive disappointment

## 🎨 "Artistic" Applications (Stretching the Definition)

This digital fever dream can be used for:

- **Generative Art** - Each crash produces unique error patterns
- **VJ Performances** - Epilepsy warnings recommended
- **Meditation Apps** - Stare into the void, void stares back
- **Screensavers** - For when you want your screen to scream
- **Creative Coding** - Teaching people why GPU acceleration exists

## 🤝 Contributing (Misery Loves Company)

Want to make this worse? You're in luck!

1. **New Entity Types** - More ways to confuse triangles
2. **Chaos Algorithms** - Advanced randomization techniques
3. **Performance "Optimization"** - Making it slower more efficiently
4. **Interactive Features** - Let users break things manually
5. **Documentation** - Explaining why this exists (good luck)

## 🎉 Try It Now! (You've Been Warned)

Every execution promises:
- **Completely different** patterns of digital chaos
- **Unique color combinations** that defy human perception
- **Unpredictable movement** like a broken physics simulation
- **Organic morphing** that would make Cronenberg proud
- **Chaotic screen effects** that test your monitor's refresh rate

**No two launches are identical!** (Neither are the crash logs)

## 📄 License

MIT License - because even questionable life choices deserve legal protection.

## 🙏 Acknowledgments (Blame Assignment Sheet)

- **SDL2** - The only library we trust (barely)
- **Linear Algebra** - Thanks for the nightmares, high school math
- **Chaos Theory** - Edward Lorenz, this is your fault
- **Procedural Generation** - Ken Perlin has a lot to answer for
- **Real-Time Graphics** - John Carmack probably wouldn't approve

---

**Built with ❤️, questionable life choices, and a fundamental misunderstanding of modern graphics APIs** 🌪️💀

*P.S. - Your graphics card is laughing at you right now.*
