#pragma once

#include "../Buffers/ArrayBuffer/ArrayBuffer.hpp"
#include "../Buffers/IndexBuffer/IndexBuffer.hpp"
#include "../Buffers/VertexArray/VertexArray.hpp"
#include "CullMode.hpp"
#include "FrontFace.hpp"
#include "RenderMode.hpp"
#include "Topology.hpp"
#include "Vertex.hpp"
#include <string>
#include <utility>
#include <vector>

namespace Ivy::G
{
/**
 * @brief Represents a renderable mesh resource.
 *
 * Stores vertex and index data and manages the OpenGL resources required
 * to render the mesh. GPU resources are generated lazily when Generate()
 * is called.
 */
struct Mesh
{
    VertexArray VAO;
    ArrayBuffer VBO;
    IndexBuffer EBO;

    RenderMode RenderMode = RenderMode::Solid;
    Topology Topology = Topology::Triangles;
    CullMode CullMode = CullMode::Back;
    FrontFace FrontFace = FrontFace::CounterClockwise;

    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;

    Mesh(std::string name) : m_Name(std::move(name)) {}
    ~Mesh() = default;
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&&) noexcept = default;
    Mesh& operator=(Mesh&&) noexcept = default;

    /**
     * @brief Gets the OpenGL vertex array object ID.
     * @return OpenGL VAO ID, or 0 if the mesh has not been generated.
     */
    unsigned int GetId() const;

    /** @brief Gets the resource name. */
    const std::string& GetName() const;

    /** @return Whether the mesh's OpenGL resources have been generated. */
    bool IsGenerated() const;

    /**
     * @brief Generates the OpenGL resources required to render the mesh.
     * Generates the VAO, VBO, EBO, and vertex attribute configuration from
     * the mesh's stored vertex and index data.
     */
    void Generate();

    /**
     * @brief Draws the mesh using its configured rendering state.
     *
     * Applies the culling mode, front-face winding, render mode, and
     * topology before issuing the draw call. Generates the OpenGL
     * resources first if they have not yet been created.
     */
    void Draw();

    /**
     * @brief Draws the mesh using its configured rendering state using Instanced
     * rendering. Applies the culling mode, front-face winding, render mode, and topology
     * before issuing the draw call. Generates the OpenGL resources first if they have not
     * yet been created.
     * @param instanceCount number of meshes to render.
     */
    void DrawInstanced(int instanceCount);

    /**
     * @brief Regenerates the mesh's OpenGL resources.
     *
     * Deletes the existing GPU resources and marks the mesh as not
     * generated so they can be recreated by Generate().
     */
    void Delete();

  private:
    std::string m_Name;

    /** @brief Issues the OpenGL draw call using mesh's index & vertex data.
     * Does NOT generate the VAO, VBO or EBO. nor binds the VAO.
     */
    void DrawElements() const;

    /**
     * @brief Applies the mesh's face culling configuration.
     *
     * Configures the front-face winding and enables or disables face
     * culling according to the mesh's CullMode and FrontFace settings.
     */
    void ApplyCulling() const;
};
} // namespace Ivy::G
