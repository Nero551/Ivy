#pragma once
#include <OpenGL.hpp>

#include "Utilities/Image.hpp"

namespace N::C
{
/**
 * @brief Wrapper around a GLFW window.
 * Owns the underlying GLFW window and provides basic window
 * management and event handling.
 */
struct Window
{
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

  private:
    GLFWwindow* m_GlfwWindow;

    void SetHints();
};
} // namespace N::C
