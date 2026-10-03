#include "Shader.hpp"

#include <OpenGL.hpp>

#include "Uniforms/FloatUniform.hpp"
#include "Utilities/Log.hpp"

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

    m_UniformLocations.clear();
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

    // TODO- add preprocessing to shader program, ex: a ShowNormals option that when true,
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

int Shader::GetUniformLocation(const std::string& name)
{
    int location;

    if (m_UniformLocations.contains(name))
    {
        location = m_UniformLocations[name];
    }
    else
    {
        location = glGetUniformLocation(m_Id, name.data());

        if (location == -1)
        {
            // U::Logger::Warning("Shader: " + Name + " Uniform Not Found: " + name);
            return -1;
        }

        m_UniformLocations[name] = location;
    }
    return location;
}

void Shader::UploadUniforms()
{
    for (auto& [uniformName, uniform] : m_PendingUniforms)
    {
        int location = GetUniformLocation(uniformName);
        if (location != -1)
        {
            uniform->Upload(location);
        }
    }
}
} // namespace Ivy::G