#pragma once

#include <sim-driver/CallbackWrapper.hpp>
#include <sim-driver/SimData.hpp>
#include <memory>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>

namespace sim
{

template<size_t N>
struct priority_tag: public priority_tag<N - 1>
{
};
template<>
struct priority_tag<0>
{
};

template<typename C = EmptyCallbacks>
class SimCallbacks
{
public:
    explicit SimCallbacks(SimData *pSimData, C *pCallbacks = nullptr);

    void framebufferSizeCallback(GLFWwindow *pWindow, int width, int height);
    void windowFocusCallback(GLFWwindow *pWindow, int focus);

    void mouseButtonCallback(GLFWwindow *pWindow, int button, int action, int mods);
    void keyCallback(GLFWwindow *pWindow, int key, int scancode, int action, int mods);
    void cursorPosCallback(GLFWwindow *pWindow, double xpos, double ypos);
    void scrollCallback(GLFWwindow *pWindow, double xoffset, double yoffset);
    void charCallback(GLFWwindow *pWindow, unsigned codepoint);

    bool isLeftMouseDown() const;
    bool isRightMouseDown() const;
    bool isShiftDown() const;
    bool isCtrlDown() const;

private:
    SimData *pSimData_;
    C *pCallbacks_;

    sim::priority_tag<2> p_;

    bool leftMouseDown_{false};
    bool rightMouseDown_{false};

    bool shiftDown_{false};
    bool ctrlDown_{false};

    double prevX_;
    double prevY_;

