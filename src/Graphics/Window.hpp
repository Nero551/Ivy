#pragma once
#include "GLFWPlatform.hpp"
#include "Graphics/OpenGLProfile.hpp"

#include <OpenGL.hpp>

#include "Utilities/Image.hpp"

namespace Ivy::G
{

/**
 * @brief Wrapper around a GLFW window.
 * Owns the underlying GLFW window and provides basic window
 * management and event handling.
 */
struct Window
{
    struct Configuration
    {
        int Major = 4;
        int Minor = 6;
        OpenGLProfile Profile = OpenGLProfile::Core;
        bool Debug = true;
        bool ForwardCompatibility = true;
        std::string X11ClassName = "ivy_engine";
        std::string WaylandAppId = "ivy_engine";
        bool DoubleBuffer = true;
        int DepthBits = 24;
        int StencilBits = 8;
        int Samples = 4;
    };

    Configuration Config;

    Window() = default;
    ~Window();

    /**
    * @brief Generates a window.
    * @param width Window width in pixels.
    * @param height Window height in pixels.
    * @param title Window title.
    */
    void Generate(int width, int height, const std::string& title);
    void Terminate();

    bool IsTerminated() const;

    /** @brief Returns the window's width-to-height ratio. */
    [[nodiscard]] float GetAspectRatio() const;

    void MakeCurrentContext();

    /**
     * @brief Checks whether the window has been requested to close.
     * @return True if the window should close.
     */
    bool ShouldClose() const;

    /** Swaps the front and back buffers. */
    void SwapBuffers();

    /** Processes pending GLFW events. */
    void PollEvents();

    void SetTitle(const std::string& title);

    void SetIcon(const U::Image& icon);

    /** @param height New height in pixels */
    void SetHeight(int height);

    /** @param width New width in pixels */
    void SetWidth(int width);

    /**
     * @brief Changes the window dimensions.
     * @param width New width in pixels.
     * @param height New height in pixels.
     */
    void SetSize(int width, int height);

    /** @brief Returns height in pixels */
    int GetHeight() const;

    /** @brief Returns width in pixels */
    int GetWidth() const;

    void Close();

    GLFWwindow* GetGlfwWindow() const;

    GLFWPlatform GetPlatform() const
    {
        return m_Platform;
    }

  private:
    GLFWwindow* m_GlfwWindow;
    GLFWPlatform m_Platform;

    void SetConfiguration();
};
} // namespace Ivy::G
