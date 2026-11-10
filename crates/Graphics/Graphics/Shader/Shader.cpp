#include "Shader.hpp"

#include "Graphics/OpenGL.hpp"
#include <concepts>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "Graphics/Shader/ShaderSource.hpp"
#include "Math/Matrix/Matrices.hpp"
#include "Math/Vector/Vectors.hpp"
#include "Utilities/CheckedPtr.hpp"
#include "Diagnostics/Log.hpp"

namespace Ivy::G
{
unsigned int Shader::GetId() const
{
    return m_Id;
}

const std::string& Shader::GetName() const
{
    return m_Name;
}

void Shader::Use()
{
    CreateProgram();
    glUseProgram(m_Id);
    UploadUniforms();
}

void Shader::SetUniform(std::string_view name, const Uniform& uniform)
{
    m_PendingUniforms[name] = UniformPair{.Uniform = uniform, .Location = GetUniformLocation(name)};
}

bool Shader::IsProgramCreated() const
{
    return m_Id != 0;
}

void Shader::AssignSource(ShaderSource& source)
{
    for (auto& existing : m_Sources)
    {
        if (existing->GetStage() == source.GetStage())
        {
            return;
        }
    }

    m_Sources.emplace_back(&source);
}

void Shader::Delete()
{
    if (IsProgramCreated())
    {
        for (auto& source : m_Sources)
        {
            source->Delete();
        }

        for (UniformPair& uniPair : m_PendingUniforms)
        {
            uniPair.Location = -1;
        }

        glDeleteProgram(m_Id);
        m_Id = 0;
    }
}

std::vector<U::CheckedPtr<ShaderSource>> Shader::GetSources()
{
    return m_Sources;
}

void Shader::CreateProgram()
{
    if (IsProgramCreated())
    {
        return;
    }

    //TODO: add preprocessing to shader program, ex: a ShowNormals option that when true,
    // automatically adds a geometry shader
    //  for that.
    //  gotta figure out how to "Merge" multiple sources too, like if there is 2 vertex
    //  shaders attached both with void main(). Preprocess();

    if (m_Sources.empty())
    {
        D::Log::Warning("Shader Program:", GetId(), " Has No Sources");
        return;
    }

    m_Id = glCreateProgram();

    for (const auto& source : m_Sources)
    {
        if (!source->IsCompiled())
        {
            source->Compile();
        }
        glAttachShader(m_Id, source->GetId());
    }
    glLinkProgram(m_Id);

    int success;
    char infoLog[512];
    glGetProgramiv(m_Id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_Id, 512, nullptr, infoLog);
        D::Log::Error(std::string("Shader Program: " + GetName() + " Linking Failed: " + infoLog));
        Delete();
    }
}

int Shader::GetUniformLocation(std::string_view name)
{
    int location;

    if (m_PendingUniforms.Contains(name) && m_PendingUniforms[name].Location > 0)
    {
        location = m_PendingUniforms[name].Location;
    }
    else
    {
        location = glGetUniformLocation(m_Id, name.data());
        if (location == -1)
        {
        }
    }
    return location;
}

void Shader::UploadUniforms()
{
    for (UniformPair& uniPair : m_PendingUniforms)
    {
        UploadUniform(uniPair.Uniform, uniPair.Location);
    }
}

void Shader::UploadUniform(const Uniform& uniform, const int location)
{
    uniform.visit(
        [location](const auto& value)
        {
            using T = std::remove_cvref_t<decltype(value)>;

            if constexpr (std::same_as<T, int> || std::same_as<T, bool>)
            {
                glUniform1i(location, value);
            }
            else if constexpr (std::same_as<T, float>)
            {
                glUniform1f(location, value);
            }
            else if constexpr (std::same_as<T, Ivy::M::Vector<2, float>>)
            {
                glUniform2fv(location, 1, &value.x);
            }
            else if constexpr (std::same_as<T, M::Vector<3, float>>)
            {
                glUniform3fv(location, 1, &value.x);
            }
            else if constexpr (std::same_as<T, M::Vector<4, float>>)
            {
                glUniform4fv(location, 1, &value.x);
            }
            else if constexpr (std::same_as<T, M::Matrix<3, 3, float>>)
            {
                glUniformMatrix3fv(location, 1, GL_FALSE, &value.Data()[0][0]);
            }
            else if constexpr (std::same_as<T, Ivy::M::Matrix<4, 4, float>>)
            {
                glUniformMatrix4fv(location, 1, GL_FALSE, &value.Data()[0][0]);
            }
        });
}
} // namespace Ivy::G
