#include "ShaderSource.hpp"

#include <filesystem>
#include <string>
#include <unordered_set>
#include <utility>

#include "../../Utilities/Debug/Log.hpp"
#include "../OpenGL.hpp"
#include "Graphics/Shader/ShaderStage.hpp"
#include "Utilities/FileSystem.hpp"

namespace Ivy::G
{

unsigned int ShaderSource::GetId() const
{
    return m_Id;
}

const std::string& ShaderSource::GetName() const
{
    return m_Name;
}

ShaderStage ShaderSource::GetStage() const
{
    return m_Stage;
}

void ShaderSource::Compile()
{
    if (IsCompiled())
    {
        return;
    }

    GeneratedCode = SourceCode;
    Preprocess();

    // auto result = ShaderSourceValidator::Validate(*this);
    // if (!result.Success)
    // {
    //     U::Logger::Error("[VALIDATOR] ", result.Log);
    //     if (Stage == ShaderStage::Fragment)
    //     {
    //         U::FileSystem::WriteFile("Assets/ShaderCompileError.frag", GeneratedCode);
    //     }
    //     if (Stage == ShaderStage::Vertex)
    //     {
    //         U::FileSystem::WriteFile("Assets/ShaderCompileError.vert", GeneratedCode);
    //     }
    //     if (Stage == ShaderStage::Geometry)
    //     {
    //         U::FileSystem::WriteFile("Assets/ShaderCompileError.geom", GeneratedCode);
    //     }
    // }

    const char* string = GeneratedCode.c_str();

    m_Id = glCreateShader(static_cast<GLenum>(m_Stage));
    glShaderSource(m_Id, 1, &string, nullptr);
    glCompileShader(m_Id);

    int success;
    char infoLog[512];
    glGetShaderiv(m_Id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(m_Id, 512, nullptr, infoLog);
        U::Log::Error(std::string("Shader Source:" + GetName() + infoLog + " | " + Path));

        if (m_Stage == ShaderStage::Fragment)
        {
            U::FileSystem::WriteFile("Assets/ShaderCompileError.frag", GeneratedCode);
        }
        if (m_Stage == ShaderStage::Vertex)
        {
            U::FileSystem::WriteFile("Assets/ShaderCompileError.vert", GeneratedCode);
        }
        Delete();
    }
}

bool ShaderSource::IsCompiled() const
{
    return m_Id != 0;
}

void ShaderSource::Delete()
{
    if (IsCompiled())
    {
        SourceCode = U::FileSystem::ReadFile(Path);
        GeneratedCode = SourceCode;
        m_Includes.clear();
        glDeleteShader(m_Id);
        m_Id = 0;
    }
}

void ShaderSource::Preprocess()
{
    GeneratedCode.insert(0, "#" + Version + "\n");

    std::unordered_set<std::string> includesProcessing;
    PreprocessIncludes(Path, GeneratedCode, includesProcessing);
}

//TODO: replace with line by line parsing
void ShaderSource::PreprocessIncludes(
    const std::string& path, std::string& code, std::unordered_set<std::string>& includesProcessing)
{
    const std::string include = "#include \"";
    auto pos = code.find(include);

    while (pos != std::string::npos)
    {
        const auto start = pos + include.length();
        const auto end = code.find('\"', start);
        const auto directory = code.substr(start, end - start);
        auto includePath = std::filesystem::path(path).parent_path() / directory;

        if (!includePath.empty())
        {
            // Check If Is Already Included
            if (m_Includes.contains(includePath))
            {
                code.replace(pos, end - pos + 1, "");
            }
            else
            {
                // Check Circular Include
                if (!includesProcessing.insert(includePath).second)
                {
                    U::Log::Fatal("Circular Include: " + includePath.string() + " | In Shader: " + path);
                }

                // Recursively Include
                std::string includeCode = U::FileSystem::ReadFile(includePath);
                PreprocessIncludes(includePath, includeCode, includesProcessing);

                code.replace(pos, end - pos + 1, includeCode);
                m_Includes.insert(includePath);

                includesProcessing.erase(includePath);
            }
        }
        pos = code.find(include, pos + 1);
    }
}
} // namespace Ivy::G
