# Raycaster

A 2.5D raycasting engine written in C++ and GLSL, rebuilt and extended from the [original EPQ version](https://ruairi.uk/projects/raycasting-engine.html). The core rendering pipeline has been moved entirely onto the GPU via an OpenGL fragment shader, with textures, mouse look, billboarded sprites, and enemy AI added on top.

---

## Demo

> WASD to move · Mouse to look

---

## What's new vs. the original

The original engine ran a DDA loop once per column on the CPU and drew solid-coloured walls. This version has:

- **GPU-side DDA** — the entire ray loop runs per-pixel in a GLSL fragment shader (`raycaster_shader.glsl`), eliminating the column-by-column CPU bottleneck and unlocking per-pixel texture sampling
- **Wall textures** — six distinct wall materials (brick, dirt, dark oak log, spruce planks, stone, and a fallback), each sampled from its own `sampler2D` uniform and selected by wall type in the shader
- **Textured floors** — floor pixels are ray-marched back to world space each frame and sampled from a tiling grass texture, with distance-based darkening applied
- **Sky gradient** — a logarithmic blue gradient fills ceiling pixels above the horizon line
- **Mouse look** — horizontal mouse delta rotates yaw (direction + camera plane together), vertical delta adjusts pitch, which shifts the horizon line to give the impression of looking up and down
- **Billboarded sprites** — sprites are projected into camera space using the camera plane determinant, depth-sorted back-to-front on the CPU each frame, and drawn per-pixel in the shader against the wall depth. Textures come from a sprite atlas
- **Zombie enemies** — each frame, sprite positions step toward the player using the normalised displacement vector, giving something to actually run away from
- **Bayer dithering** — a 4×4 Bayer matrix is applied as a final pass to reduce colour banding at low light levels
- **Much bigger map** — a hand-authored 24×24 grid with varied wall types across interconnected rooms

---

## Architecture

```
main.cpp / main.h         Game loop, uniform uploads, sprite management
Player.cpp / Player.h     Movement, collision, yaw/pitch rotation
Vector2D.h                2D vector class with operator overloading and rotation matrix
Map.h                     Static 24×24 map and bounds-checked lookup
raycaster_shader.glsl     Fragment shader — DDA, texture sampling, sprite rendering, sky/floor
Assets/                   PNG textures and sprite atlas
CMakeLists.txt            Build config (pkg-config on Linux/macOS, hardcoded fallback on Windows)
```

### Rendering pipeline (per fragment)

1. A ray is constructed from the fragment's X position relative to screen centre and the camera plane
2. DDA marches the ray through the map grid until it hits a wall or reaches `VIEW_DIST = 48`
3. Perpendicular distance determines wall height; texture coordinates are derived from the exact hit point on the wall face
4. Pixels above the wall top receive the sky gradient; pixels below receive a floor colour sampled from world space
5. Sprites are iterated (already sorted on the CPU) and projected; pixels inside a sprite's screen bounds sample the atlas and overwrite the background if closer than the wall hit
6. Distance-based darkening (`DARKEN_MAX = 0.8`, falloff over `DARKEN_DISTANCE = 6.0`) and Y-side darkening are applied throughout
7. Bayer dithering is added as a final pass

---

## Tech Stack

| Area | Detail |
|---|---|
| Language | C++17 |
| Graphics / windowing | raylib |
| Shader | GLSL 3.30 fragment shader |
| Algorithm | DDA (Digital Differential Analysis) |
| Build | CMake + pkg-config |
| Vector maths | Custom `Vector2D` class with operator overloading |

---

## Building

### Prerequisites

- CMake 3.10+
- A C++17 compiler
- [raylib-devel](https://www.raylib.com/) installed

### Linux / macOS

```bash
git clone https://github.com/ruairia/raycaster
cd raycaster
mkdir build && cd build
cmake ..
cmake --build .
./raycaster
```

### Windows

The CMakeLists falls back to `C:/raylib` if pkg-config is not found. Adjust that path to match your raylib installation before building.

> **Note:** the shader file is copied into the build directory automatically by CMake, so it will always be found at runtime relative to the executable.

---

## Controls

| Input   | Action                        | 
|---------|-------------------------------|
| W / S   | Move forward / backward       |
| A / D   | Strafe left / right           |
| Mouse X | Look left / right / up / down |
| Esc     | Quit                          |