    template<typename T>
    auto framebufferSizeCallback(
        T &callbacks, const SimCallbacks<T> &parent, GLFWwindow *window, int width, int height, priority_tag<2> p)
    -> decltype(callbacks.framebufferSizeCallback(window, width, height, parent), void());
    template<typename T>
    auto framebufferSizeCallback(
        T &callbacks, const SimCallbacks<T> &parent, GLFWwindow *window, int width, int height, priority_tag<1> p)
    -> decltype(callbacks.framebufferSizeCallback(window, width, height), void());
    template<typename T>
    void framebufferSizeCallback(
        T &callbacks, const SimCallbacks<T> &parent, GLFWwindow *window, int width, int height, priority_tag<0> p);
    template<typename T>
    auto
    windowFocusCallback(T &callbacks, const SimCallbacks<T> &parent, GLFWwindow *window, int focus, priority_tag<2> p)
    -> decltype(callbacks.windowFocusCallback(window, focus, parent), void());
    template<typename T>
    auto
    windowFocusCallback(T &callbacks, const SimCallbacks<T> &parent, GLFWwindow *window, int focus, priority_tag<1> p)
    -> decltype(callbacks.windowFocusCallback(window, focus), void());
    template<typename T>
    void
    windowFocusCallback(T &callbacks, const SimCallbacks<T> &parent, GLFWwindow *window, int focus, priority_tag<0> p);
    template<typename T>
    auto mouseButtonCallback(
        T &callbacks,
        const SimCallbacks<T> &parent,
        GLFWwindow *window,
        int button,
        int action,
        int mods,
        priority_tag<2> p)
    -> decltype(callbacks.mouseButtonCallback(window, button, action, mods, parent), void());
    template<typename T>
    auto mouseButtonCallback(
        T &callbacks,
        const SimCallbacks<T> &parent,
        GLFWwindow *window,
        int button,
        int action,
        int mods,
        priority_tag<1> p)
    -> decltype(callbacks.mouseButtonCallback(window, button, action, mods), void());
    template<typename T>
    void mouseButtonCallback(
        T &callbacks,
        const SimCallbacks<T> &parent,
        GLFWwindow *window,
        int button,
        int action,
        int mods,
        priority_tag<0> p);
    template<typename T>
    auto keyCallback(T &callbacks,
                     const SimCallbacks<T> &parent,
                     GLFWwindow *window,
                     int key,
                     int scancode,
                     int action,
                     int mods,
                     priority_tag<2> p) -> decltype(callbacks
        .keyCallback(window, key, scancode, action, mods, parent), void());
    template<typename T>
    auto keyCallback(T &callbacks,
                     const SimCallbacks<T> &parent,
                     GLFWwindow *window,
                     int key,
                     int scancode,
                     int action,
                     int mods,
                     priority_tag<1> p) -> decltype(callbacks.keyCallback(window, key, scancode, action, mods), void());
    template<typename T>
    void keyCallback(T &callbacks,
                     const SimCallbacks<T> &parent,
                     GLFWwindow *window,
                     int key,
                     int scancode,
                     int action,
                     int mods,
                     priority_tag<0> p);
    template<typename T>
    auto
    cursorPosCallback(T &callbacks,
                      const SimCallbacks<T> &parent,
                      GLFWwindow *window,
                      double xpos,
                      double ypos,
                      priority_tag<2> p)
    -> decltype(callbacks.cursorPosCallback(window, xpos, ypos, parent), void());
    template<typename T>
    auto
    cursorPosCallback(T &callbacks,
                      const SimCallbacks<T> &parent,
                      GLFWwindow *window,
                      double xpos,
                      double ypos,
                      priority_tag<1> p)
    -> decltype(callbacks.cursorPosCallback(window, xpos, ypos), void());
    template<typename T>
    void
    cursorPosCallback(T &callbacks,
                      const SimCallbacks<T> &parent,
                      GLFWwindow *window,
                      double xpos,
                      double ypos,
                      priority_tag<0> p);
    template<typename T>
    auto scrollCallback(
        T &callbacks,
        const SimCallbacks<T> &parent,
        GLFWwindow *window,
        double xoffset,
        double yoffset,
        priority_tag<2> p)
    -> decltype(callbacks.scrollCallback(window, xoffset, yoffset, parent), void());
    template<typename T>
    auto scrollCallback(
        T &callbacks,
        const SimCallbacks<T> &parent,
        GLFWwindow *window,
        double xoffset,
        double yoffset,
        priority_tag<1> p)
    -> decltype(callbacks.scrollCallback(window, xoffset, yoffset), void());
    template<typename T>
    void scrollCallback(
        T &callbacks,
        const SimCallbacks<T> &parent,
        GLFWwindow *window,
        double xoffset,
        double yoffset,
        priority_tag<0> p);
    template<typename T>
    auto
    charCallback(T &callbacks, const SimCallbacks<T> &parent, GLFWwindow *window, unsigned codepoint, priority_tag<2> p)
    -> decltype(callbacks.charCallback(window, codepoint, parent), void());
    template<typename T>
    auto
    charCallback(T &callbacks, const SimCallbacks<T> &parent, GLFWwindow *window, unsigned codepoint, priority_tag<1> p)
    -> decltype(callbacks.charCallback(window, codepoint), void());
    template<typename T>
    void charCallback(T &callbacks,
                      const SimCallbacks<T> &parent,
                      GLFWwindow *window,
                      unsigned codepoint,
                      priority_tag<0> p);
};

template<typename C>
SimCallbacks<C>::SimCallbacks(SimData *pSimData, C *pCallbacks)
    : pSimData_{pSimData}, pCallbacks_(pCallbacks)
{
}

template<typename C>
void SimCallbacks<C>::framebufferSizeCallback(GLFWwindow *pWindow, int width, int height)
{
    if (pSimData_) {
        pSimData_->camera().setAspectRatio(static_cast<float>(width) / height);
    }
    if (pCallbacks_) {
        framebufferSizeCallback(*pCallbacks_, *this, pWindow, width, height, p_);
    }
}
template<typename C>
void SimCallbacks<C>::windowFocusCallback(GLFWwindow *pWindow, int focus)
{
    if (pCallbacks_) {
        windowFocusCallback(*pCallbacks_, *this, pWindow, focus, p_);
    }
}
template<typename C>
void SimCallbacks<C>::mouseButtonCallback(GLFWwindow *pWindow, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_1) {
        if (action == GLFW_PRESS) {
            leftMouseDown_ = true;
            glfwGetCursorPos(pWindow, &prevX_, &prevY_);
        }
        else if (action == GLFW_RELEASE) {
            leftMouseDown_ = false;
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_2) {
        if (action == GLFW_PRESS) {
            rightMouseDown_ = true;
            glfwGetCursorPos(pWindow, &prevX_, &prevY_);
        }
        else if (action == GLFW_RELEASE) {
            rightMouseDown_ = false;
        }
    }

    if (pCallbacks_) {
        mouseButtonCallback(*pCallbacks_, *this, pWindow, button, action, mods, p_);
    }
}
template<typename C>
void SimCallbacks<C>::keyCallback(GLFWwindow *pWindow, int key, int scancode, int action, int mods)
{
    switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(pWindow, GLFW_TRUE);
            break;

        case GLFW_KEY_S:
            if (action == GLFW_RELEASE && mods == GLFW_MOD_CONTROL) {
                // save something?
            }
            break;

        case GLFW_KEY_P:
            if (pSimData_ && action == GLFW_RELEASE) {
                pSimData_->paused = !(pSimData_->paused);
            }
        default:
            break;
    }

    shiftDown_ = (mods == GLFW_MOD_SHIFT);
    ctrlDown_ = (mods == GLFW_MOD_CONTROL);

