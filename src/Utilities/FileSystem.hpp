#pragma once

#include <fstream>
#include <sstream>
#include <string>

#include "Utilities/Log.hpp"

namespace Ivy::U::FileSystem
{
/** @brief Reads and returns the contents of a file. */
inline std::string ReadFile(const std::string& path)
{
    std::ifstream file(path);

    if (!file)
    {
        Log::Error("Failed to read file: ", path);
        return {};
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

/** @brief Writes content to a file. */
inline void WriteFile(const std::string& path, const std::string& content)
{
    std::ofstream file(path);

    if (!file)
    {
        Log::Error("Failed to write file: ", path);
        return;
    }

    file << content;
}
} // namespace Ivy::U::FileSystem