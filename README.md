# Ivy (2026-06-26)

**A game engine in C++26.**

Ivy is a hobby game engine project focused on learning the fundamentals of game engine development, computer graphics, mathematics, and physics.

> [!WARNING]
> Ivy is very WIP.

## Features

- **Architecture:** ECS-style.
- **Graphics:** OpenGL, shaders, textures, lighting, framebuffers, and MSAA
- **Math:** Vectors, matrices, complex, quaternions, functions, ODEs, sets & linear systems
- **Physics:** dimensional analysis
- **Reflection:** field modification, method invocation, creation. all without knowing the type at compile-time.

## Getting Started

### System Dependencies

#### Arch

```bash
sudo pacman -S base-devel clang clang-tools-extra libc++ libc++abi lld ccache cmake ninja git zip unzip tar pkgconf python doxygen gdb xmake
```

### Build and Run

```bash
xmake build Ivy
xmake run Ivy
```

### Run Tests

```bash
xmake run IvyTests
```

### Run Benchmarks

```bash
./scripts/bench.sh
```

### Generate Documentation

```bash
./scripts/docs.sh
```

## Development Log

[`Log.md`](docs/Log.md) for development history and weekly progress.
