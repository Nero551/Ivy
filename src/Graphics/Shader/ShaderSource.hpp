#pragma once

#include "Core/Services/ResourceManager/Resource.hpp"
#include "ShaderStage.hpp"

namespace N::G
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

    // TODO- it would be really cool if i could have methods for adding code
    //  dynamically , like GenerateInt("MaxLights", 20)
    //  and it adds it to the shader code, (hint: dont forget std::variants)
    //  could also do full string and just concatenate. Generate("int MaxLights = 20;",
    //  lineNumber) although, all this requires a more advanced parser , like maybe line
    //  by line and storing which line has which code.

    ShaderSource(const std::string& name, const std::string& path, ShaderStage stage,
        const std::string& version = "version 460 core");

    ~ShaderSource();
    unsigned int GetId() const;
    const std::string& GetName() const;
    ShaderStage GetStage() const;

    /** @brief Preprocesses the source, generates an OpenGL Id & compiles the source */
    void Compile();

    /** @brief checks if the source was compiled.
     * @return true if compiled , false if not.
     */
    bool IsCompiled() const;

    /** @brief Reloads and preprocesses the shader source. */
    void Reload();

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
} // namespace N::G
