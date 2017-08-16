#pragma once

#include <sim-driver/Camera.hpp>
#include <sim-driver/SimCallbacks.hpp>
#include <sim-driver/SimData.hpp>

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

#ifdef NDEBUG
#define DEBUG_PRINT(msg)
#else
#define DEBUG_PRINT(msg) std::cout << (msg) << std::endl
#endif

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

    const std::shared_ptr<SimData> &getSimData() const;

protected:

    explicit SimDriver(SimInitData initData);

    virtual ~SimDriver() = default;

    SimDriver(SimDriver &&) noexcept = default;
    SimDriver &operator=(SimDriver &&) noexcept = default;

private:

    std::shared_ptr<SimData> spSimData_;

    double timeStep_{1.0 / 60.0};

    double worldTime_{0.0};
    std::unique_ptr<int, std::function<void(int *)>> upGlfw_{nullptr};
    std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow *)>> upWindow_{nullptr};
    std::unique_ptr<bool, std::function<void(bool *)>> upImgui_{nullptr};
    SimCallbacks<> callbacks_;

    void update();
    void render(double alpha, bool eventBased = false);

    GLFWwindow *getWindow();
    const GLFWwindow *getWindow() const;
};

template<typename Child>
SimDriver<Child>::SimDriver(SimInitData initData)
        : spSimData_{std::make_shared<SimData>()},
          callbacks_{spSimData_}
{
    glfwSetErrorCallback([](int error, const char *description)
                         {
                             std::cerr << "ERROR: (" << error << ") " << description << std::endl;
                         });

    upGlfw_ = std::unique_ptr<int, std::function<void(int *)>>(new int(glfwInit()),
                                                               [](auto p)
                                                               {
                                                                   DEBUG_PRINT("Terminating GLFW");
                                                                   glfwTerminate();
                                                                   delete p;
                                                               });
    DEBUG_PRINT("Initializing GLFW");

    if (*upGlfw_ == 0)
    {
        throw std::runtime_error("GLFW init failed");
    }

    if (initData.width == 0 || initData.height == 0)
    {
        const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        initData.width = mode->width;
        initData.height = mode->height;
    }

    glfwWindowHint(GLFW_SAMPLES, initData.samples);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1); // highest on mac :(
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif // __APPLE__

    upWindow_ = std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow *)>>(
            glfwCreateWindow(initData.width, initData.height, initData.title.c_str(), nullptr, nullptr),
            [](auto p)
            {
                if (p)
                {
                    DEBUG_PRINT("Destroying GLFW window");
                    glfwDestroyWindow(p);
                }
            });
    DEBUG_PRINT("Creating GLFW window");

    if (upWindow_ == nullptr)
    {
        throw std::runtime_error("GLFW window creation failed");
    }

    glfwMakeContextCurrent(upWindow_.get());
    glfwSwapInterval(1);

    if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == 0)
    {
        throw std::runtime_error("Failed to initialize OpenGL context");
    }

    upImgui_ = std::unique_ptr<bool, std::function<void(bool *)>>(
            new bool(ImGui_ImplGlfwGL3_Init(upWindow_.get(), false)), // false for no callbacks
            [](auto p)
            {
                DEBUG_PRINT("Shutting down ImGui");
                ImGui_ImplGlfwGL3_Shutdown();
                delete p;
            });
    DEBUG_PRINT("Initializing ImGui");

    spSimData_->camera.setAspectRatio(initData.width / float(initData.height));

    setCallbackClass(&callbacks_);

    DEBUG_PRINT("");
}


template<typename Child>
void SimDriver<Child>::runEventLoop()
{
    do
    {
        if (!spSimData_->paused)
        {
            update();
            worldTime_ += timeStep_;
        }
        render(1.0, true);

        glfwWaitEvents();
    } while (!glfwWindowShouldClose(getWindow()));
}


template<typename Child>
void SimDriver<Child>::runAsFastAsPossibleLoop()
{
    glfwSwapInterval(0);
    do
    {
        if (!spSimData_->paused)
        {
            update();
            worldTime_ += timeStep_;
        }
        render(1.0);

        glfwPollEvents();
    } while (!glfwWindowShouldClose(getWindow()));
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

        if (!spSimData_->paused)
        {
            accumulator += frameTime;

            while (accumulator >= timeStep_)
            {
                update();
                worldTime_ += timeStep_;
                accumulator -= timeStep_;
            }
        }

        const double alpha = accumulator / timeStep_;

        render(alpha);

        glfwPollEvents();
    } while (!glfwWindowShouldClose(getWindow()));
}


template<typename Child>
template<typename T>
void SimDriver<Child>::setCallbackClass(T *pCallbacks)
{
    glfwSetWindowUserPointer(getWindow(), pCallbacks);

    glfwSetWindowSizeCallback(getWindow(), [](GLFWwindow *pWindow, int width, int height)
    {
        static_cast<T *>(glfwGetWindowUserPointer(pWindow))->windowSizeCallback(pWindow, width, height);
    });

    glfwSetWindowFocusCallback(getWindow(), [](GLFWwindow *pWindow, int focus)
    {
        static_cast<T *>(glfwGetWindowUserPointer(pWindow))->windowFocusCallback(pWindow, focus);
    });

    glfwSetMouseButtonCallback(getWindow(), [](GLFWwindow *pWindow, int button, int action, int mods)
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

    glfwSetKeyCallback(getWindow(), [](GLFWwindow *pWindow, int key, int scancode, int action, int mods)
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

    glfwSetCursorPosCallback(getWindow(), [](GLFWwindow *pWindow, double xpos, double ypos)
    {
        ImGuiIO &io = ImGui::GetIO();
        if (!io.WantCaptureMouse)
        {
            static_cast<T *>(glfwGetWindowUserPointer(pWindow))->cursorPosCallback(pWindow, xpos, ypos);
        }
    });

    glfwSetScrollCallback(getWindow(), [](GLFWwindow *pWindow, double xoffset, double yoffset)
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

    glfwSetCharCallback(getWindow(), [](GLFWwindow *pWindow, unsigned codepoint)
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
    static_cast<Child *>(this)->update(worldTime_, timeStep_);
}

template<typename Child>
void SimDriver<Child>::render(double alpha, bool eventBased)
{
    int w, h;
    glfwGetWindowSize(getWindow(), &w, &h);
    static_cast<Child *>(this)->render(w, h, alpha, eventBased);
    glfwSwapBuffers(getWindow());
}

template<typename Child>
GLFWwindow *SimDriver<Child>::getWindow()
{
    return upWindow_.get();
}

template<typename Child>
const GLFWwindow *SimDriver<Child>::getWindow() const
{
    return upWindow_.get();
}

template<typename Child>
const std::shared_ptr<SimData> &SimDriver<Child>::getSimData() const
{
    return spSimData_;
}

} // namespace sim
