#pragma once

#include "Core/Services/ResourceManager/Resource.hpp"
#include "CullMode.hpp"
#include "FrontFace.hpp"
#include "Modules/Graphics/Buffers/ArrayBuffer.hpp"
#include "Modules/Graphics/Buffers/IndexBuffer.hpp"
#include "Modules/Graphics/Buffers/VertexArray.hpp"
#include "RenderMode.hpp"
#include "Topology.hpp"
#include "Vertex.hpp"

namespace N::G
{
/**
 * @brief Represents a renderable mesh resource.
 *
 * Stores vertex and index data and manages the OpenGL resources required
 * to render the mesh. GPU resources are generated lazily when Generate()
 * is called.
 */
struct Mesh : C::Resource
{
    VertexArray VAO;
    ArrayBuffer VBO;
    IndexBuffer EBO;
    /** Rendering mode used when drawing the mesh. */
    RenderMode RenderMode = RenderMode::Solid;

    /** Primitive topology used to interpret the mesh indices. */
    Topology Topology = Topology::Triangles;

    /**
     * @brief Face culling mode used when drawing the mesh.
     * Determines which faces are discarded during rasterization.
     */
    CullMode CullMode = CullMode::Back;

    /**
     * @brief Winding order considered front-facing.
     * Determines how OpenGL identifies front-facing and back-facing faces.
     */
    FrontFace FrontFace = FrontFace::CounterClockwise;

    /** CPU-side vertex data used to generate the GPU resources. */
    std::vector<Vertex> Vertices;

    /** CPU-side index data used to generate the GPU resources. */
    std::vector<unsigned int> Indices;

    /**
     * @brief Creates a mesh resource.
     * @param name Resource name.
     */
    Mesh(const std::string& name);

    /** Releases the OpenGL resources owned by the mesh. */
    ~Mesh() override;

    /**
     * @brief Gets the OpenGL vertex array object ID.
     * @return OpenGL VAO ID, or 0 if the mesh has not been generated.
     */
    [[nodiscard]] unsigned int GetId() const;

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
    void Regenerate();

  private:
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
} // namespace N::G
