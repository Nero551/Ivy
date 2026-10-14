#pragma once

#include <concepts>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
namespace Ivy::U
{

template <typename T, std::unsigned_integral IndexType = unsigned int> struct StringMap
{
    struct StringHash
    {
        using is_transparent = void;

        std::size_t operator()(const std::string_view value) const noexcept
        {
            return std::hash<std::string_view>{}(value);
        }
    };

    template <bool Const> struct BasicIterator
    {
        StringMap* Map;
        IndexType Index;
        using ReturnType = std::conditional_t<Const, const T, T>;

        BasicIterator& operator++()
        {
            ++Index;
            return *this;
        }

        ReturnType& operator*() const
        {
            return Map->m_Data[Index];
        }

        ReturnType* operator->() const
        {
            return &Map->m_Data[Index];
        }

        bool operator==(const BasicIterator& other) const
        {
            return Index == other.Index;
        }

        bool operator!=(const BasicIterator& other) const
        {
            return !(*this == other);
        }
    };

    using Iterator = BasicIterator<false>;
    using ConstIterator = BasicIterator<true>;

    Iterator begin()
    {
        return {this, 0};
    }

    Iterator end()
    {
        return {this, Size()};
    }

    ConstIterator begin() const
    {
        return {this, 0};
    }

    ConstIterator end() const
    {
        return {this, Size()};
    }

    T& operator[](const std::string_view key)
    {
        if (const auto it = m_IndicesMap.find(key); it != m_IndicesMap.end())
        {
            return m_Data[it->second];
        }

        m_IndicesMap.emplace(std::string(key), Size());
        m_Data.emplace_back();

        return m_Data.back();
    }

    bool Contains(std::string_view name) const
    {
        return m_IndicesMap.contains(name);
    }

    template <typename... Args> std::pair<Iterator, bool> Emplace(const std::string_view key, Args&&... args)
    {
        if (const auto it = m_IndicesMap.find(key); it != m_IndicesMap.end())
        {
            return {
                Iterator{this, it->second},
                false
            };
        }

        const IndexType index = Size();

        m_Data.emplace_back(std::forward<Args>(args)...);
        m_IndicesMap.emplace(std::string(key), index);

        return {
            Iterator{this, index},
            true
        };
    }

    IndexType Size() const
    {
        return m_Data.size();
    }

  private:
    IndexType m_Next = 0;
    std::unordered_map<std::string, IndexType, StringHash, std::equal_to<>> m_IndicesMap{};
    std::vector<T> m_Data{};
};
} // namespace Ivy::U
