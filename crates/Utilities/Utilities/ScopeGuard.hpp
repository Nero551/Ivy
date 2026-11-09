#pragma once
#include <utility>
namespace Ivy::U
{
template <typename F> struct [[nodiscard("ScopeGuard must be stored to ensure cleanup")]] ScopeGuard
{
    F Callback;

    explicit ScopeGuard(F&& callback) : Callback(std::forward<F>(callback)) {}
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
    ~ScopeGuard() noexcept
    {
        Callback();
    }
};
} // namespace Ivy::U
