#include <cstddef>
#include <cstdlib>
#include <new>
#include <tracy/Tracy.hpp>
// ============================================================================
// Standard new
// ============================================================================

void* operator new(const std::size_t count)
{
    void* ptr = std::malloc(count);

    if (!ptr)
    {
        throw std::bad_alloc();
    }

    TracyAllocS(ptr, count, 16);
    return ptr;
}

void* operator new[](const std::size_t count)
{
    void* ptr = std::malloc(count);

    if (!ptr)
    {
        throw std::bad_alloc();
    }

    TracyAllocS(ptr, count, 16);
    return ptr;
}

// ============================================================================
// Sized delete
// ============================================================================

void operator delete(void* ptr, std::size_t) noexcept
{
    if (!ptr)
    {
        return;
    }

    TracyFreeS(ptr, 16);
    std::free(ptr);
}

void operator delete[](void* ptr, std::size_t) noexcept
{
    if (!ptr)
    {
        return;
    }

    TracyFreeS(ptr, 16);
    std::free(ptr);
}

// ============================================================================
// Unsized delete
// ============================================================================

void operator delete(void* ptr) noexcept
{
    if (!ptr)
    {
        return;
    }

    TracyFreeS(ptr, 16);
    std::free(ptr);
}

void operator delete[](void* ptr) noexcept
{
    if (!ptr)
    {
        return;
    }

    TracyFreeS(ptr, 16);
    std::free(ptr);
}

// ============================================================================
// Nothrow new
// ============================================================================

void* operator new(const std::size_t count, const std::nothrow_t&) noexcept
{
    void* ptr = std::malloc(count);

    if (ptr)
    {
        TracyAllocS(ptr, count, 16);
    }

    return ptr;
}

void* operator new[](const std::size_t count, const std::nothrow_t&) noexcept
{
    void* ptr = std::malloc(count);

    if (ptr)
    {
        TracyAllocS(ptr, count, 16);
    }

    return ptr;
}

// ============================================================================
// Nothrow delete
// ============================================================================

void operator delete(void* ptr, const std::nothrow_t&) noexcept
{
    if (!ptr)
    {
        return;
    }

    TracyFreeS(ptr, 16);
    std::free(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t&) noexcept
{
    if (!ptr)
    {
        return;
    }

    TracyFreeS(ptr, 16);
    std::free(ptr);
}

// ============================================================================
// Aligned new
// ============================================================================

void* operator new(const std::size_t count, std::align_val_t alignment)
{
    const std::size_t align = static_cast<std::size_t>(alignment);

    void* ptr = std::aligned_alloc(align, count);

    if (!ptr)
    {
        throw std::bad_alloc();
    }

    TracyAllocS(ptr, count, 16);
    return ptr;
}

void* operator new[](const std::size_t count, std::align_val_t alignment)
{
    const std::size_t align = static_cast<std::size_t>(alignment);

    void* ptr = std::aligned_alloc(align, count);

    if (!ptr)
    {
        throw std::bad_alloc();
    }

    TracyAllocS(ptr, count, 16);
    return ptr;
}

// ============================================================================
// Aligned delete
// ============================================================================

void operator delete(void* ptr, std::align_val_t) noexcept
{
    if (!ptr)
    {
        return;
    }

    TracyFreeS(ptr, 16);
    std::free(ptr);
}

void operator delete[](void* ptr, std::align_val_t) noexcept
{
    if (!ptr)
    {
        return;
    }

    TracyFreeS(ptr, 16);
    std::free(ptr);
}

// ============================================================================
// Sized + aligned delete
// ============================================================================

void operator delete(void* ptr, std::size_t, std::align_val_t) noexcept
{
    if (!ptr)
    {
        return;
    }

    TracyFreeS(ptr, 16);
    std::free(ptr);
}

void operator delete[](void* ptr, std::size_t, std::align_val_t) noexcept
{
    if (!ptr)
    {
        return;
    }

    TracyFreeS(ptr, 16);
    std::free(ptr);
}

// ============================================================================
// Aligned nothrow new
// ============================================================================

void* operator new(const std::size_t count, std::align_val_t alignment, const std::nothrow_t&) noexcept
{
    const std::size_t align = static_cast<std::size_t>(alignment);

    void* ptr = std::aligned_alloc(align, count);

    if (ptr)
    {
        TracyAllocS(ptr, count, 16);
    }

    return ptr;
}

void* operator new[](const std::size_t count, std::align_val_t alignment, const std::nothrow_t&) noexcept
{
    const std::size_t align = static_cast<std::size_t>(alignment);

    void* ptr = std::aligned_alloc(align, count);

    if (ptr)
    {
        TracyAllocS(ptr, count, 16);
    }

    return ptr;
}

// ============================================================================
// Aligned nothrow delete
// ============================================================================

void operator delete(void* ptr, std::align_val_t, const std::nothrow_t&) noexcept
{
    if (!ptr)
    {
        return;
    }

    TracyFreeS(ptr, 16);
    std::free(ptr);
}

void operator delete[](void* ptr, std::align_val_t, const std::nothrow_t&) noexcept
{
    if (!ptr)
    {
        return;
    }

    TracyFreeS(ptr, 16);
    std::free(ptr);
}