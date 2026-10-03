#pragma once

namespace N::G
{

/**
 * @brief Defines how vertices are assembled into primitives for rendering.
 */
enum class Topology
{
    /** Renders each vertex as an individual point. */
    Points = GL_POINTS,

    /** Renders each pair of vertices as an independent line. */
    Lines = GL_LINES,

    /** Renders every group of three vertices as an independent triangle. */
    Triangles = GL_TRIANGLES,

    /** Renders connected triangles, where each new vertex forms a triangle with the previous two
     * vertices. */
    TriangleStrip = GL_TRIANGLE_STRIP,

    /** Renders connected line segments, where each new vertex extends the previous line. */
    LineStrip = GL_LINE_STRIP,
};

} // namespace N::G
