# Ivy (2026-06-26)

**A 3D game engine in C++26.**

Ivy is a hobby game engine project focused on learning the fundamentals of game engine development, computer graphics, mathematics, and physics.

> [!WARNING]
> Ivy is very WIP.

## Features

- **Architecture:** ECS-style ([`Framework.md`](Docs/Framework.md) ).
- **Rendering:** OpenGL, shaders, textures, lighting, framebuffers, and MSAA
- **Mathematics:** Vectors, matrices, complex, quaternions, functions, sets, linear systems
- **Physics Library:** dimensional analysis

## Getting Started

### Prerequisites

- Linux
- [xmake](https://xmake.io/)

### Install Dependencies

```bash
./scripts/setup-linux.sh
```

### Build and Run

```bash
xmake Ivy
xmake run Ivy
```

### Run Tests

```bash
xmake IvyTests
xmake run IvyTests
```

### Generate Documentation

```bash
./scripts/docs.sh
```

## Development Log

[`Log.md`](Docs/Log.md) for Ivy's development history and weekly progress.
