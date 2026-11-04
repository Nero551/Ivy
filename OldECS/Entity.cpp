#include "Entity.hpp"
#include "Core/ECS/ECS.hpp"
#include "Core/ECS/EntityID.hpp"
#include "Utilities/CheckedPtr.hpp"
#include "Utilities/Debug/Log.hpp"
#include <cstddef>
#include <format>
#include <functional>
#include <iostream>
#include <source_location>
#include <string>
#include <vector>

namespace Ivy::C
{
EntityID Entity::GetId() const
{
    return m_Id;
}
void Entity::DestroyChild(const EntityID id)
{
    if (!HasChild(id))
    {
        return;
    }

    auto& child = m_ECS.GetEntity(id);
    child.Destroy();
}

void Entity::AttachChild(Entity& child)
{
    if (HasChild(child.m_Id))
    {
        U::Log::Error("Child Entity already exists: " + std::to_string(child.m_Id.Index));
        return;
    }

    if (&child == this)
    {
        U::Log::Error("An entity cannot be its own child.");
        return;
    }

    if (child.IsDescendantOf(m_Id))
    {
        U::Log::Error("An entity cannot have its descendant as a child.");
        return;
    }

    if (child.IsAncestorOf(m_Id))
    {
        U::Log::Error("An entity cannot have its ancestor as a child.");
        return;
    }

    if (child.HasParent())
    {
        child.ClearParent();
    }

    m_Children.Emplace(child.m_Id.Index, child.m_Id);
    child.m_Parent = m_Id;
}

void Entity::AttachChild(const EntityID id)
{
    AttachChild(m_ECS.FindEntity(id));
}

void Entity::DetachChild(const EntityID id)
{
    if (m_Children.Contains(id.Index) && m_Children[id.Index] == id)
    {
        m_Children.Erase(id.Index);
        m_ECS.GetEntity(id).m_Parent = {0, 0};
    }
}

Entity& Entity::GetChild(const EntityID id)
{
    return m_ECS.GetEntity(id);
}

Entity& Entity::FindChild(const EntityID id)
{
    if (HasChild(id))
    {
        return m_ECS.FindEntity(id);
    }

    U::Log::Fatal(std::format("Entity {} has no child {}", m_Id.Index, id.Index));
}

U::CheckedPtr<Entity> Entity::TryFindChild(const EntityID id)
{
    if (HasChild(id))
    {
        return &m_ECS.FindEntity(id);
    }

    return {std::format("Entity {} has no child {}", m_Id.Index, id.Index)};
}

bool Entity::HasChild(const EntityID id) const
{
    return m_Children.Contains(id.Index) && m_Children[id.Index] == id;
}

size_t Entity::ChildCount() const
{
    return m_Children.Size();
}

std::vector<U::CheckedPtr<Entity>> Entity::GetChildren()
{
    std::vector<U::CheckedPtr<Entity>> children;
    children.reserve(m_Children.Size());

    for (auto id : m_Children)
    {
        children.emplace_back(&m_ECS.GetEntity(id));
    }

    return children;
}

void Entity::DestroyChildren()
{
    while (!m_Children.Empty())
    {
        DestroyChild(*m_Children.begin());
    }
}
std::vector<U::CheckedPtr<Entity>> Entity::GetDescendants()
{
    std::vector<U::CheckedPtr<Entity>> descendants;
    descendants.reserve(m_Children.Size() * 3);

    RecursiveChildren(descendants, *this);

    return descendants;
}

bool Entity::HasDescendant(const EntityID id) const
{
    return IsAncestorOf(id);
}

bool Entity::IsDescendantOf(const EntityID id) const
{
    EntityID current = m_Parent;

    while (current != EntityID{0, 0})
    {
        if (current == id)
        {
            return true;
        }

        current = m_ECS.GetEntity(current).m_Parent;
    }

    return false;
}

std::vector<U::CheckedPtr<Entity>> Entity::GetAncestors() const
{
    std::vector<U::CheckedPtr<Entity>> ancestors;

    EntityID current = m_Parent;

    while (current != EntityID{0, 0})
    {
        Entity& entity = m_ECS.GetEntity(current);
        ancestors.emplace_back(&entity);
        current = entity.m_Parent;
    }

    return ancestors;
}

bool Entity::IsAncestorOf(const EntityID entityId) const
{
    for (auto id : m_Children)
    {
        if (id == entityId)
        {
            return true;
        }

        const Entity& child = m_ECS.GetEntity(id);

        if (child.IsAncestorOf(entityId))
        {
            return true;
        }
    }

    return false;
}

bool Entity::HasAncestor(const EntityID id) const
{
    return IsDescendantOf(id);
}

U::CheckedPtr<Entity> Entity::TryFindParent() const
{
    if (m_Parent == EntityID{0, 0})
    {
        return nullptr;
    }
    return m_ECS.TryFindEntity(m_Parent);
}

Entity& Entity::GetParent() const
{
    return m_ECS.GetEntity(m_Parent);
}

void Entity::SetParent(Entity& parent)
{
    parent.AttachChild(*this);
}

void Entity::SetParent(EntityID parent)
{
    m_ECS.FindEntity(parent).AttachChild(*this);
}

void Entity::ClearParent()
{
    if (HasParent())
    {
        m_ECS.GetEntity(m_Parent).DetachChild(m_Id);
    }
}

bool Entity::HasParent() const
{
    return m_Parent != EntityID{0, 0};
}

void Entity::Destroy()
{
    m_ECS.RemoveEntity(m_Id);
}

Entity& Entity::GetRoot()
{
    U::CheckedPtr current = this;

    while (current->HasParent())
    {
        current = &m_ECS.GetEntity(current->m_Parent);
    }

    return *current;
}

void Entity::RecursiveChildren(std::vector<U::CheckedPtr<Entity>>& entities, const Entity& entity)
{
    for (auto id : entity.m_Children)
    {
        Entity& child = m_ECS.GetEntity(id);

        entities.emplace_back(&child);
        RecursiveChildren(entities, child);
    }
}
} // namespace Ivy::C
