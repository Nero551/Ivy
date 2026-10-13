# Ivy (2026-06-26)

**A game engine in C++26.**

Ivy is a hobby game engine project focused on learning the fundamentals of game engine development, computer graphics, mathematics, and physics.

> [!WARNING]
> Ivy is very WIP.

## Features

- **Architecture:** ECS-style ([`Framework.md`](Docs/Framework.md) ).
- **Graphics:** OpenGL, shaders, textures, lighting, framebuffers, and MSAA
- **Math:** Vectors, matrices, complex, quaternions, functions, sets, linear systems
- **Physics:** dimensional analysis

## Getting Started

### Prerequisites

- Linux
- [xmake](https://xmake.io/)

### Install Dependencies

```bash
sudo ./scripts/setup-linux.sh
```

### Build and Run

```bash
xmake build Ivy
xmake run Ivy
```

### Run Tests

```bash
xmake build IvyTests
xmake run IvyTests
```

### Generate Documentation

```bash
./scripts/docs.sh
```

## Development Log

[`Log.md`](Docs/Log.md) for development history and weekly progress.
