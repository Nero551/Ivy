@mainpage Ivy

# Framework (Archetype ECS)

## Architecture

The engine is organized into a simple hierarchy:

```
Main
└── Engine
    ├── Systems
    ├── ECS
    │   └── Entities
    │   └── Components
    └── World
        └── Systems

```

## Main

The application's entry point.
Its only responsibility is to create and run the **Engine**.

---

## Engine

The central object of the framework.
It owns and updates:

- The **Window**
- The **World**
- The **ECS**
- All **Systems**

The engine is responsible for driving the application's lifecycle (startup, update loop, rendering, shutdown).

---

---

## Systems

Systems contain behavior and logic.
They perform work every frame, either independently or by operating on entities and their components.
Examples include:

- Camera System
- Transform System

---

## World

stores scene state and entity references. The Engine owns and runs systems such as the transform system.

---

## ECS ( Component System)

Owns the entities and the query (which owns archetypes and those own components).
It is responsible for creating, storing & removing entities.

---

## Entities

Entities are lightweight objects.
An entity is simply:

- A unique ID

Entities contain no game logic.

---

## Components

Components are pure data blocks.
They describe an entity's properties without containing behavior.

Examples include:

- TransformComponent
- MeshComponent
- MaterialComponent
- CameraComponent
- LightComponent

Systems read and modify component data to produce behavior.
This separation keeps data independent of behavior while making systems reusable and easy to extend.

### Coordinate Conventions

Engine uses a right-handed Cartesian coordinate system:

- **+X** points right.
- **+Y** points up.
- **-Z** points forward.
