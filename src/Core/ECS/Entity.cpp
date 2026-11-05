#include "Entity.hpp"
#include "ECS.hpp"
#include "EntityID.hpp"
#include "Utilities/CheckedPtr.hpp"
#include "Utilities/Debug/Log.hpp"
#include <format>
#include <functional>
#include <string>
#include <vector>
namespace Ivy::C
{
EntityID Entity::GetId() const
{
    return m_Id;
}

Entity& Entity::GetRoot()
{
    U::CheckedPtr<Entity> current = this;

    while (current->HasParent())
    {
        current = &m_ECS.GetEntity(current->m_Parent);
    }

    return *current;
}

void Entity::Destroy()
{
    m_ECS.RemoveEntity(*this);
}

void Entity::AttachChild(Entity& child)
{
    if (HasChild(child))
    {
        U::Log::Fatal("Child Entity already exists: ", child.m_Id.Index);
        return;
    }

    if (&child == this)
    {
        U::Log::Error("An entity cannot be its own child.");
        return;
    }

    if (child.IsDescendantOf(*this))
    {
        U::Log::Error("An entity cannot have its descendant as a child.");
        return;
    }

    if (child.IsAncestorOf(*this))
    {
        U::Log::Error("An entity cannot have its ancestor as a child.");
        return;
    }

    m_Children.Emplace(child.m_Id.Index, child.m_Id);
    child.m_Parent = m_Id;
}

void Entity::DetachChild(Entity& child)
{
    if (HasChild(child))
    {
        m_Children.Erase(child.GetId().Index);
        child.ClearParent();
    }
}

bool Entity::HasChild(const Entity& child) const
{
    return m_Children.Contains(child.GetId().Index) && m_Children[child.GetId().Index] == child.GetId();
}

std::vector<U::CheckedPtr<Entity>> Entity::GetChildren() const
{
    std::vector<U::CheckedPtr<Entity>> children;
    children.reserve(m_Children.Size());

    for (auto id : m_Children)
    {
        children.emplace_back(&m_ECS.GetEntity(id));
    }

    return children;
}

unsigned int Entity::ChildCount() const
{
    return m_Children.Size();
}

void Entity::ForEachChild(std::function_ref<void(Entity&)> callback) const
{
    for (const EntityID id : m_Children)
    {
        callback(m_ECS.GetEntity(id));
    }
}

void Entity::DestroyChild(Entity& child)
{
    if (!HasChild(child))
    {
        return;
    }
    child.Destroy();
}

void Entity::DestroyChildren()
{
    while (!m_Children.Empty())
    {
        DestroyChild(m_ECS.GetEntity(*m_Children.begin()));
    }
}

bool Entity::HasDescendant(const Entity& descendant) const
{
    return IsAncestorOf(descendant);
}

bool Entity::IsDescendantOf(const Entity& ancestor) const
{
    EntityID current = m_Parent;

    while (current != EntityID{0, 0})
    {
        if (current == ancestor.GetId())
        {
            return true;
        }

        current = m_ECS.GetEntity(current).m_Parent;
    }

    return false;
}

std::vector<U::CheckedPtr<Entity>> Entity::GetDescendants() const
{
    std::vector<U::CheckedPtr<Entity>> descendants;
    descendants.reserve(m_Children.Size() * 3);

    RecursiveChildren(descendants, *this);

    return descendants;
}

void Entity::ForEachDescendant(std::function_ref<void(Entity&)> callback)
{
    for (EntityID id : m_Children)
    {
        auto& child = m_ECS.GetEntity(id);
        callback(child);
        child.ForEachDescendant(callback);
    }
}

bool Entity::HasAncestor(const Entity& ancestor) const
{
    return IsDescendantOf(ancestor);
}

bool Entity::IsAncestorOf(const Entity& descendant) const
{
    for (auto id : m_Children)
    {
        if (id == descendant.GetId())
        {
            return true;
        }

        const Entity& child = m_ECS.GetEntity(id);

        if (child.IsAncestorOf(descendant))
        {
            return true;
        }
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

void Entity::ForEachAncestor(std::function_ref<void(Entity&)> callback)
{
    U::CheckedPtr<Entity> current = TryFindParent();

    while (current)
    {
        callback(*current);
        current = current->TryFindParent();
    }
}

void Entity::SetParent(Entity& parent)
{
    parent.AttachChild(*this);
}

Entity& Entity::GetParent() const
{
    return m_ECS.GetEntity(m_Parent);
}

U::CheckedPtr<Entity> Entity::TryFindParent() const
{
    if (m_Parent == EntityID{0, 0})
    {
        return nullptr;
    }
    return m_ECS.TryFindEntity(m_Parent);
}

bool Entity::HasParent() const
{
    return m_Parent != EntityID{0, 0};
}

void Entity::ClearParent()
{
    if (HasParent())
    {
        GetParent().DetachChild(*this);
    }
}

bool Entity::operator==(Entity& other) const
{
    return GetId() == other.GetId();
}

void Entity::RecursiveChildren(std::vector<U::CheckedPtr<Entity>>& entities, const Entity& entity) const
{
    for (auto id : entity.m_Children)
    {
        Entity& child = m_ECS.GetEntity(id);

        entities.emplace_back(&child);
        RecursiveChildren(entities, child);
    }
}
} // namespace Ivy::C
