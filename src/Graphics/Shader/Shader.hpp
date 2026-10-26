#pragma once

#include "Math/Matrix/Matrix3.hpp"
#include "Math/Matrix/Matrix4.hpp"
#include "Math/Vector/Vector2.hpp"
#include "Math/Vector/Vector3.hpp"
#include "Math/Vector/Vector4.hpp"
#include "ShaderSource.hpp"
#include "Utilities/CheckedPtr.hpp"
#include "Utilities/DataStructures/StringMap.hpp"
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

namespace Ivy::G
{
/**
 * @brief Represents an OpenGL shader program resource.
 *
 * Manages shader sources, program creation and reloading, and uniform values.
 * Uniform values are stored by name and uploaded when the program is activated.
 */
struct Shader
{
    /**
     * @brief Enables automatic reloading when shader source changes.
     *
     * @note The reload mechanism must be triggered externally.
     */
    bool HotReload = false;

    /**
     * @brief Creates a shader resource.
     * @param name Name used to identify the shader resource.
     */
    Shader(std::string name) : m_Name(std::move(name)) {}
    Shader(std::string name, ShaderSource& vert, ShaderSource& frag) : m_Name(std::move(name))
    {
        AssignSource(vert);
        AssignSource(frag);
    }
    ~Shader()
    {
        Delete();
    }
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&& other) noexcept : m_Name(std::move(other.m_Name)), m_Id(std::exchange(other.m_Id, 0)) {}
    Shader& operator=(Shader&& other) noexcept
    {
        if (this != &other)
        {
            Delete();

            m_Name = std::move(other.m_Name);
            m_Id = std::exchange(other.m_Id, 0);
        }

        return *this;
    }

    /**
     * @brief Gets the OpenGL program handle.
     * @return The program ID, or zero if no program has been created.
     */
    unsigned int GetId() const;

    /**
     * @brief Gets the shader resource name.
     * @return The resource name.
     */
    const std::string& GetName() const;

    /**
     * @brief Activates the program and uploads its pending uniform values.
     *
     * Creates and links the program on first use if necessary.
     */
    void Use();

    /**
     * @brief Checks whether an OpenGL program has been created.
     * @return True if the program handle is nonzero.
     */
    bool IsProgramCreated() const;

    /**
     * @brief Represents a value supported by the uniform upload mechanism.
     * Supports Boolean, integer, and floating-point scalars; 2D, 3D, and 4D
     * vectors; and 3x3 and 4x4 matrices.
     */
    using Uniform = std::variant<bool, int, float, M::Vector<2>, M::Vector<3>, M::Vector<4>, M::Matrix<3, 3>,
        M::Matrix<4, 4>>;

    /**
     * @brief Stores a uniform value and its resolved OpenGL location.
     */
    struct UniformPair
    {
        /** @brief Value to upload to the shader program. */
        Uniform Uniform;

        /**
         * @brief OpenGL location of the uniform in the linked program.
         *
         * A value less than 0 indicates that the location is invalid or has not
         * been resolved successfully.
         */
        int Location;
    };

    /**
     * @brief Sets or replaces a pending uniform value.
     *
     * Resolves the uniform location and stores the value for upload by Use().
     *
     * @param name Name of the uniform in the shader.
     * @param uniform Value to upload.
     *
     * @note The uniform name must be null-terminated when passed to OpenGL.
     */
    void SetUniform(std::string_view name, const Uniform& uniform)
    {
        m_PendingUniforms[name] = UniformPair{.Uniform = uniform, .Location = GetUniformLocation(name)};
    }

    /**
     * @brief Associates a shader source with this program.
     * Sources with a stage already assigned to this program are ignored.
     * @param source Shader source to associate.
     */
    void AssignSource(ShaderSource& source);

    /**
     * @brief Reloads the shader sources and invalidates the current program.
     *
     * Deletes the linked program and invalidates cached uniform locations.
     * The program is recreated on the next call to Use().
     */
    void Delete();

    /**
     * @brief Gets the shader sources associated with this program.
     * @return Mutable references to the checked source pointers.
     */
    std::vector<U::CheckedPtr<ShaderSource>>& GetSources();

  private:
    /** @brief Name used to identify this shader resource. */
    std::string m_Name;

    /** @brief OpenGL handle for the linked shader program. */
    unsigned int m_Id = 0;

    /**
     * @brief Uniform values and their OpenGL locations, indexed by uniform name.
     * Locations are cached alongside values and invalidated when the program is reloaded.
     */
    U::StringMap<UniformPair> m_PendingUniforms;

    /** @brief Shader sources attached to this program. */
    std::vector<U::CheckedPtr<ShaderSource>> m_Sources;

    /**
     * @brief Creates and links the OpenGL program from the assigned sources.
     * Does nothing if a program already exists or no sources are assigned.
     */
    void CreateProgram();

    /**
     * @brief Resolves the OpenGL location of a uniform.
     * @param name Name of the uniform in the shader.
     * @return The uniform location, or -1 if it cannot be found.
     */
    int GetUniformLocation(std::string_view name);

    /** @brief Uploads all stored uniform values to the active program. */
    void UploadUniforms();

    /**
     * @brief Uploads a uniform value using its corresponding OpenGL function.
     * @param uniform Value to upload.
     * @param location OpenGL location of the uniform.
     */
    void UploadUniform(const Uniform& uniform, int location);
};
} // namespace Ivy::G
