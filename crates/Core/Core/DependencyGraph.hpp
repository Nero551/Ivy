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
        m_Dirty = true;
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
        m_Dirty = true;
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
        m_Dirty = true;
        return true;
    }
    std::span<const T> TopologicalOrder()
    {
        if (m_Dirty)
        {
            m_Order.clear();
            std::vector<T> indeg(m_Nodes.size());
            for (size_t i = 0; i < m_Nodes.size(); ++i)
            {
                indeg[i] = static_cast<T>(m_Nodes[i].Predecessors.size());
            }

            for (T s : m_Sources.Data())
            {
                m_Order.push_back(s);
            }
            for (size_t h = 0; h < m_Order.size(); ++h)
            {
                for (T next : m_Nodes[m_Order[h]].Successors)
                {
                    if (--indeg[next] == 0)
                    {
                        m_Order.push_back(next);
                    }
                }
            }

            // m_Order.size() < m_Nodes.size()  ==> cycle, report it
            m_Dirty = false;
        }
        return m_Order;
    }
    std::span<const T> ViewSources() const
    {
        return m_Sources.Data();
    }
    std::vector<T> m_Order;
    bool m_Dirty = false;
    std::vector<Node> m_Nodes;
    U::SparseSetSoA<T> m_Sources;
};
} // namespace Ivy::C
