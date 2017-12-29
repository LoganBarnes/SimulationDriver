#pragma once

#include <sim-driver/Camera.hpp>
#include <sim-driver/SimCallbacks.hpp>
#include <sim-driver/SimData.hpp>

#include <sim-driver/OpenGLTypes.hpp>
#ifndef EGL_OFFSCREEN
#include <sim-driver/WindowManager.hpp>
#endif

#include <string>
#include <iostream>
#include <memory>
#include <chrono>
#include <algorithm>
#include <functional>

namespace sim {

template <typename Child>
class SimDriver
{
public:
    SimDriver(const SimDriver &) = delete;
    SimDriver &operator=(const SimDriver &) = delete;

    void runEventLoop(std::size_t max_iterations = std::numeric_limits<std::size_t>::max());
    void runAsFastAsPossibleLoop(std::size_t max_iterations = std::numeric_limits<std::size_t>::max());
    void runNoFasterThanRealTimeLoop(std::size_t max_iterations = std::numeric_limits<std::size_t>::max());

    template <typename C>
    void setCallbackClass(C *callbacks);

    int getWidth() const;
    int getHeight() const;

    GLFWwindow *getWindow();
    const GLFWwindow *getWindow() const;

    SimData simData;

protected:
    explicit SimDriver(SimInitData initData);

    ~SimDriver() = default;
    SimDriver(SimDriver &&) noexcept = default;
    SimDriver &operator=(SimDriver &&) noexcept = default;

private:
    double timeStep_{1.0 / 60.0};
    double worldTime_{0.0};

    int window_idx_{-1};

    SimCallbacks<> callbacks_;

