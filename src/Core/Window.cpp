#include "Window.hpp"

#include "Engine.hpp"
#include "Utilities/Log.hpp"

namespace N::C
{
Window::~Window()
{
    Terminate();
}
void Window::Generate(const int width, const int height, const std::string& title)
{
    SetHints();

    GLFWwindow* glfwWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    m_GlfwWindow = glfwWindow;
    if (!glfwWindow)
    {
        U::Log::Fatal("Failed To Create Window");
    }

    MakeCurrentContext();

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        U::Log::Fatal("Failed To Initialize GLAD");
    }

    U::Log::Info(glGetString(GL_VERSION));

    glViewport(0, 0, width, height);
}
void Window::Terminate()
{
    if (!IsTerminated())
    {
        glfwDestroyWindow(m_GlfwWindow);
        m_GlfwWindow = nullptr;
    }
}
bool Window::IsTerminated() const
{
    return m_GlfwWindow == nullptr;
}

float Window::GetAspectRatio() const
{
    return static_cast<float>(GetWidth()) / static_cast<float>(GetHeight());
}
void Window::MakeCurrentContext()
{
    glfwMakeContextCurrent(m_GlfwWindow);
}

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(m_GlfwWindow);
}

void Window::SwapBuffers()
{
    glfwSwapBuffers(m_GlfwWindow);
}

void Window::PollEvents()
{
    glfwPollEvents();
}

void Window::SetTitle(const std::string& title)
{
    glfwSetWindowTitle(m_GlfwWindow, title.c_str());
}

void Window::SetIcon(const U::Image& icon)
{
    GLFWimage image;
    image.height = icon.Height;
    image.width = icon.Width;
    image.pixels = const_cast<unsigned char*>(icon.Pixels.data());
    glfwSetWindowIcon(m_GlfwWindow, 1, &image);
}

void Window::SetHeight(const int height)
{
    glfwSetWindowSize(m_GlfwWindow, GetWidth(), height);
}

void Window::SetWidth(const int width)
{
    glfwSetWindowSize(m_GlfwWindow, width, GetHeight());
}

void Window::SetSize(const int width, const int height)
{
    glfwSetWindowSize(m_GlfwWindow, width, height);
}

int Window::GetHeight() const
{
    int height = 0;
    int width = 0;
    glfwGetWindowSize(m_GlfwWindow, &width, &height);
    return height;
}

int Window::GetWidth() const
{
    int height = 0;
    int width = 0;
    glfwGetWindowSize(m_GlfwWindow, &width, &height);
    return width;
}

void Window::Close()
{
    glfwSetWindowShouldClose(m_GlfwWindow, GL_TRUE);
}

GLFWwindow* Window::GetGlfwWindow() const
{
    return m_GlfwWindow;
}

void Window::SetHints()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHintString(GLFW_X11_CLASS_NAME, "nova_engine");
    glfwWindowHintString(GLFW_WAYLAND_APP_ID, "nova_engine");
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_SAMPLES, 4);
}
} // namespace N::C
