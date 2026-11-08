#pragma once

/**
 * @brief Determines how geometry is rasterized.
 */
enum class RenderMode
{
    /** Renders filled polygons. */
    Solid,

    /** Renders only the edges of polygons. */
    Wireframe,

    /** Renders both filled polygons and their edges. */
    SolidWireframe
};
