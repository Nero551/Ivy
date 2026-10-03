#pragma once

#include "Core/Services/ResourceManager/Resource.hpp"
#include "ShaderSource.hpp"
#include "Uniforms/Uniform.hpp"
#include "Utilities/CheckedPtr.hpp"

namespace Ivy::G
{
/**
 * @brief Represents an OpenGL shader program.
 * manages the lifetime and use of an OpenGL shader program,
 * including its source files and pending uniform values.
 */
struct Shader
{
    /**
     * @brief Whether the shader supports hot reloading.
     * When enabled, the shader can be reloaded when its source changes.
     */
    bool HotReload = false;

    /**
     * @brief Creates a shader resource.
     * @param name Name used to identify the shader resource.
     */
    explicit Shader(const std::string& name) : m_Name(name) {}

    /** @brief Destroys the shader and releases its OpenGL program. */
    ~Shader();

    /**
     * @brief Gets the OpenGL shader program ID.
     * @return OpenGL program ID.
     */
    unsigned int GetId() const;

    /** @brief Gets the resource name. */
    const std::string& GetName() const;

    /** @brief Activates the shader and uploads pending uniforms. */
    void Use();

    /** @brief Checks if the shader program exists.
     * @return  true if exists , false if not.
     */
    bool IsProgramCreated() const;

    /**
     * @brief Queues a uniform value for upload.
     *
     * @tparam T Uniform type derived from @ref Uniform.
     * @param uniform Uniform value to queue.
     *
     * @note The uniform is uploaded the next time the shader is used.
     */
    template <typename T> void SetUniform(const T& uniform) requires(std::derived_from<T, Uniform>)
    {
        auto it = m_PendingUniforms.find(uniform.Name);

        if (it == m_PendingUniforms.end())
        {
            m_PendingUniforms.emplace(uniform.Name, std::make_unique<T>(uniform));
        }
        else
        {
            *static_cast<T*>(it->second.get()) = uniform;
        }
    }

    void AssignSource(ShaderSource& source);

    /** @brief deletes the current shader program and uniform locations , which
     * causes them to be recreated on Use() */
    void Reload();

    std::vector<U::CheckedPtr<ShaderSource>>& GetSources();

  private:
    std::string m_Name;

    /** @brief OpenGL handle for the shader program. */
    unsigned int m_Id = 0;

    std::unordered_map<std::string, int> m_UniformLocations;

    /**
     * @brief Uniform values waiting to be uploaded to the GPU.
     * Uniforms are stored by name and resolved to OpenGL locations
     * when they are uploaded.
     */
    std::unordered_map<std::string, std::unique_ptr<Uniform>> m_PendingUniforms;

    std::vector<U::CheckedPtr<ShaderSource>> m_Sources;

    void CreateProgram();

    /**
     * @brief Gets and caches the OpenGL location of a uniform.
     * @param name Name of the shader uniform.
     * @return OpenGL uniform location.
     */
    int GetUniformLocation(const std::string& name);

    void UploadUniforms();
};
} // namespace Ivy::G