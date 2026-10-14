#include "Shader.hpp"

#include "Graphics/OpenGL.hpp"
#include <concepts>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "../../Utilities/Debug/Log.hpp"
#include "Graphics/Shader/ShaderSource.hpp"
#include "Math/Matrix/Matrix3.hpp"
#include "Math/Matrix/Matrix4.hpp"
#include "Math/Vector/Vector2.hpp"
#include "Math/Vector/Vector3.hpp"
#include "Math/Vector/Vector4.hpp"
#include "Utilities/CheckedPtr.hpp"
#include "Utilities/Debug/Inspector.hpp"

namespace Ivy::G
{

Shader::~Shader()
{
    glDeleteProgram(m_Id);
}

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
            // Ivy::U::Logger::Error("Shader: " + Name + " Duplicate Shader Stage.");
            return;
        }
    }

    m_Sources.emplace_back(&source);
}

void Shader::Reload()
{
    for (auto& source : m_Sources)
    {
        source->Reload();
    }

    for (UniformPair& uniPair : m_PendingUniforms)
    {
        uniPair.Location = -1;
    }

    glDeleteProgram(m_Id);
    m_Id = 0;
}

std::vector<U::CheckedPtr<ShaderSource>>& Shader::GetSources()
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
        U::Log::Warning("Shader Program:", GetId(), " Has No Sources");
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
        U::Log::Error(std::string("Shader Program: " + GetName() + " Linking Failed: " + infoLog));
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
            // U::Logger::Warning("Shader: " + Name + " Uniform Not Found: " + name);
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
            else if constexpr (std::same_as<T, Ivy::M::Vector<2>>)
            {
                glUniform2fv(location, 1, &value.x);
            }
            else if constexpr (std::same_as<T, M::Vector<3>>)
            {
                glUniform3fv(location, 1, &value.x);
            }
            else if constexpr (std::same_as<T, M::Vector<4>>)
            {
                glUniform4fv(location, 1, &value.x);
            }
            else if constexpr (std::same_as<T, M::Matrix<3, 3>>)
            {
                glUniformMatrix3fv(location, 1, GL_FALSE, &value.Data()[0][0]);
            }
            else if constexpr (std::same_as<T, Ivy::M::Matrix<4, 4>>)
            {
                glUniformMatrix4fv(location, 1, GL_FALSE, &value.Data()[0][0]);
            }
            else
            {
                static_assert(U::Inspector::TInspect<T>{}, "Unsupported uniform type");
            }
        });
}
} // namespace Ivy::G
