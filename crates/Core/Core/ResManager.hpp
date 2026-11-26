#pragma once

#include "Core/Res.hpp"
#include "Diagnostics/Log.hpp"
#include "Utilities/DataStructures/TypedVector.hpp"
#include <format>
#include <memory>
#include <tuple>
namespace Ivy::C
{
struct ResManager
{

    template <typename T> Res<T>& GetRes() const
    {
        auto res = m_Resources.Find<Res<T>>();
        if (res == m_Resources.end())
        {
            D::Log::Error(std::format("Res {} not found", typeid(T).name()));
        }
        return static_cast<Res<T>&>(**res);
    }

    template <typename... Args> std::tuple<Res<Args>&...> GetRes() const requires(sizeof...(Args) != 1)
    {
        return {GetRes<Args>()...};
    }

    template <typename T> Res<T>& AddRes()
    {
        auto existing = m_Resources.Find<Res<T>>();
        if (existing != m_Resources.end())
        {
            return static_cast<Res<T>&>(**existing);
        }
        m_Resources.Emplace<Res<T>>(std::make_unique<Res<T>>());
        return static_cast<Res<T>&>(*m_Resources.Get<Res<T>>());
    }

    template <typename... Args> std::tuple<Res<Args>&...> AddRes() requires(sizeof...(Args) != 1)
    {
        return {AddRes<Args>()...};
    }

  private:
    U::TypedVector<std::unique_ptr<IRes>> m_Resources;
};
} // namespace Ivy::C
