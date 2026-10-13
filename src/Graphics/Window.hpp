#pragma once
#include "GLFW/glfw3.h"
#include "GLFWPlatform.hpp"
#include "Graphics/OpenGLProfile.hpp"

#include <OpenGL.hpp>
#include <concepts>
#include <functional>
#include <string>
#include <type_traits>

#include "Utilities/Image.hpp"

namespace Ivy::G
{

/**
 * @brief Wrapper around a GLFW window.
 * Owns the underlying GLFW window and provides window
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
        bool Visibility = true;
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
    float GetAspectRatio() const;

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

    GLFWPlatform GetPlatform() const;

    template <typename F>
    void SetScrollCallback(F&& f)
        requires(std::same_as<std::invoke_result_t<F, Window&, double, double>, void>)
    {
        m_ScrollCallback = std::forward<F>(f);
        glfwSetScrollCallback(m_GlfwWindow,
            [](GLFWwindow* w, const double x, const double y)
            {
                Window* win = static_cast<Window*>(glfwGetWindowUserPointer(w));
                if (win == nullptr || !win->m_ScrollCallback)
                {
                    return;
                }
                win->m_ScrollCallback(*win, x, y);
            });
    }
    template <typename F>
    void SetCursorPosCallback(F&& f)
        requires(std::same_as<std::invoke_result_t<F, Window&, double, double>, void>)
    {
        m_CursorPosCallback = std::forward<F>(f);
        glfwSetCursorPosCallback(m_GlfwWindow,
            [](GLFWwindow* w, const double x, const double y)
            {
                Window* win = static_cast<Window*>(glfwGetWindowUserPointer(w));
                if (win == nullptr || !win->m_CursorPosCallback)
                {
                    return;
                }
                win->m_CursorPosCallback(*win, x, y);
            });
    }
    template <typename F>
    void SetFramebufferSizeCallback(F&& f)
        requires(std::same_as<std::invoke_result_t<F, Window&, int, int>, void>)
    {
        m_FramebufferSizeCallback = std::forward<F>(f);
        glfwSetFramebufferSizeCallback(m_GlfwWindow,
            [](GLFWwindow* w, const int x, const int y)
            {
                Window* win = static_cast<Window*>(glfwGetWindowUserPointer(w));
                if (win == nullptr || !win->m_FramebufferSizeCallback)
                {
                    return;
                }
                win->m_FramebufferSizeCallback(*win, x, y);
            });
    }

  private:
    GLFWwindow* m_GlfwWindow;
    GLFWPlatform m_Platform;

    std::function<void(Window&, double, double)> m_ScrollCallback;
    std::function<void(Window&, double, double)> m_CursorPosCallback;
    std::function<void(Window&, int, int)> m_FramebufferSizeCallback;

    void SetConfiguration();
};
} // namespace Ivy::G