    if (pCallbacks_) {
        keyCallback(*pCallbacks_, *this, pWindow, key, scancode, action, mods, p_);
    }
}
template<typename C>
void SimCallbacks<C>::cursorPosCallback(GLFWwindow *pWindow, double xpos, double ypos)
{
    if (pSimData_) {
        if (leftMouseDown_) {
            pSimData_->cameraMover.pitch(static_cast<float>(prevY_ - ypos));
            pSimData_->cameraMover.yaw(static_cast<float>(prevX_ - xpos));
        }
        else if (rightMouseDown_) {
            pSimData_->cameraMover.zoom(static_cast<float>(prevY_ - ypos));
        }
    }

    prevX_ = xpos;
    prevY_ = ypos;

    if (pCallbacks_) {
        cursorPosCallback(*pCallbacks_, *this, pWindow, xpos, ypos, p_);
    }
}
template<typename C>
void SimCallbacks<C>::scrollCallback(GLFWwindow *pWindow, double xoffset, double yoffset)
{
    if (pSimData_) {
        pSimData_->cameraMover.zoom(static_cast<float>(-yoffset));
    }
    if (pCallbacks_) {
        scrollCallback(*pCallbacks_, *this, pWindow, xoffset, yoffset, p_);
    }
}
template<typename C>
void SimCallbacks<C>::charCallback(GLFWwindow *pWindow, unsigned codepoint)
{
    if (pCallbacks_) {
        charCallback(*pCallbacks_, *this, pWindow, codepoint, p_);
    }
}

/////////////////////////////////////// Private implementation functions ///////////////////////////////////////

template<typename C>
template<typename T>
auto SimCallbacks<C>::framebufferSizeCallback(
    T &callbacks, const SimCallbacks<T> &parent, GLFWwindow *window, int width, int height, priority_tag<2>)
-> decltype(callbacks.framebufferSizeCallback(window, width, height, parent), void())
{
    static_assert(std::is_same<T, C>::value, "");
    callbacks.framebufferSizeCallback(window, width, height, parent);
}

template<typename C>
template<typename T>
auto SimCallbacks<C>::framebufferSizeCallback(
    T &callbacks, const SimCallbacks<T> &, GLFWwindow *window, int width, int height, priority_tag<1>)
-> decltype(callbacks.framebufferSizeCallback(window, width, height), void())
{
    static_assert(std::is_same<T, C>::value, "");
    callbacks.framebufferSizeCallback(window, width, height);
}

template<typename C>
template<typename T>
auto SimCallbacks<C>::windowFocusCallback(
    T &callbacks, const SimCallbacks<T> &parent, GLFWwindow *window, int focus, priority_tag<2>)
-> decltype(callbacks.windowFocusCallback(window, focus, parent), void())
{
    static_assert(std::is_same<T, C>::value, "");
    callbacks.windowFocusCallback(window, focus, parent);
}

template<typename C>
template<typename T>
auto SimCallbacks<C>::windowFocusCallback(
    T &callbacks, const SimCallbacks<T> &parent, GLFWwindow *window, int focus, priority_tag<1>)
-> decltype(callbacks.windowFocusCallback(window, focus), void())
{
    static_assert(std::is_same<T, C>::value, "");
    callbacks.windowFocusCallback(window, focus);
}

template<typename C>
template<typename T>
auto SimCallbacks<C>::mouseButtonCallback(
    T &callbacks, const SimCallbacks<T> &parent, GLFWwindow *window, int button, int action, int mods, priority_tag<2>)
-> decltype(callbacks.mouseButtonCallback(window, button, action, mods, parent), void())
{
    static_assert(std::is_same<T, C>::value, "");
    callbacks.mouseButtonCallback(window, button, action, mods, parent);
}

template<typename C>
template<typename T>
auto SimCallbacks<C>::mouseButtonCallback(
    T &callbacks, const SimCallbacks<T> &parent, GLFWwindow *window, int button, int action, int mods, priority_tag<1>)
-> decltype(callbacks.mouseButtonCallback(window, button, action, mods), void())
{
    static_assert(std::is_same<T, C>::value, "");
    callbacks.mouseButtonCallback(window, button, action, mods);
}

template<typename C>
template<typename T>
auto SimCallbacks<C>::keyCallback(
    T &callbacks,
    const SimCallbacks<T> &parent,
    GLFWwindow *window,
    int key,
    int scancode,
    int action,
    int mods,
    priority_tag<2>)
-> decltype(callbacks.keyCallback(window, key, scancode, action, mods, parent), void())
{
    static_assert(std::is_same<T, C>::value, "");
    callbacks.keyCallback(window, key, scancode, action, mods, parent);
}

template<typename C>
template<typename T>
auto SimCallbacks<C>::keyCallback(
    T &callbacks,
    const SimCallbacks<T> &parent,
    GLFWwindow *window,
    int key,
    int scancode,
    int action,
    int mods,
    priority_tag<1>)
