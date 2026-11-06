#include "Entity.hpp"
#include "ECS.hpp"
#include "EntityID.hpp"
#include "Utilities/CheckedPtr.hpp"
#include "Utilities/Debug/Log.hpp"
#include <format>
#include <functional>
#include <string>
#include <utility>
#include <vector>
namespace Ivy::C
{
EntityID Entity::GetId() const
{
    return m_Id;
}

Entity& Entity::GetRoot() const
{
    return m_ECS->GetActiveEntity(m_ECS->EntityHierarchy.GetRoot(GetId().Index));
}

void Entity::Destroy()
{
    m_ECS->RemoveEntity(*this);
}

void Entity::AttachChild(Entity& child)
{
    m_ECS->EntityHierarchy.Attach(m_Id.Index, child.m_Id.Index);
}

void Entity::DetachChild(Entity& child)
{
    m_ECS->EntityHierarchy.Detach(child.GetId().Index);
}

bool Entity::HasChild(const Entity& child) const
{
    return m_ECS->EntityHierarchy.IsChildOf(child.GetId().Index, GetId().Index);
}

std::vector<U::CheckedPtr<Entity>> Entity::GetChildren() const
{
    std::vector<U::CheckedPtr<Entity>> children{};
    m_ECS->EntityHierarchy.ForEachChild(
        GetId().Index, [&](unsigned int id) { children.emplace_back(&m_ECS->GetActiveEntity(id)); });
    return children;
}

unsigned int Entity::ChildCount() const
{
    return m_ECS->EntityHierarchy.GetChildCount(GetId().Index);
}

void Entity::ForEachChild(std::function_ref<void(Entity&)> callback) const
{
    m_ECS->EntityHierarchy.ForEachChild(
        GetId().Index, [&](unsigned int id) { callback(m_ECS->GetActiveEntity(id)); });
}

void Entity::DestroyChildren()
{
    m_ECS->EntityHierarchy.ForEachChild(
        GetId().Index, [&](unsigned int id) { m_ECS->GetActiveEntity(id).Destroy(); });
}

bool Entity::HasDescendant(const Entity& descendant) const
{
    return IsAncestorOf(descendant);
}

bool Entity::IsDescendantOf(const Entity& ancestor) const
{
    return m_ECS->EntityHierarchy.IsDescendantOf(GetId().Index, ancestor.GetId().Index);
}

std::vector<U::CheckedPtr<Entity>> Entity::GetDescendants() const
{
    std::vector<U::CheckedPtr<Entity>> descendants{};
    m_ECS->EntityHierarchy.ForEachDescendant(
        GetId().Index, [&](unsigned int id) { descendants.emplace_back(&m_ECS->GetActiveEntity(id)); });
    return descendants;
}

void Entity::ForEachDescendant(std::function_ref<void(Entity&)> callback)
{
    m_ECS->EntityHierarchy.ForEachDescendant(
        GetId().Index, [&](unsigned int id) { callback(m_ECS->GetActiveEntity(id)); });
}

bool Entity::HasAncestor(const Entity& ancestor) const
{
    return IsDescendantOf(ancestor);
}

bool Entity::IsAncestorOf(const Entity& descendant) const
{
    return m_ECS->EntityHierarchy.IsAncestorOf(GetId().Index, descendant.GetId().Index);
}

std::vector<U::CheckedPtr<Entity>> Entity::GetAncestors() const
{
    std::vector<U::CheckedPtr<Entity>> ancestors{};
    m_ECS->EntityHierarchy.ForEachAncestor(
        GetId().Index, [&](unsigned int id) { ancestors.emplace_back(&m_ECS->GetActiveEntity(id)); });
    return ancestors;
}

void Entity::ForEachAncestor(std::function_ref<void(Entity&)> callback)
{
    m_ECS->EntityHierarchy.ForEachAncestor(
        GetId().Index, [&](unsigned int id) { callback(m_ECS->GetActiveEntity(id)); });
}

void Entity::SetParent(Entity& parent)
{
    parent.AttachChild(*this);
}

Entity& Entity::GetParent() const
{
    return m_ECS->GetActiveEntity(m_ECS->EntityHierarchy.GetParent(GetId().Index));
}

U::CheckedPtr<Entity> Entity::TryFindParent() const
{
    return m_ECS->TryFindActiveEntity(m_ECS->EntityHierarchy.GetParent(GetId().Index));
}

bool Entity::HasParent() const
{
    return m_ECS->EntityHierarchy.HasParent(GetId().Index);
}

void Entity::ClearParent()
{
    if (HasParent())
    {
        GetParent().DetachChild(*this);
    }
}

bool Entity::operator==(const Entity& other) const
{
    return m_ECS == other.m_ECS && m_Id == other.m_Id;
}
} // namespace Ivy::C
