#pragma once
#include <concepts>
#include <cstddef>
#include <functional>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Ivy::U
{

/**
 * @brief Associates string keys with densely stored values.
 *
 * Maps string keys to indices in an internal vector, providing key-based
 * lookup while storing values contiguously.
 *
 * @tparam T Type of values stored in the map.
 * @tparam IndexType Unsigned integer type used to represent value indices.
 *
 * @note Keys are stored as owning std::string objects. Lookups accept
 * std::string_view and use transparent hashing and equality to avoid creating
 * temporary strings.
 */
template <typename T, std::unsigned_integral IndexType = unsigned int> struct StringMap
{
    /**
     * @brief Hashes strings and string views using their character contents.
     *
     * Declares transparent lookup support so the unordered map can search
     * using std::string_view without constructing a std::string.
     */
    struct StringHash
    {
        using is_transparent = void;

        /**
         * @brief Computes the hash of a string-like value.
         * @param value String contents to hash.
         * @return Hash value.
         */
        std::size_t operator()(const std::string_view value) const noexcept
        {
            return std::hash<std::string_view>{}(value);
        }
    };

    /**
     * @brief Iterator over values stored in the map.
     *
     * Iterates through the internal value vector in index order.
     *
     * @tparam Const Whether the iterator provides read-only access.
     */
    template <bool Const> struct BasicIterator
    {
        /** @brief Map whose values are being iterated. */
        std::conditional_t<Const, const StringMap, StringMap>* Map;

        /** @brief Index of the current value. */
        IndexType Index;

        /** @brief Reference type returned by dereferencing the iterator. */
        using ReturnType = std::conditional_t<Const, const T, T>;

        /**
         * @brief Advances the iterator to the next value.
         * @return Reference to this iterator.
         */
        BasicIterator& operator++()
        {
            ++Index;
            return *this;
        }

        /**
         * @brief Accesses the current value.
         * @return Reference to the current value.
         */
        ReturnType& operator*() const
        {
            return Map->m_Data[Index];
        }

        /**
         * @brief Accesses a member of the current value.
         * @return Pointer to the current value.
         */
        ReturnType* operator->() const
        {
            return &Map->m_Data[Index];
        }

        /**
         * @brief Checks whether two iterators have the same index.
         * @param other Iterator to compare against.
         * @return True if both iterators have the same index.
         */
        bool operator==(const BasicIterator& other) const
        {
            return Index == other.Index;
        }

        /**
         * @brief Checks whether two iterators have different indices.
         * @param other Iterator to compare against.
         * @return True if the indices differ.
         */
        bool operator!=(const BasicIterator& other) const
        {
            return !(*this == other);
        }
    };

    /** @brief Mutable iterator type. */
    using Iterator = BasicIterator<false>;

    /** @brief Read-only iterator type. */
    using ConstIterator = BasicIterator<true>;

    /**
     * @brief Gets an iterator to the first value.
     * @return Iterator to the first value, or the end iterator if empty.
     */
    Iterator begin()
    {
        return {this, 0};
    }

    /**
     * @brief Gets the end iterator.
     * @return Iterator one position past the last value.
     */
    Iterator end()
    {
        return {this, Size()};
    }

    /**
     * @brief Gets a read-only iterator to the first value.
     * @return Const iterator to the first value, or the end iterator if empty.
     */
    ConstIterator begin() const
    {
        return {this, 0};
    }

    /**
     * @brief Gets the read-only end iterator.
     * @return Const iterator one position past the last value.
     */
    ConstIterator end() const
    {
        return {this, Size()};
    }

    /**
     * @brief Gets the value associated with a key, inserting it if absent.
     *
     * If the key already exists, returns its associated value. Otherwise,
     * inserts the key and value-initializes a new value.
     *
     * @param key Key to look up or insert.
     * @return Reference to the associated value.
     */
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

    /**
     * @brief Checks whether a key exists in the map.
     * @param name Key to search for.
     * @return True if the key exists.
     */
    bool Contains(std::string_view name) const
    {
        return m_IndicesMap.contains(name);
    }

    /**
     * @brief Constructs and inserts a value if its key is absent.
     *
     * If the key already exists, leaves its value unchanged.
     *
     * @tparam Args Argument types forwarded to the value constructor.
     * @param key Key associated with the value.
     * @param args Arguments forwarded to the value constructor.
     * @return A pair containing an iterator to the associated value and a
     * Boolean indicating whether insertion occurred.
     */
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

    /**
     * @brief Gets the number of stored values.
     * @return Number of entries in the map.
     */
    IndexType Size() const
    {
        return m_Data.size();
    }

  private:
    /** @brief Next index reserved for future allocation. */
    IndexType m_Next = 0;

    /** @brief Maps owning string keys to their corresponding value indices. */
    std::unordered_map<std::string, IndexType, StringHash, std::equal_to<>> m_IndicesMap{};

    /** @brief Stores values contiguously in index order. */
    std::vector<T> m_Data{};
};
} // namespace Ivy::U