-> decltype(callbacks.keyCallback(window, key, scancode, action, mods), void())
{
    static_assert(std::is_same<T, C>::value, "");
    callbacks.keyCallback(window, key, scancode, action, mods);
}

template<typename C>
template<typename T>
auto SimCallbacks<C>::cursorPosCallback(
    T &callbacks, const SimCallbacks<T> &parent, GLFWwindow *window, double xpos, double ypos, priority_tag<2>)
-> decltype(callbacks.cursorPosCallback(window, xpos, ypos, parent), void())
{
    static_assert(std::is_same<T, C>::value, "");
    callbacks.cursorPosCallback(window, xpos, ypos, parent);
}

template<typename C>
template<typename T>
auto SimCallbacks<C>::cursorPosCallback(
    T &callbacks, const SimCallbacks<T> &parent, GLFWwindow *window, double xpos, double ypos, priority_tag<1>)
-> decltype(callbacks.cursorPosCallback(window, xpos, ypos), void())
{
    static_assert(std::is_same<T, C>::value, "");
    callbacks.cursorPosCallback(window, xpos, ypos);
}

template<typename C>
template<typename T>
auto SimCallbacks<C>::scrollCallback(
    T &callbacks, const SimCallbacks<T> &parent, GLFWwindow *window, double xoffset, double yoffset, priority_tag<2>)
-> decltype(callbacks.scrollCallback(window, xoffset, yoffset, parent), void())
{
    static_assert(std::is_same<T, C>::value, "");
    callbacks.scrollCallback(window, xoffset, yoffset, parent);
}

template<typename C>
template<typename T>
auto SimCallbacks<C>::scrollCallback(
    T &callbacks, const SimCallbacks<T> &parent, GLFWwindow *window, double xoffset, double yoffset, priority_tag<1>)
-> decltype(callbacks.scrollCallback(window, xoffset, yoffset), void())
{
    static_assert(std::is_same<T, C>::value, "");
    callbacks.scrollCallback(window, xoffset, yoffset);
}

template<typename C>
template<typename T>
auto SimCallbacks<C>::charCallback(
    T &callbacks, const SimCallbacks<T> &parent, GLFWwindow *window, unsigned codepoint, priority_tag<2>)
-> decltype(callbacks.charCallback(window, codepoint, parent), void())
{
    static_assert(std::is_same<T, C>::value, "");
    callbacks.charCallback(window, codepoint, parent);
}

template<typename C>
template<typename T>
auto SimCallbacks<C>::charCallback(
    T &callbacks, const SimCallbacks<T> &parent, GLFWwindow *window, unsigned codepoint, priority_tag<1>)
-> decltype(callbacks.charCallback(window, codepoint), void())
{
    static_assert(std::is_same<T, C>::value, "");
    callbacks.charCallback(window, codepoint);
}

/////////////////////////////////////// Empty implementation functions ///////////////////////////////////////

template<typename C>
template<typename T>
void SimCallbacks<C>::framebufferSizeCallback(T &, const SimCallbacks<T> &, GLFWwindow *, int, int, priority_tag<0>)
{
}
template<typename C>
template<typename T>
void SimCallbacks<C>::windowFocusCallback(T &, const SimCallbacks<T> &, GLFWwindow *, int, priority_tag<0>)
{
}
template<typename C>
template<typename T>
void SimCallbacks<C>::mouseButtonCallback(T &, const SimCallbacks<T> &, GLFWwindow *, int, int, int, priority_tag<0>)
{
}
template<typename C>
template<typename T>
void SimCallbacks<C>::keyCallback(T &, const SimCallbacks<T> &, GLFWwindow *, int, int, int, int, priority_tag<0>)
{
}
template<typename C>
template<typename T>
void SimCallbacks<C>::cursorPosCallback(T &, const SimCallbacks<T> &, GLFWwindow *, double, double, priority_tag<0>)
{
}
template<typename C>
template<typename T>
void SimCallbacks<C>::scrollCallback(T &, const SimCallbacks<T> &, GLFWwindow *, double, double, priority_tag<0>)
{
}
template<typename C>
template<typename T>
void SimCallbacks<C>::charCallback(T &, const SimCallbacks<T> &, GLFWwindow *, unsigned, priority_tag<0>)
{
}

template<typename C>
bool SimCallbacks<C>::isLeftMouseDown() const
{
    return leftMouseDown_;
}

template<typename C>
bool SimCallbacks<C>::isRightMouseDown() const
{
    return rightMouseDown_;
}

template<typename C>
bool SimCallbacks<C>::isShiftDown() const
{
    return shiftDown_;
}

template<typename C>
bool SimCallbacks<C>::isCtrlDown() const
{
    return ctrlDown_;
}

} // namespace sim
