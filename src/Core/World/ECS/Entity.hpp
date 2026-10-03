#pragma once

#include "Utilities/CheckedPtr.hpp"
#include "Utilities/DataStructures/SparseSetAoS.hpp"
#include "Utilities/DataStructures/SparseSetSoA.hpp"

namespace Ivy::C
{
/** @brief Represents an entity and its parent-child hierarchy. */
struct Entity
{
    virtual ~Entity() = default;

    Entity() = default;

    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;

    Entity(Entity&&) = default;
    Entity& operator=(Entity&&) = default;

    /** @brief Returns the entity's unique ID. */
    unsigned int GetId() const;

    /** @brief Destroys a direct child by ID. */
    void DestroyChild(unsigned int id);

    /**
     * @brief Attaches an entity as a direct child.
     * Detaches it from its previous parent if necessary.
     */
    void AttachChild(Entity& child);

    /** @brief Detaches a direct child by ID. */
    void DetachChild(unsigned int id);

    /** @brief Returns a direct child by ID. */
    Entity& GetChild(unsigned int id);

    /** @brief Returns a direct child by ID, or null if it does not exist. */
    U::CheckedPtr<Entity> TryGetChild(unsigned int id);

    /** @brief Returns whether the entity has the specified direct child. */
    bool HasChild(unsigned int id) const;

    /** @brief Returns the number of direct children. */
    size_t ChildCount() const;

    /** @brief Returns all direct children. */
    std::vector<U::CheckedPtr<Entity>> GetChildren();

    /** @brief Calls a callback for every direct child ID. */
    template <typename F> requires std::invocable<F&, unsigned int>
    void ForEachChild(F&& callback)
    {
        for (const auto id : m_Children)
        {
            callback(id);
        }
    }

    /** @brief Destroys all direct children. */
    void DestroyChildren();

    /** @brief Returns all descendants of the entity. */
    std::vector<U::CheckedPtr<Entity>> GetDescendants();

    /** @brief Calls a callback for every descendant ID. */
    template <typename F> requires std::invocable<F&, unsigned int>
    void ForEachDescendant(F&& callback)
    {
        ForEachChild(
            [&](const unsigned int childId)
            {
                callback(childId);
                GetChild(childId).ForEachDescendant(callback);
            });
    }

    /** @brief Returns whether the specified entity is a descendant. */
    bool HasDescendant(unsigned int id) const;

    /** @brief Returns whether this entity is a descendant of the specified entity. */
    bool IsDescendantOf(unsigned int id) const;

    /** @brief Returns all ancestors from the parent to the root. */
    std::vector<U::CheckedPtr<Entity>> GetAncestors() const;

    /** @brief Calls a callback for every ancestor ID. */
    void ForEachAncestor(const std::function<void(unsigned int)>& callback) const;

    /** @brief Returns whether the specified entity is an ancestor. */
    bool IsAncestorOf(unsigned int entityId) const;

    /** @brief Returns whether the entity has the specified ancestor. */
    bool HasAncestor(unsigned int id) const;

    /** @brief Returns the entity's parent, or null if it has none. */
    U::CheckedPtr<Entity> TryGetParent() const;

    /** @brief Returns the entity's parent. */
    Entity& GetParent() const;

    /** @brief Sets the entity's parent. */
    void SetParent(Entity& parent);

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

  private:
    void RecursiveChildren(std::vector<U::CheckedPtr<Entity>>& entities, const Entity& entity);

    U::SparseSetSoA<unsigned int> m_Children{};
    unsigned int m_Id = 0;
    unsigned int m_Parent = 0;

    friend struct World;
};
} // namespace Ivy::C