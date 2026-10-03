#include "Mesh.hpp"

#include <OpenGL.hpp>

namespace Ivy::G
{

Mesh::~Mesh() {}

const std::string& Mesh::GetName() const
{
    return m_Name;
}

unsigned int Mesh::GetId() const
{
    return VAO.GetId();
}

bool Mesh::IsGenerated() const
{
    return VAO.GetId() != 0;
}

void Mesh::Generate()
{
    if (IsGenerated())
    {
        return;
    }
    VBO.SetData(Vertices);
    EBO.SetData(Indices);

    VAO.SetIndexBuffer(EBO);
    VAO.SetVertexBuffer(VBO, 0, sizeof(Vertex), 0);

    VAO.SetAttribPointer(0, 4, DataType::Float, 0, offsetof(Vertex, Position));
    VAO.SetAttribPointer(1, 4, DataType::Float, 0, offsetof(Vertex, Color));
    VAO.SetAttribPointer(2, 2, DataType::Float, 0, offsetof(Vertex, UV));
    VAO.SetAttribPointer(3, 3, DataType::Float, 0, offsetof(Vertex, Normal));
}

void Mesh::Draw()
{
    Generate();

    ApplyCulling();
    VAO.Bind();
    if (RenderMode == RenderMode::Solid)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        DrawElements();
    }

    else if (RenderMode == RenderMode::Wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        DrawElements();
    }

    else if (RenderMode == RenderMode::SolidWireframe)
    {
        // TODO- apparently there is a better way to do this using geometry shaders
        // (search SolidWireframe opengl on yt).
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        DrawElements();

        glDepthFunc(GL_ALWAYS);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        DrawElements();

        glDepthFunc(GL_LESS);
    }
    VAO.Unbind();
}

void Mesh::DrawInstanced(const int instanceCount)
{
    Generate();

    ApplyCulling();
    VAO.Bind();
    glDrawElementsInstanced(static_cast<GLenum>(Topology), static_cast<GLsizei>(Indices.size()),
        GL_UNSIGNED_INT, nullptr, static_cast<GLsizei>(instanceCount));

    VAO.Unbind();
}

void Mesh::Regenerate()
{
    VBO.Delete();
    EBO.Delete();
    VAO.Delete();
}

void Mesh::DrawElements() const
{
    glDrawElements(
        static_cast<GLenum>(Topology), static_cast<GLsizei>(Indices.size()), GL_UNSIGNED_INT, nullptr);
}

void Mesh::ApplyCulling() const
{
    glFrontFace(static_cast<GLenum>(FrontFace));

    if (CullMode == CullMode::None)
    {
        glDisable(GL_CULL_FACE);
    }
    else
    {
        if (!glIsEnabled(GL_CULL_FACE))
        {
            glEnable(GL_CULL_FACE);
        }
        glCullFace(static_cast<GLenum>(CullMode));
    }
}
} // namespace Ivy::G
