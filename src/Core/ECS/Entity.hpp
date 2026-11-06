#pragma once
#include "EntityID.hpp"
#include "Utilities/CheckedPtr.hpp"
#include <functional>
#include <utility>
#include <vector>
namespace Ivy::C
{

struct EntityComponentSystem;
struct IArchetype;
struct Query;

struct Entity
{
    Entity(EntityComponentSystem& ecs) : m_ECS(&ecs) {}
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;
    Entity(Entity&&) noexcept = default;
    Entity& operator=(Entity&& other) noexcept = default;

    EntityID GetId() const;
    Entity& GetRoot() const;
    void Destroy();

    void AttachChild(Entity& child);
    void DetachChild(Entity& child);
    bool HasChild(const Entity& child) const;
    std::vector<U::CheckedPtr<Entity>> GetChildren() const;
    unsigned int ChildCount() const;
    void ForEachChild(std::function_ref<void(Entity&)> callback) const;
    void DestroyChildren();

    bool HasDescendant(const Entity& descendant) const;
    bool IsDescendantOf(const Entity& ancestor) const;
    std::vector<U::CheckedPtr<Entity>> GetDescendants() const;
    void ForEachDescendant(std::function_ref<void(Entity&)> callback);

    bool HasAncestor(const Entity& ancestor) const;
    bool IsAncestorOf(const Entity& ancestor) const;
    std::vector<U::CheckedPtr<Entity>> GetAncestors() const;
    void ForEachAncestor(std::function_ref<void(Entity&)> callback);

    void SetParent(Entity& parent);
    Entity& GetParent() const;
    U::CheckedPtr<Entity> TryFindParent() const;
    bool HasParent() const;
    void ClearParent();

    bool operator==(Entity& other) const;

  protected:
    U::CheckedPtr<EntityComponentSystem> m_ECS;
    virtual void Initialize() {}

  private:
    EntityID m_Id;
    U::CheckedPtr<IArchetype> m_Archetype;

    friend struct Query;
    friend struct EntityComponentSystem;
};
} // namespace Ivy::C
