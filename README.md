# Nova (2026-06-26) 3D game engine hobby project

## Installing System Dependencies for Linux.

- ./scripts.setup-linux.sh

## Building & Running Engine

- ./scripts/build.sh
- ./scripts/run.sh

## Building & Running Tests

- ./scripts/build-tests.sh
- ./scripts/run-tests.sh

## Loading Docs

- ./scripts/docs.sh

## Progress

### Week 1 — June 26–July 2

- Vectors
- Shaders
- First Triangle
- EBO , VBO & VAO
- Color Vertex data

![Colored Quad](Docs/Screenshots/Colored%20Quad.png)

### Week 2 — July 3–9

- Shader class
- Textures & texture class
- UV / Texture Coordinates
- Mipmaps
- Texture Filtering
- Transformations
- Matrices
- Coordinate systems

![Textured Quad](Docs/Screenshots/Textured%20Quad.png)

### Week 3 — July 10–16

- Complete Architecture redesign
- ECS-style framework
- Camera
- LookAt Matrix
- Arbitrary Axis Rotation
- Input

![3D Cube](Docs/Screenshots/3D%20Cube.png)

### Week 4 — July 17–23

- Ambient, Diffuse & Specular lighting
- Normals & Normal Matrix
- Light entity
- Material properties
- Emission
- Diffuse , Specular & Emission maps
- Unit Tests
- Directional Lights , Point Lights & Spot Lights
- Entity hierarchy (Entities can have child entities)
- Multiple Lights
- Event Bus

![Cube](Docs/Screenshots/MaybeACube.png)

### Week 5 — July 24–30

- Assimp model loading
- Shader Include preprocessing
- Parent Child transform relations

![Backpack](Docs/Screenshots/Backpack.png)

### Week 6 — July 31–August 6

- Basic calc 1 derivatives

### Week 7 — August 7–13

- Added Doxygen for docs
- basic limits

### Week 8 — August 14–20

- vcpkg package management
- basic calc 1 integration

### Week 9 — August 21–28

- complex arithmetic
- Quaternion<> arithmetic & rotation conversions
- depth, stencil & blending added as per material configurations
- per mesh face culling configurations
- redesigned texture system
- Framebuffers & Renderbuffers
- Cubemaps
- Uniform buffers

![Post-Processed Cube](Docs/Screenshots/Post-ProccesedCube.png)

### Week 10 — August 29–September 4

- Geometry shaders
- Instancing
- MSAA (Multisample anti-aliasing)
- DSA (Direct State Access)

![4D Projection (10k entities)](Docs/Screenshots/4D%20Projection.png)

### Week 11 — September 5–11

- Optimizations (CPU can now handle ~32k entities 40fps)

### Week 12 — September 12–18

- Compile-time dimensional analysis 