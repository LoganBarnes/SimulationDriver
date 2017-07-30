#pragma once

#include <sim-driver/cameras/Camera.hpp>
#include <sim-driver/callbacks/Callbacks.hpp>

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#include <string>
#include <iostream>
#include <memory>

namespace sim
{

template<typename Child>
class SimDriver
{
public:

    void runEventLoop();

    template<typename T>
    void setCallbackClass(T *pCallbacks);

    SimDriver(const SimDriver &) = delete;
    SimDriver &operator=(const SimDriver &) = delete;

protected:
    explicit SimDriver(std::string title = "Sim Window",
                       int width = 0,
                       int height = 0);

    virtual ~SimDriver();
    SimDriver(SimDriver &&) noexcept = default;
    SimDriver &operator=(SimDriver &&) noexcept = default;

private:

    double timeStep_{0.1};
    double worldTime_{0.0};
    bool paused_{false};

    GLFWwindow *pWindow_{nullptr};
    Camera camera_;
    Callbacks callbacks_{};

    void update();
    void render(double alpha, bool eventBased = false);

};


template<typename Child>
SimDriver<Child>::SimDriver(std::string title, int width, int height)
{
    glfwSetErrorCallback([](int error, const char *description)
                         { std::cerr << "ERROR: (" << error << ") " << description << std::endl; });

    if (!glfwInit())
    {
        throw std::runtime_error("GLFW init failed");
    }

    if (width == 0 || height == 0)
    {
        const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        width = mode->width;
        height = mode->height;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1); // highest on mac :(
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE );
#endif // __APPLE__

    pWindow_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    if (!pWindow_)
    {
        throw std::runtime_error("GLFW window creation failed");
    }

    glfwMakeContextCurrent(pWindow_);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader(reinterpret_cast< GLADloadproc >( glfwGetProcAddress )))
    {
        throw std::runtime_error("Failed to initialize OpenGL context");
    }

    ImGui_ImplGlfwGL3_Init(pWindow_, false); // false for no callbacks

    setCallbackClass(&callbacks_);
}


template<typename Child>
SimDriver<Child>::~SimDriver()
{
    ImGui_ImplGlfwGL3_Shutdown();
    if (pWindow_)
    {
        glfwDestroyWindow(pWindow_);
        pWindow_ = nullptr;
    }
    glfwTerminate();
}


template<typename Child>
void SimDriver<Child>::runEventLoop()
{
    do
    {
        if (!paused_)
        {
            update();
            worldTime_ += timeStep_;
        }
        render(1.0, true);

        glfwWaitEvents();
    } while (!glfwWindowShouldClose(pWindow_));
}


template<typename Child>
template<typename T>
void SimDriver<Child>::setCallbackClass(T *pCallbacks)
{
    glfwSetWindowUserPointer(pWindow_, pCallbacks);

    glfwSetWindowSizeCallback(pWindow_, [](GLFWwindow *pWindow, int width, int height)
    { static_cast<T *>(glfwGetWindowUserPointer(pWindow))->windowSizeCallback(pWindow, width, height); });

    glfwSetWindowFocusCallback(pWindow_, [](GLFWwindow *pWindow, int focus)
    { static_cast<T *>(glfwGetWindowUserPointer(pWindow))->windowFocusCallback(pWindow, focus); });

    glfwSetMouseButtonCallback(pWindow_, [](GLFWwindow *pWindow, int button, int action, int mods)
    { static_cast<T *>(glfwGetWindowUserPointer(pWindow))->mouseButtonCallback(pWindow, button, action, mods); });

    glfwSetKeyCallback(pWindow_, [](GLFWwindow *pWindow, int key, int scancode, int action, int mods)
    { static_cast<T *>(glfwGetWindowUserPointer(pWindow))->keyCallback(pWindow, key, scancode, action, mods); });

    glfwSetCursorPosCallback(pWindow_, [](GLFWwindow *pWindow, double xpos, double ypos)
    { static_cast<T *>(glfwGetWindowUserPointer(pWindow))->cursorPosCallback(pWindow, xpos, ypos); });

    glfwSetScrollCallback(pWindow_, [](GLFWwindow *pWindow, double xoffset, double yoffset)
    { static_cast<T *>(glfwGetWindowUserPointer(pWindow))->scrollCallback(pWindow, xoffset, yoffset); });

    glfwSetCharCallback(pWindow_, [](GLFWwindow *pWindow, unsigned codepoint)
    { static_cast<T *>(glfwGetWindowUserPointer(pWindow))->charCallback(pWindow, codepoint); });
}


template<typename Child>
void SimDriver<Child>::update()
{
    static_cast<Child *>(this)->update(worldTime_, timeStep_);
}

template<typename Child>
void SimDriver<Child>::render(double alpha, bool eventBased)
{
    int w, h;
    glfwGetWindowSize(pWindow_, &w, &h);
    static_cast<Child *>(this)->render(w, h, alpha, eventBased);
    glfwSwapBuffers(pWindow_);
}

} // namespace sim