    void update();
    void render(double alpha, bool eventBased = false);
    bool isPaused() const;
};

template <typename Child>
SimDriver<Child>::SimDriver(SimInitData initData) : callbacks_{&simData}
{
    auto &wm = WindowManager::instance();

    window_idx_ = wm.create_window(initData.title, initData.width, initData.height, initData.samples);

    simData.camera().setAspectRatio(getWidth() / float(getHeight()));

    setCallbackClass(&callbacks_);
}

template <typename Child>
void SimDriver<Child>::runEventLoop(std::size_t max_iterations)
{
    std::size_t iterations = 1;
    do {
        if (!isPaused()) {
            update();
            worldTime_ += timeStep_;
            ++iterations;
        }
        render(1.0, true);

        WindowManager::instance().poll_events_blocking();
    } while (!glfwWindowShouldClose(getWindow()) && iterations <= max_iterations);
}

template <typename Child>
void SimDriver<Child>::runAsFastAsPossibleLoop(std::size_t max_iterations)
{
    std::size_t iterations = 1;
    glfwSwapInterval(0);
    do {
        if (!isPaused()) {
            update();
            worldTime_ += timeStep_;
            ++iterations;
        }
        render(1.0, isPaused());

        if (isPaused()) {
            WindowManager::instance().poll_events_blocking();
        } else {
            WindowManager::instance().poll_events_non_blocking();
        }
    } while (!glfwWindowShouldClose(getWindow()) && iterations <= max_iterations);
}

template <typename Child>
void SimDriver<Child>::runNoFasterThanRealTimeLoop(std::size_t max_iterations)
{
    std::size_t iterations = 1;
    auto currentTime = std::chrono::steady_clock::now();
    double accumulator = 0.0;

    do {
        auto newTime = std::chrono::steady_clock::now();
        double frameTime = std::chrono::duration<double>{newTime - currentTime}.count();
        currentTime = newTime;

        frameTime = std::min(0.1, frameTime);

        if (!isPaused()) {
            accumulator += frameTime;

            while (accumulator >= timeStep_) {
                update();
                worldTime_ += timeStep_;
                accumulator -= timeStep_;
                ++iterations;
            }
        }

        const double alpha = accumulator / timeStep_;

        render(alpha, isPaused());

        if (isPaused()) {
            WindowManager::instance().poll_events_blocking();
        } else {
            WindowManager::instance().poll_events_non_blocking();
        }
    } while (!glfwWindowShouldClose(getWindow()));
}

template <typename Child>
template <typename C>
void SimDriver<Child>::setCallbackClass(C *pCallbacks)
{
    glfwSetWindowUserPointer(getWindow(), pCallbacks);

    glfwSetFramebufferSizeCallback(getWindow(), [](GLFWwindow *pWindow, int width, int height) {
        static_cast<C *>(glfwGetWindowUserPointer(pWindow))->framebufferSizeCallback(pWindow, width, height);
    });

    glfwSetWindowFocusCallback(getWindow(), [](GLFWwindow *pWindow, int focus) {
        static_cast<C *>(glfwGetWindowUserPointer(pWindow))->windowFocusCallback(pWindow, focus);
    });

    glfwSetMouseButtonCallback(getWindow(), [](GLFWwindow *pWindow, int button, int action, int mods) {
        ImGuiIO &io = ImGui::GetIO();
        if (!io.WantCaptureMouse) {
            static_cast<C *>(glfwGetWindowUserPointer(pWindow))->mouseButtonCallback(pWindow, button, action, mods);
        } else {
            if (action == GLFW_PRESS && button >= 0 && button < 3) {
                io.MouseDown[button] = true;
            }
        }
    });

    glfwSetKeyCallback(getWindow(), [](GLFWwindow *pWindow, int key, int scancode, int action, int mods) {
        ImGuiIO &io = ImGui::GetIO();
        if (!io.WantCaptureKeyboard) {
            static_cast<C *>(glfwGetWindowUserPointer(pWindow))->keyCallback(pWindow, key, scancode, action, mods);
        } else {
            if (action == GLFW_PRESS) {
                io.KeysDown[key] = true;
            } else if (action == GLFW_RELEASE) {
                io.KeysDown[key] = false;
            }
            io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
            io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
            io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
            io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
        }
    });

    glfwSetCursorPosCallback(getWindow(), [](GLFWwindow *pWindow, double xpos, double ypos) {
        ImGuiIO &io = ImGui::GetIO();
        if (!io.WantCaptureMouse) {
            static_cast<C *>(glfwGetWindowUserPointer(pWindow))->cursorPosCallback(pWindow, xpos, ypos);
        }
    });

    glfwSetScrollCallback(getWindow(), [](GLFWwindow *pWindow, double xoffset, double yoffset) {
        ImGuiIO &io = ImGui::GetIO();
        if (!io.WantCaptureMouse) {
            static_cast<C *>(glfwGetWindowUserPointer(pWindow))->scrollCallback(pWindow, xoffset, yoffset);
        } else {
            io.MouseWheel += static_cast<float>(yoffset); // the fractional mouse wheel. 1.0 unit 5 lines
        }
    });

    glfwSetCharCallback(getWindow(), [](GLFWwindow *pWindow, unsigned codepoint) {
        ImGuiIO &io = ImGui::GetIO();
        if (!io.WantCaptureKeyboard) {
            static_cast<C *>(glfwGetWindowUserPointer(pWindow))->charCallback(pWindow, codepoint);
        } else if (io.WantCaptureKeyboard && codepoint > 0 && codepoint < 0x10000) {
            io.AddInputCharacter(static_cast<unsigned short>(codepoint));
        }
    });
}

template <typename Child>
void SimDriver<Child>::update()
{
    static_cast<Child *>(this)->update(worldTime_, timeStep_);
}

template <typename Child>
void SimDriver<Child>::render(double alpha, bool eventBased)
{
    int w, h;
    glfwGetWindowSize(getWindow(), &w, &h);
    static_cast<Child *>(this)->render(w, h, alpha, eventBased);
    glfwSwapBuffers(getWindow());
}

template <typename Child>
bool SimDriver<Child>::isPaused() const
{
    return static_cast<const Child *>(this)->paused();
}

template <typename Child>
GLFWwindow *SimDriver<Child>::getWindow()
{
    return WindowManager::instance().get_window(window_idx_);
}

template <typename Child>
const GLFWwindow *SimDriver<Child>::getWindow() const
{
    return WindowManager::instance().get_window(window_idx_);
}

template <typename Child>
int SimDriver<Child>::getWidth() const
{
    int w;
    glfwGetWindowSize(WindowManager::instance().get_window(window_idx_), &w, nullptr);
    return w;
}

template <typename Child>
int SimDriver<Child>::getHeight() const
{
    int h;
    glfwGetWindowSize(WindowManager::instance().get_window(window_idx_), nullptr, &h);
    return h;
}

} // namespace sim
