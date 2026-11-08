#pragma once

#include "ShaderStage.hpp"
#include "Utilities/FileSystem.hpp"
#include <string>
#include <unordered_set>
#include <utility>

namespace Ivy::G
{
/**
 * @brief Represents a shader source file and its OpenGL shader object.
 * Handles loading, preprocessing, and reloading of a shader source.
 */
struct ShaderSource
{
    std::string Path;

    /** @brief code loaded from the source , no modifications */
    std::string SourceCode;

    /** @brief SourceCode with modifications (preprocessing) */
    std::string GeneratedCode;

    std::string Version = "version 460 core";

    //TODO: it would be really cool if i could have methods for adding code
    //  dynamically , like GenerateInt("MaxLights", 20)
    //  and it adds it to the shader code, (hint: dont forget std::variants)
    //  could also do full string and just concatenate. Generate("int MaxLights = 20;",
    //  lineNumber) although, all this requires a more advanced parser , like maybe line
    //  by line and storing which line has which code.

    ShaderSource(std::string name, const std::string& path, ShaderStage stage,
        std::string version = "version 460 core")
        : Path(path), Version(std::move(version)), m_Name(std::move(name)), m_Stage(stage)
    {
        SourceCode = U::FileSystem::ReadFile(path);
    }

    ~ShaderSource()
    {
        Delete();
    }
    ShaderSource(const ShaderSource&) = delete;
    ShaderSource& operator=(const ShaderSource&) = delete;
    ShaderSource(ShaderSource&& other) noexcept
        : Path(std::move(other.Path)), SourceCode(std::move(other.SourceCode)),
          GeneratedCode(std::move(other.GeneratedCode)), Version(std::move(other.Version)),
          m_Name(std::move(other.m_Name)), m_Id(std::exchange(other.m_Id, 0)), m_Stage(other.m_Stage),
          m_Includes(std::move(other.m_Includes))
    {
    }

    ShaderSource& operator=(ShaderSource&& other) noexcept
    {
        if (this != &other)
        {
            Delete();

            Path = std::move(other.Path);
            SourceCode = std::move(other.SourceCode);
            GeneratedCode = std::move(other.GeneratedCode);
            Version = std::move(other.Version);
            m_Name = std::move(other.m_Name);
            m_Id = std::exchange(other.m_Id, 0);
            m_Stage = other.m_Stage;
            m_Includes = std::move(other.m_Includes);
        }

        return *this;
    }

    unsigned int GetId() const;
    const std::string& GetName() const;
    ShaderStage GetStage() const;

    /** @brief Preprocesses the source, generates an OpenGL Id & compiles the source */
    void Compile();
    bool IsCompiled() const;

    void Delete();

  private:
    std::string m_Name;
    unsigned int m_Id = 0;
    ShaderStage m_Stage;

    /** @brief Paths of shader files included by this source. */
    std::unordered_set<std::string> m_Includes;

    void Preprocess();
    void PreprocessIncludes(
        const std::string& path, std::string& code, std::unordered_set<std::string>& includesProcessing);
};
} // namespace Ivy::G
