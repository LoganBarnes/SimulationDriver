#pragma once

#include <sim-driver/cameras/Camera.hpp>
#include <sim-driver/callbacks/CameraCallbacks.hpp>

#include <sim-driver/OpenGLForwardDeclarations.hpp>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#include <string>
#include <iostream>
#include <memory>
#include <chrono>
#include <algorithm>

namespace sim
{

template<typename Child>
class SimDriver
{
public:

    void runEventLoop();
    void runAsFastAsPossibleLoop();
    void runNoFasterThanRealTimeLoop();

    template<typename T>
    void setCallbackClass(T *callbacks);

    SimDriver(const SimDriver &) = delete;
    SimDriver &operator=(const SimDriver &) = delete;

    std::shared_ptr<Camera> camera_;

protected:

    explicit SimDriver(const std::string &title = "Sim Window",
                       int width = 0,
                       int height = 0);

    virtual ~SimDriver();

    SimDriver(SimDriver &&) noexcept = default;
    SimDriver &operator=(SimDriver &&) noexcept = default;

private:
    double timeStep_{1.0 / 60.0};
    double worldTime_{0.0};

    bool paused_{false};
    GLFWwindow *pWindow_{nullptr};
    CameraCallbacks<> callbacks_;

    void update();
    void render(double alpha, bool eventBased = false);

};

template<typename Child>
SimDriver<Child>::SimDriver(const std::string &title, int width, int height)
        : camera_{std::make_shared<Camera>()},
          callbacks_{camera_}
{
    glfwSetErrorCallback([](int error, const char *description)
                         {
                             std::cerr << "ERROR: (" << error << ") " << description << std::endl;
                         });

    if (glfwInit() == 0)
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
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif // __APPLE__

    pWindow_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    if (pWindow_ == nullptr)
    {
        throw std::runtime_error("GLFW window creation failed");
    }

    glfwMakeContextCurrent(pWindow_);
    glfwSwapInterval(1);

    if (gladLoadGLLoader(reinterpret_cast< GLADloadproc >( glfwGetProcAddress )) == 0)
    {
        throw std::runtime_error("Failed to initialize OpenGL context");
    }

    ImGui_ImplGlfwGL3_Init(pWindow_, false); // false for no callbacks

    camera_->setAspectRatio(width / float(height));

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
void SimDriver<Child>::runAsFastAsPossibleLoop()
{
    glfwSwapInterval(0);
    do
    {
        if (!paused_)
        {
            update();
            worldTime_ += timeStep_;
        }
        render(1.0);

        glfwPollEvents();
    } while (!glfwWindowShouldClose(pWindow_));
}


template<typename Child>
void SimDriver<Child>::runNoFasterThanRealTimeLoop()
{
    auto currentTime = std::chrono::steady_clock::now();
    double accumulator = 0.0;

    do
    {
        auto newTime = std::chrono::steady_clock::now();
        double frameTime = std::chrono::duration<double>{newTime - currentTime}.count();
        currentTime = newTime;

        frameTime = std::min(0.1, frameTime);

        accumulator += frameTime;

        while (accumulator >= timeStep_)
        {
            update();
            worldTime_ += timeStep_;
            accumulator -= timeStep_;
        }

        const double alpha = accumulator / timeStep_;

        render(alpha);

        glfwPollEvents();
    } while (!glfwWindowShouldClose(pWindow_));
}


template<typename Child>
template<typename T>
void SimDriver<Child>::setCallbackClass(T *pCallbacks)
{
    glfwSetWindowUserPointer(pWindow_, pCallbacks);

    glfwSetWindowSizeCallback(pWindow_, [](GLFWwindow *pWindow, int width, int height)
    {
        static_cast<T *>(glfwGetWindowUserPointer(pWindow))->windowSizeCallback(pWindow, width, height);
    });

    glfwSetWindowFocusCallback(pWindow_, [](GLFWwindow *pWindow, int focus)
    {
        static_cast<T *>(glfwGetWindowUserPointer(pWindow))->windowFocusCallback(pWindow, focus);
    });

    glfwSetMouseButtonCallback(pWindow_, [](GLFWwindow *pWindow, int button, int action, int mods)
    {
        ImGuiIO &io = ImGui::GetIO();
        if (!io.WantCaptureMouse)
        {
            static_cast<T *>(glfwGetWindowUserPointer(pWindow))->mouseButtonCallback(pWindow, button, action, mods);
        }
        else
        {
            if (action == GLFW_PRESS && button >= 0 && button < 3)
            {
                io.MouseDown[button] = true;
            }
        }
    });

    glfwSetKeyCallback(pWindow_, [](GLFWwindow *pWindow, int key, int scancode, int action, int mods)
    {
        ImGuiIO &io = ImGui::GetIO();
        if (!io.WantCaptureKeyboard)
        {
            static_cast<T *>(glfwGetWindowUserPointer(pWindow))->keyCallback(pWindow, key, scancode, action, mods);
        }
        else
        {
            if (action == GLFW_PRESS)
            {
                io.KeysDown[key] = true;
            }
            else if (action == GLFW_RELEASE)
            {
                io.KeysDown[key] = false;
            }
            io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
            io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
            io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
            io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
        }
    });

    glfwSetCursorPosCallback(pWindow_, [](GLFWwindow *pWindow, double xpos, double ypos)
    {
        ImGuiIO &io = ImGui::GetIO();
        if (!io.WantCaptureMouse)
        {
            static_cast<T *>(glfwGetWindowUserPointer(pWindow))->cursorPosCallback(pWindow, xpos, ypos);
        }
    });

    glfwSetScrollCallback(pWindow_, [](GLFWwindow *pWindow, double xoffset, double yoffset)
    {
        ImGuiIO &io = ImGui::GetIO();
        if (!io.WantCaptureMouse)
        {
            static_cast<T *>(glfwGetWindowUserPointer(pWindow))->scrollCallback(pWindow, xoffset, yoffset);
        }
        else
        {
            io.MouseWheel += static_cast<float>(yoffset); // the fractional mouse wheel. 1.0 unit 5 lines
        }
    });

    glfwSetCharCallback(pWindow_, [](GLFWwindow *pWindow, unsigned codepoint)
    {
        ImGuiIO &io = ImGui::GetIO();
        if (!io.WantCaptureKeyboard)
        {
            static_cast<T *>(glfwGetWindowUserPointer(pWindow))->charCallback(pWindow, codepoint);
        }
        else if (io.WantCaptureKeyboard && codepoint > 0 && codepoint < 0x10000)
        {
            io.AddInputCharacter(static_cast<unsigned short>(codepoint));
        }
    });
}


template<typename Child>
void SimDriver<Child>::update()
{
    static_cast<Child *>(this)->update(worldTime_, timeStep_, 0);
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
