#pragma once
#include <cstddef>
#include <exception>
#include <iostream>
#include <vector>

namespace Ivy::U
{
template <typename T> std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
    os << "[";
    for (size_t i = 0; i < vec.size(); ++i)
    {
        os << vec[i];
        if (i != vec.size() - 1)
        {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

/** @brief Provides formatted console logging and pointer validation. */
namespace Log
{

/** @brief Resets the console text color. */
constexpr const char* Reset = "\033[0m";
/** @brief Sets the console text color to red. */
constexpr const char* Red = "\033[31m";
/** @brief Sets the console text color to green. */
constexpr const char* Green = "\033[32m";
/** @brief Sets the console text color to yellow. */
constexpr const char* Yellow = "\033[33m";
/** @brief Sets the console text color to blue. */
constexpr const char* Blue = "\033[34m";

#define LOG()                                                                                                \
    std::cout << source.file_name() << " " << source.function_name() << " " << source.line() << " "          \
              << source.column() << "\n";

/** @brief Prints a message to the console. */
template <typename... Args> void Print(const Args&... args)
{
    (..., (std::cout << args));
    std::cout << '\n';
}

/** @brief Logs an informational message. */
template <typename... Args> void Info(const Args&... args)
{
    std::cout << Green << "ℹ️ [INFO] " << Reset;
    (..., (std::cout << args));
    std::cout << '\n';
}

/** @brief Logs a warning message. */
template <typename... Args> void Warning(const Args&... args)
{
    std::cout << Yellow << "⚠️ [WARNING] " << Reset;
    (..., (std::cout << args));
    std::cout << '\n';
}

/** @brief Logs a fatal error and throws an exception. */
template <typename... Args> [[noreturn]] static void Error(const Args&... args)
{
    std::cout << Red << "💀 [Error] " << Reset;
    (..., (std::cout << args));
    std::cout << '\n';
    throw std::exception();
}

/** @brief Validates a raw pointer and returns the dereferenced object. */
template <typename T, typename... Args> static T& Require(T* rawPtr, const Args&... args)
{
    if (!rawPtr)
    {
        Error(args...);
    }
    return *rawPtr;
}

/** @brief its assert but cleaner. */
template <typename... Args> static constexpr void Assert(const bool condition, const Args&... args)
{
    if (!condition)
    {
        Error(args...);
    }
}
}; // namespace Log
} // namespace Ivy::U
