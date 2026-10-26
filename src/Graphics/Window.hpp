#pragma once
#include "GLFW/glfw3.h"
#include "GLFWPlatform.hpp"
#include "Graphics/OpenGLProfile.hpp"
#include "InputEnums/Action.hpp"
#include "InputEnums/CursorMode.hpp"
#include "InputEnums/Key.hpp"
#include "InputEnums/MouseButton.hpp"
#include "OpenGL.hpp"
#include <concepts>
#include <functional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include "Utilities/Image.hpp"

namespace Ivy::G
{

/**
 * @brief Wrapper around a GLFW window.
 * Owns the underlying GLFW window and provides window
 * management and input/event handling.
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
        std::string X11ClassName = "X11";
        std::string WaylandAppId = "Wayland";
        bool DoubleBuffer = true;
        int DepthBits = 24;
        int StencilBits = 8;
        int Samples = 4;
        bool Visibility = true;
    };

    Configuration Config;

    Window() = default;
    ~Window()
    {
        Delete();
    }
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    Window(Window&& other) noexcept
        : Config(std::move(other.Config)), m_GlfwWindow(std::exchange(other.m_GlfwWindow, nullptr)),
          m_Platform(other.m_Platform), m_ScrollCallback(std::move(other.m_ScrollCallback)),
          m_CursorPosCallback(std::move(other.m_CursorPosCallback)),
          m_FramebufferSizeCallback(std::move(other.m_FramebufferSizeCallback)),
          m_KeyCallback(std::move(other.m_KeyCallback)),
          m_MouseButtonCallback(std::move(other.m_MouseButtonCallback)),
          m_CursorEnteredCallback(std::move(other.m_CursorEnteredCallback))
    {
        if (m_GlfwWindow != nullptr)
        {
            glfwSetWindowUserPointer(m_GlfwWindow, this);
        }
    }
    Window& operator=(Window&& other) noexcept
    {
        if (this != &other)
        {
            Delete();

            Config = std::move(other.Config);

            m_GlfwWindow = std::exchange(other.m_GlfwWindow, nullptr);
            m_Platform = other.m_Platform;

            m_KeyCallback = std::move(other.m_KeyCallback);
            m_ScrollCallback = std::move(other.m_ScrollCallback);
            m_CursorPosCallback = std::move(other.m_CursorPosCallback);
            m_MouseButtonCallback = std::move(other.m_MouseButtonCallback);
            m_CursorEnteredCallback = std::move(other.m_CursorEnteredCallback);
            m_FramebufferSizeCallback = std::move(other.m_FramebufferSizeCallback);

            if (m_GlfwWindow != nullptr)
            {
                glfwSetWindowUserPointer(m_GlfwWindow, this);
            }
        }

        return *this;
    }

    /**
    * @brief Generates a window.
    * @param width Window width in pixels.
    * @param height Window height in pixels.
    * @param title Window title.
    */
    void Generate(int width, int height, std::string_view title);
    void Delete();

    bool IsGenerated() const;

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

    template <typename F>
    void SetKeyCallback(F&& f)
        requires(std::same_as<std::invoke_result_t<F, Window&, Key, int, Action, int>, void>)
    {
        m_KeyCallback = std::forward<F>(f);
        glfwSetKeyCallback(m_GlfwWindow,
            [](GLFWwindow* w, int key, int scancode, int action, int mods)
            {
                Window* win = static_cast<Window*>(glfwGetWindowUserPointer(w));
                if (win == nullptr || !win->m_KeyCallback)
                {
                    return;
                }
                win->m_KeyCallback(*win, static_cast<Key>(key), scancode, static_cast<Action>(action), mods);
            });
    }

    template <typename F>
    void SetMouseButtonCallback(F&& f)
        requires(std::same_as<std::invoke_result_t<F, Window&, MouseButton, Action, int>, void>)
    {
        m_MouseButtonCallback = std::forward<F>(f);
        glfwSetMouseButtonCallback(m_GlfwWindow,
            [](GLFWwindow* w, int button, int action, int mods)
            {
                Window* win = static_cast<Window*>(glfwGetWindowUserPointer(w));
                if (win == nullptr || !win->m_MouseButtonCallback)
                {
                    return;
                }
                win->m_MouseButtonCallback(
                    *win, static_cast<MouseButton>(button), static_cast<Action>(action), mods);
            });
    };

    template <typename F>
    void SetCursorEnterCallback(F&& f) requires(std::same_as<std::invoke_result_t<F, Window&, bool>, void>)
    {
        m_CursorEnteredCallback = std::forward<F>(f);
        glfwSetCursorEnterCallback(m_GlfwWindow,
            [](GLFWwindow* w, bool entered)
            {
                Window* win = static_cast<Window*>(glfwGetWindowUserPointer(w));
                if (win == nullptr || !win->m_CursorEnteredCallback)
                {
                    return;
                }
                win->m_CursorEnteredCallback(*win, entered);
            });
    }

    void SetCursorMode(CursorMode mode);
    void SetCursor(const U::Image& icon);
    void SetDefaultCursor();

  private:
    GLFWwindow* m_GlfwWindow = nullptr;
    GLFWPlatform m_Platform = GLFWPlatform::Null;

    std::function<void(Window&, double, double)> m_ScrollCallback;
    std::function<void(Window&, double, double)> m_CursorPosCallback;
    std::function<void(Window&, int, int)> m_FramebufferSizeCallback;
    std::function<void(Window&, Key, int, Action, int)> m_KeyCallback;
    std::function<void(Window&, MouseButton, Action, int)> m_MouseButtonCallback;

    std::function<void(Window&, bool)> m_CursorEnteredCallback;

    void SetConfiguration();
};
} // namespace Ivy::G
