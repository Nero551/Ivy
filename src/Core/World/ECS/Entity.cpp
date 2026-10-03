#include "Entity.hpp"
#include "Core/Engine.hpp"

namespace Ivy::C
{
unsigned int Entity::GetId() const
{
    return m_Id;
}
void Entity::DestroyChild(const unsigned int id)
{
    if (!HasChild(id))
    {
        return;
    }

    auto& child = World::Get().FindEntity(id);
    child.Destroy();
}

void Entity::AttachChild(Entity& child)
{
    if (HasChild(child.m_Id))
    {
        U::Log::Error("Child Entity already exists: " + std::to_string(child.m_Id));
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

    m_Children.Emplace(child.m_Id, child.m_Id);
    child.m_Parent = m_Id;
}

void Entity::DetachChild(const unsigned int id)
{
    if (m_Children.Erase(id))
    {
        World::Get().FindEntity(id).m_Parent = 0;
    }
}

Entity& Entity::GetChild(const unsigned int id)
{
    if (HasChild(id))
    {
        return World::Get().FindEntity(id);
    }

    U::Log::Fatal(std::format("Entity {} has no child {}", m_Id, id));
}

U::CheckedPtr<Entity> Entity::TryGetChild(const unsigned int id)
{
    if (HasChild(id))
    {
        return &World::Get().FindEntity(id);
    }

    return {std::format("Entity {} has no child {}", m_Id, id)};
}

bool Entity::HasChild(const unsigned int id) const
{
    return m_Children.Contains(id);
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
        children.emplace_back(&World::Get().FindEntity(id));
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

bool Entity::HasDescendant(const unsigned int id) const
{
    return IsAncestorOf(id);
}

bool Entity::IsDescendantOf(const unsigned int id) const
{
    unsigned int current = m_Parent;

    while (current != 0)
    {
        if (current == id)
        {
            return true;
        }

        current = World::Get().FindEntity(current).m_Parent;
    }

    return false;
}

std::vector<U::CheckedPtr<Entity>> Entity::GetAncestors() const
{
    std::vector<U::CheckedPtr<Entity>> ancestors;

    unsigned int current = m_Parent;

    while (current != 0)
    {
        Entity& entity = World::Get().FindEntity(current);
        ancestors.emplace_back(&entity);
        current = entity.m_Parent;
    }

    return ancestors;
}

void Entity::ForEachAncestor(const std::function<void(unsigned int)>& callback) const
{
    unsigned int current = m_Parent;

    while (current != 0)
    {
        callback(current);
        const Entity& entity = World::Get().FindEntity(current);
        current = entity.m_Parent;
    }
}
bool Entity::IsAncestorOf(const unsigned int entityId) const
{
    for (auto id : m_Children)
    {
        if (id == entityId)
        {
            return true;
        }

        const Entity& child = World::Get().FindEntity(id);

        if (child.IsAncestorOf(entityId))
        {
            return true;
        }
    }

    return false;
}

bool Entity::HasAncestor(const unsigned int id) const
{
    return IsDescendantOf(id);
}

U::CheckedPtr<Entity> Entity::TryGetParent() const
{
    if (m_Parent == 0)
    {
        return nullptr;
    }
    return &World::Get().FindEntity(m_Parent);
}

Entity& Entity::GetParent() const
{
    return World::Get().GetEntity(m_Parent);
}

void Entity::SetParent(Entity& parent)
{
    parent.AttachChild(*this);
}

void Entity::ClearParent()
{
    if (HasParent())
    {
        World::Get().GetEntity(m_Parent).DetachChild(m_Id);
    }
}

bool Entity::HasParent() const
{
    return m_Parent != 0;
}

void Entity::Destroy()
{
    World::Get().RemoveEntity(m_Id);
}

Entity& Entity::GetRoot()
{
    U::CheckedPtr current = this;

    while (current->HasParent())
    {
        current = &World::Get().FindEntity(current->m_Parent);
    }

    return *current;
}

void Entity::RecursiveChildren(std::vector<U::CheckedPtr<Entity>>& entities, const Entity& entity)
{
    for (auto id : entity.m_Children)
    {
        Entity& child = World::Get().FindEntity(id);

        entities.emplace_back(&child);
        RecursiveChildren(entities, child);
    }
}
} // namespace Ivy::C