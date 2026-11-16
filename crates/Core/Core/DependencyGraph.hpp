#pragma once
#include "Utilities/DataStructures/SparseSetSoA.hpp"
#include <algorithm>
#include <concepts>
#include <cstddef>
#include <limits>
#include <span>
#include <vector>
namespace Ivy::C
{
template <std::unsigned_integral T> struct DependencyGraph
{
    static constexpr T Invalid = std::numeric_limits<T>::max();
    struct Node
    {
        std::vector<T> Predecessors;
        std::vector<T> Successors;
    };

    void Ensure(T index)
    {
        if (index == Invalid)
        {
            return;
        }

        std::size_t oldSize = m_Nodes.size();
        std::size_t newSize = static_cast<std::size_t>(index) + 1;

        if (newSize <= oldSize)
        {
            return;
        }

        m_Nodes.resize(newSize);

        for (std::size_t i = oldSize; i < newSize; ++i)
        {
            T node = static_cast<T>(i);
            m_Sources.Push(node, node);
        }
    }

    bool Exists(T node) const
    {
        return node < m_Nodes.size();
    }

    void Attach(T pred, T suc)
    {
        if (!Exists(pred) || !Exists(suc))
        {
            return;
        }
        m_Nodes[pred].Successors.push_back(suc);
        m_Nodes[suc].Predecessors.push_back(pred);
        m_Sources.Erase(suc);
    }

    bool Detach(T pred, T suc)
    {
        if (!Exists(pred) || !Exists(suc))
        {
            return false;
        }

        Node& predNode = m_Nodes[pred];
        Node& sucNode = m_Nodes[suc];

        auto edge = std::ranges::find(predNode.Successors, suc);

        if (edge == predNode.Successors.end())
        {
            return false;
        }

        std::erase(predNode.Successors, suc);
        std::erase(sucNode.Predecessors, pred);

        if (sucNode.Predecessors.empty())
        {
            m_Sources.Push(suc, suc);
        }

        return true;
    }
    template <typename F> void ForEachDescendant(T node, F&& action) const requires std::invocable<F, T>
    {
        if (!Exists(node))
        {
            return;
        }

        std::vector<bool> visited(m_Nodes.size(), false);

        auto Traverse = [&](auto&& self, T current) -> void
        {
            for (T child : m_Nodes[current].Successors)
            {
                if (visited[child])
                {
                    continue;
                }

                visited[child] = true;
                action(child);
                self(self, child);
            }
        };

        Traverse(Traverse, node);
    }

    std::span<const T> ViewSources() const
    {
        return m_Sources.Data();
    }

    std::vector<Node> m_Nodes;
    U::SparseSetSoA<T> m_Sources;
};
} // namespace Ivy::C
