#pragma once

#include "Core/ECS/EntityID.hpp"
#include "Utilities/CheckedPtr.hpp"
#include "Utilities/DataStructures/GIndexPool.hpp"
#include "Utilities/DataStructures/SparseSetSoA.hpp"
#include <concepts>
#include <utility>
#include <vector>

namespace Ivy::C
{
struct EntityComponentSystem;

/** @brief Represents an entity and its parent-child hierarchy. */
struct Entity
{
    virtual ~Entity() = default;

    Entity(C::EntityComponentSystem& ecs) : m_ECS(ecs) {};

    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;

    Entity(Entity&&) noexcept = default;
    Entity& operator=(Entity&& other) noexcept
    {
        Entity::operator=(std::move(other));
        return *this;
    }

    /** @brief Returns the entity's unique ID. */
    EntityID GetId() const;

    /** @brief Destroys a direct child by ID. */
    void DestroyChild(EntityID id);

    /**
     * @brief Attaches an entity as a direct child.
     * Detaches it from its previous parent if necessary.
     */
    void AttachChild(Entity& child);

    void AttachChild(const EntityID id);

    /** @brief Detaches a direct child by ID. */
    void DetachChild(EntityID id);

    Entity& GetChild(const EntityID id);

    /** @brief Returns a direct child by ID. */
    Entity& FindChild(EntityID id);

    /** @brief Returns a direct child by ID, or null if it does not exist. */
    U::CheckedPtr<Entity> TryFindChild(EntityID id);

    /** @brief Returns whether the entity has the specified direct child. */
    bool HasChild(EntityID id) const;

    /** @brief Returns the number of direct children. */
    size_t ChildCount() const;

    /** @brief Returns all direct children. */
    std::vector<U::CheckedPtr<Entity>> GetChildren();

    /** @brief Calls a callback for every direct child ID. */
    template <typename F> requires std::invocable<F&, EntityID>
    void ForEachChild(F&& callback)
    {
        for (const EntityID id : m_Children)
        {
            callback(id);
        }
    }

    /** @brief Destroys all direct children. */
    void DestroyChildren();

    /** @brief Returns all descendants of the entity. */
    std::vector<U::CheckedPtr<Entity>> GetDescendants();

    /** @brief Calls a callback for every descendant ID. */
    template <typename F> requires std::invocable<F&, EntityID>
    void ForEachDescendant(F&& callback)
    {

        for (const EntityID childId : m_Children)
        {
            callback(childId);
            GetChild(childId).ForEachDescendant(callback);
        }
    }

    /** @brief Returns whether the specified entity is a descendant. */
    bool HasDescendant(EntityID id) const;

    /** @brief Returns whether this entity is a descendant of the specified entity. */
    bool IsDescendantOf(EntityID id) const;

    /** @brief Returns all ancestors from the parent to the root. */
    std::vector<U::CheckedPtr<Entity>> GetAncestors() const;

    /** @brief Calls a callback for every ancestor ID. */
    template <typename F> requires std::invocable<F&, unsigned int>
    void ForEachAncestor(F&& callback) const
    {
        U::CheckedPtr<Entity> current = TryFindParent();

        while (current)
        {
            callback(current->GetId());
            current = current->TryFindParent();
        }
    }

    /** @brief Returns whether the specified entity is an ancestor. */
    bool IsAncestorOf(EntityID entityId) const;

    /** @brief Returns whether the entity has the specified ancestor. */
    bool HasAncestor(EntityID id) const;

    /** @brief Returns the entity's parent, or null if it has none. */
    U::CheckedPtr<Entity> TryFindParent() const;

    /** @brief Returns the entity's parent. */
    Entity& GetParent() const;

    /** @brief Sets the entity's parent. */
    void SetParent(Entity& parent);

    void SetParent(EntityID parent);

    /** @brief Removes the entity from its current parent. */
    void ClearParent();

    /** @brief Returns whether the entity has a parent. */
    bool HasParent() const;

    /** @brief Destroys the entity. */
    void Destroy();

    /** @brief Returns the root entity of the hierarchy. */
    Entity& GetRoot();

    /** @brief Initializes the entity. */
    virtual void Initialize() {}

  protected:
    EntityComponentSystem& m_ECS;

  private:
    void RecursiveChildren(std::vector<U::CheckedPtr<Entity>>& entities, const Entity& entity);

    U::SparseSetSoA<EntityID> m_Children{};
    EntityID m_Id{0, 0};
    EntityID m_Parent{0, 0};

    friend struct EntityComponentSystem;
};
} // namespace Ivy::C
