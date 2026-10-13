@mainpage Ivy

# Framework (ECS)

## Architecture

The engine is organized into a simple hierarchy:

```
Main
└── Engine
    ├── Modules
    │   └── Systems
    └── World
        ├── Systems
        └── Entities
        └── Components
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
- All **Modules**

The engine is responsible for driving the application's lifecycle (startup, update loop, rendering, shutdown).

---

## Modules

Modules are large, self-contained parts of the engine that participate in the runtime loop.
Examples include:

- Renderer
- Input
- Physics simulation

Each module owns one or more **Systems**. The module handles the lifecycle of all related **Systems**.
It can also contain premade **Entities** & **Components**.

---

## Systems

Systems contain behavior and logic.
They perform work every frame, either independently or by operating on entities and their components.
Examples include:

- Camera System
- Transform System

---

## World

The world owns the entities and the component query (which owns component pools).
It is responsible for creating, storing & removing entities.

---

## Entities

Entities are lightweight objects.
An entity is simply:

- A unique ID
- Ids of children and parent

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
