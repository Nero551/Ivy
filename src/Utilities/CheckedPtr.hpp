#pragma once

namespace Ivy::U
{
/**
 * @brief Non-owning pointer wrapper that checks for null dereferences.
 * @note Does not track object lifetime or prevent dangling pointers.
 */
template <typename T> struct CheckedPtr
{
    /** @brief Constructs a null CheckedPtr. */
    CheckedPtr() = default;

    /** @brief Constructs a CheckedPtr from a raw pointer. */
    CheckedPtr(T* objectPtr) noexcept
    {
        m_Ptr = objectPtr;
    }

    /** @brief Constructs a null CheckedPtr with a custom error message. */
    CheckedPtr(const std::string_view nullMessage) noexcept
    {
        this->m_NullMessage = nullMessage;
    }

    /** @brief Constructs a CheckedPtr from a pointer with a custom error message. */
    CheckedPtr(T* objectPtr, const std::string_view nullMessage) noexcept
    {
        m_Ptr = objectPtr;
        this->m_NullMessage = nullMessage;
    }

    /** @brief Assigns a raw pointer to the CheckedPtr. */
    CheckedPtr& operator=(T* objectPtr) noexcept
    {
        m_Ptr = objectPtr;
        return *this;
    }

    /** @brief Accesses a member through the pointer, checking for null. */
    T* operator->() const
    {
        return m_Ptr;
    }

    /** @brief Compares the stored pointer with a raw pointer. */
    bool operator==(T* otherPtr) const
    {
        return m_Ptr == otherPtr;
    }

    /** @brief Compares the stored pointer with another checked pointer. */
    bool operator==(const CheckedPtr& otherCheckedPtr) const
    {
        return m_Ptr == otherCheckedPtr.m_Ptr;
    }

    /** @brief Dereferences the pointer, checking for null. */
    T& operator*() const
    {
        return *m_Ptr;
    }

    /** @brief Checks whether the pointer is non-null. */
    explicit operator bool() const
    {
        return m_Ptr != nullptr;
    }

    /** @brief Resets the pointer to null. */
    void Reset()
    {
        m_Ptr = nullptr;
    }

    /** @brief Returns the stored raw pointer without validation. */
    [[nodiscard]] T* Get() const
    {
        return m_Ptr;
    }

    /** @brief Checks whether the pointer is null. */
    [[nodiscard]] bool IsNull() const
    {
        return m_Ptr == nullptr;
    }

  private:
    T* m_Ptr = nullptr;
    std::string_view m_NullMessage = "[NULL PTR] ";
};
} // namespace Ivy::U
