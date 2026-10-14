#pragma once

#include "Core/Services/ResourceManager/Resource.hpp"
#include "Math/Matrix/Matrix.hpp"
#include "Math/Matrix/Matrix3.hpp"
#include "Math/Matrix/Matrix4.hpp"
#include "Math/Vector/Vector.hpp"
#include "Math/Vector/Vector2.hpp"
#include "Math/Vector/Vector3.hpp"
#include "Math/Vector/Vector4.hpp"
#include "ShaderSource.hpp"
#include "Utilities/CheckedPtr.hpp"
#include <string>
#include <variant>

namespace Ivy::G
{
/**
 * @brief Represents an OpenGL shader program.
 * manages the lifetime and use of an OpenGL shader program,
 * including its source files and pending uniform values.
 */
struct Shader
{
    using Uniform = std::variant<bool, int, float, M::Vector<2>, M::Vector<3>, M::Vector<4>, M::Matrix<3, 3>,
        M::Matrix<4, 4>>;
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
    void SetUniform(const std::string& name, const Uniform& uniform)
    {
        m_PendingUniforms.insert_or_assign(name, uniform);
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
    std::unordered_map<std::string, Uniform> m_PendingUniforms;

    std::vector<U::CheckedPtr<ShaderSource>> m_Sources;

    void CreateProgram();

    /**
     * @brief Gets and caches the OpenGL location of a uniform.
     * @param name Name of the shader uniform.
     * @return OpenGL uniform location.
     */
    int GetUniformLocation(const std::string& name);

    void UploadUniforms();

    void UploadUniform(const Uniform& uniform, const int location);
};
} // namespace Ivy::G
