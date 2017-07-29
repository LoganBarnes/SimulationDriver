#pragma once

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#include <string>
#include <iostream>

namespace sim
{

template<typename Child>
class SimDriver
{
public:

    void runEventLoop();

    int width() const
    {
        int w, h;
        glfwGetWindowSize(pWindow_, &w, &h);
        return w;
    }

    int height() const
    {
        int w, h;
        glfwGetWindowSize(pWindow_, &w, &h);
        return h;
    }

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
void SimDriver<Child>::update()
{
    static_cast<Child *>(this)->update(worldTime_, timeStep_);
}

template<typename Child>
void SimDriver<Child>::render(double alpha, bool eventBased)
{
    static_cast<Child *>(this)->render(width(), height(), alpha, eventBased);
    glfwSwapBuffers(pWindow_);
}

} // namespace sim
