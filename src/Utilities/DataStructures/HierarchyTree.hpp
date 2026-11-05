#pragma once
#include <concepts>
#include <functional>
#include <limits>
#include <vector>
namespace Ivy::U
{
template <std::unsigned_integral T> struct HierarchyTree
{
    static constexpr T Invalid = std::numeric_limits<T>::max();

    struct Node
    {
        T ChildCount = 0;
        T Parent = Invalid;
        T FirstChild = Invalid;
        T LastChild = Invalid;
        T NextSibling = Invalid;
        T PrevSibling = Invalid;
    };

    void Ensure(T index)
    {
        if (index == Invalid)
        {
            return;
        }

        if (index >= m_Nodes.size())
        {
            m_Nodes.resize(index + 1);
        }
    }
    bool Exists(T index) const
    {
        return index < m_Nodes.size();
    }

    void Attach(T parent, T child)
    {
        if (!Exists(parent) || !Exists(child) || parent == child)
        {
            return;
        }

        if (IsDescendantOf(parent, child))
        {
            return;
        }

        Detach(child);

        Node& p = m_Nodes[parent];
        Node& c = m_Nodes[child];

        c.Parent = parent;
        c.PrevSibling = p.LastChild;
        c.NextSibling = Invalid;

        if (p.LastChild != Invalid)
        {
            m_Nodes[p.LastChild].NextSibling = child;
        }
        else
        {
            p.FirstChild = child;
        }

        ++p.ChildCount;
        p.LastChild = child;
    }
    void Detach(T child)
    {
        if (!Exists(child))
        {
            return;
        }
        Node& c = m_Nodes[child];

        if (c.Parent != Invalid)
        {
            Node& p = m_Nodes[c.Parent];
            if (p.FirstChild == child)
            {
                p.FirstChild = c.NextSibling;
            }

            if (p.LastChild == child)
            {
                p.LastChild = c.PrevSibling;
            }
            --p.ChildCount;
        }
        if (c.NextSibling != Invalid)
        {
            Node& ns = m_Nodes[c.NextSibling];
            ns.PrevSibling = c.PrevSibling;
        }

        if (c.PrevSibling != Invalid)
        {
            Node& ps = m_Nodes[c.PrevSibling];
            ps.NextSibling = c.NextSibling;
        }

        c.PrevSibling = Invalid;
        c.NextSibling = Invalid;
        c.Parent = Invalid;
    }

    bool IsLeaf(T node) const
    {
        return Exists(node) && m_Nodes[node].FirstChild == Invalid && m_Nodes[node].LastChild == Invalid;
    }
    bool IsChildOf(T child, T parent) const
    {
        return Exists(child) && Exists(parent) && m_Nodes[child].Parent == parent;
    }
    bool IsParentOf(T parent, T child) const
    {
        return IsChildOf(child, parent);
    }
    bool IsSiblingOf(T node, T sibling) const
    {
        return Exists(node) && Exists(sibling) && node != sibling && m_Nodes[node].Parent != Invalid &&
            m_Nodes[node].Parent == m_Nodes[sibling].Parent;
    }
    bool IsDescendantOf(T descendant, T ancestor) const
    {
        if (!Exists(descendant) || !Exists(ancestor))
        {
            return false;
        }
        T current = m_Nodes[descendant].Parent;

        while (current != ancestor && current != Invalid)
        {
            current = m_Nodes[current].Parent;
        }
        return current == ancestor;
    }
    bool IsAncestorOf(T ancestor, T descendant) const
    {
        return IsDescendantOf(descendant, ancestor);
    }

    T GetParent(T node) const
    {
        return Exists(node) ? m_Nodes[node].Parent : Invalid;
    }
    T GetFirstChild(T node) const
    {
        return Exists(node) ? m_Nodes[node].FirstChild : Invalid;
    }
    T GetLastChild(T node) const
    {
        return Exists(node) ? m_Nodes[node].LastChild : Invalid;
    }
    T GetNextSibling(T node) const
    {
        return Exists(node) ? m_Nodes[node].NextSibling : Invalid;
    }
    T GetPrevSibling(T node) const
    {
        return Exists(node) ? m_Nodes[node].PrevSibling : Invalid;
    }
    T GetRoot(T node) const
    {
        if (!Exists(node))
        {
            return Invalid;
        }

        T current = node;

        while (m_Nodes[current].Parent != Invalid)
        {
            current = m_Nodes[current].Parent;
        }

        return current;
    }
    T GetDepth(T node) const
    {
        if (!Exists(node))
        {
            return Invalid;
        }

        T depth = 0;
        T current = node;

        while (m_Nodes[current].Parent != Invalid)
        {
            ++depth;
            current = m_Nodes[current].Parent;
        }

        return depth;
    }
    T GetChildCount(T node) const
    {
        return Exists(node) ? m_Nodes[node].ChildCount : 0;
    }

    std::vector<T> GetChildren(T node) const
    {
        if (!Exists(node))
        {
            return {};
        }
        std::vector<T> children{};
        T current = m_Nodes[node].FirstChild;
        while (current != Invalid)
        {
            children.push_back(current);
            current = m_Nodes[current].NextSibling;
        }
        return children;
    }
    std::vector<T> GetSiblings(T node) const
    {
        if (!Exists(node) || !Exists(m_Nodes[node].Parent))
        {
            return {};
        }

        std::vector<T> siblings = GetChildren(m_Nodes[node].Parent);
        std::erase(siblings, node);
        return siblings;
    }
    std::vector<T> GetDescendants(T node) const
    {
        if (!Exists(node))
        {
            return {};
        }

        std::vector<T> descendants{};

        const auto Visit = [&](const auto& self, T parent)
        {
            T current = m_Nodes[parent].FirstChild;

            while (current != Invalid)
            {
                descendants.push_back(current);
                self(self, current);
                current = m_Nodes[current].NextSibling;
            }
        };

        Visit(Visit, node);

        return descendants;
    }
    std::vector<T> GetAncestors(T node) const
    {
        if (!Exists(node))
        {
            return {};
        }
        std::vector<T> ancestors{};
        T current = m_Nodes[node].Parent;
        while (current != Invalid)
        {
            ancestors.push_back(current);
            current = m_Nodes[current].Parent;
        }
        return ancestors;
    }

    void ForEachChild(T node, std::function_ref<void(T)> action) const
    {
        if (!Exists(node))
        {
            return;
        }
        T current = m_Nodes[node].FirstChild;
        while (current != Invalid)
        {
            action(current);
            current = m_Nodes[current].NextSibling;
        }
    }
    void ForEachSibling(T node, std::function_ref<void(T)> action) const
    {
        if (!Exists(node) || !Exists(m_Nodes[node].Parent))
        {
            return;
        }

        ForEachChild(m_Nodes[node].Parent,
            [&](T sibling)
            {
                if (sibling != node)
                {
                    action(sibling);
                }
            });
    }
    void ForEachDescendant(T node, std::function_ref<void(T)> action) const
    {
        if (!Exists(node))
        {
            return;
        }

        const auto Visit = [&](const auto& self, T parent)
        {
            T current = m_Nodes[parent].FirstChild;

            while (current != Invalid)
            {
                action(current);
                self(self, current);
                current = m_Nodes[current].NextSibling;
            }
        };

        Visit(Visit, node);
    }
    void ForEachAncestor(T node, std::function_ref<void(T)> action) const
    {
        if (!Exists(node))
        {
            return;
        }
        T current = m_Nodes[node].Parent;
        while (current != Invalid)
        {
            action(current);
            current = m_Nodes[current].Parent;
        }
    }

  private:
    std::vector<Node> m_Nodes;
};
} // namespace Ivy::U
