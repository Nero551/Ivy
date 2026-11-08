#pragma once
#include "Include.hpp"

/**
 * @namespace Ivy::ECS
 * @brief Archetype-style Entity-Component-System implementation for the Ivy engine.
 *
 * The ECS is built around four core concepts:
 * - **Entity** — a lightweight, movable handle with an attached hierarchy (parent/children).
 * - **Component** — plain structs deriving from Ivy::ECS::Component, stored contiguously in archetypes.
 * - **Archetype** — a cache-friendly storage bucket for one exact set of component types.
 * - **Query** — the façade for creating entities' components, fetching them, and iterating archetypes.
 *
 * All entities are owned by a single Ivy::ECS::EntityComponentSystem.
 */
namespace Ivy::ECS
{
}
