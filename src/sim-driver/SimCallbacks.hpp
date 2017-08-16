#pragma once

#include <sim-driver/CallbackWrapper.hpp>
#include <sim-driver/SimData.hpp>
#include <memory>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>

namespace sim
{

template<typename C = EmptyCallbacks>
class SimCallbacks
{
public:

    explicit SimCallbacks(const std::shared_ptr<SimData>& spSimData, const std::shared_ptr<C>& spCallbacks = nullptr);

    void windowSizeCallback(GLFWwindow *pWindow, int width, int height);
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

    std::weak_ptr<SimData> wpSimData_;
    std::weak_ptr<C> wpCallbacks_;

    bool leftMouseDown_{false};
    bool rightMouseDown_{false};

    bool shiftDown_{false};
    bool ctrlDown_{false};

    double prevX_;
    double prevY_;

    template<typename T=C>
    auto windowSizeCallback(T &callbacks,
                            const SimCallbacks<T> &parent,
                            GLFWwindow *window,
                            int width,
                            int height,
                            int i)
    -> decltype(callbacks.windowSizeCallback(window, width, height, parent), void());
    template<typename T=C>
    auto windowFocusCallback(T &callbacks, const SimCallbacks<T> &parent, GLFWwindow *window, int focus, int i)
    -> decltype(callbacks.windowFocusCallback(window, focus, parent), void());
    template<typename T=C>
    auto mouseButtonCallback(T &callbacks,
                             const SimCallbacks<T> &parent,
                             GLFWwindow *window,
                             int button,
                             int action,
                             int mods,
                             int i)
    -> decltype(callbacks.mouseButtonCallback(window, button, action, mods, parent), void());
    template<typename T=C>
    auto keyCallback(T &callbacks,
                     const SimCallbacks<T> &parent,
                     GLFWwindow *window,
                     int key,
                     int scancode,
                     int action,
                     int mods,
                     int i)
    -> decltype(callbacks.keyCallback(window, key, scancode, action, mods, parent), void());
    template<typename T=C>
    auto cursorPosCallback(T &callbacks,
                           const SimCallbacks<T> &parent,
                           GLFWwindow *window,
                           double xpos,
                           double ypos,
                           int i)
    -> decltype(callbacks.cursorPosCallback(window, xpos, ypos, parent), void());
    template<typename T=C>
    auto scrollCallback(T &callbacks,
                        const SimCallbacks<T> &parent,
                        GLFWwindow *window,
                        double xoffset,
                        double yoffset,
                        int i)
    -> decltype(callbacks.scrollCallback(window, xoffset, yoffset, parent), void());
    template<typename T=C>
    auto charCallback(T &callbacks,
                      const SimCallbacks<T> &parent,
                      GLFWwindow *window,
                      unsigned codepoint,
                      int i)
    -> decltype(callbacks.charCallback(window, codepoint, parent), void());


    template<typename T=C>
    auto windowSizeCallback(T &callbacks,
                            const SimCallbacks<T> &parent,
                            GLFWwindow *window,
                            int width,
                            int height,
                            long l) -> decltype(void());
    template<typename T=C>
    auto windowFocusCallback(T &callbacks, const SimCallbacks<T> &parent, GLFWwindow *window, int focus, long l)
    -> decltype(void());
    template<typename T=C>
    auto mouseButtonCallback(T &callbacks,
                             const SimCallbacks<T> &parent,
                             GLFWwindow *window,
                             int button,
                             int action,
                             int mods,
                             long l) -> decltype(void());
    template<typename T=C>
    auto keyCallback(T &callbacks,
                     const SimCallbacks<T> &parent,
                     GLFWwindow *window,
                     int key,
                     int scancode,
                     int action,
                     int mods,
                     long l) -> decltype(void());
    template<typename T=C>
    auto cursorPosCallback(T &callbacks,
                           const SimCallbacks<T> &parent,
                           GLFWwindow *window,
                           double xpos,
                           double ypos,
                           long l) -> decltype(void());
    template<typename T=C>
    auto scrollCallback(T &callbacks,
                        const SimCallbacks<T> &parent,
                        GLFWwindow *window,
                        double xoffset,
                        double yoffset,
                        long l) -> decltype(void());
    template<typename T=C>
    auto charCallback(T &callbacks,
                      const SimCallbacks<T> &parent,
                      GLFWwindow *window,
                      unsigned codepoint,
                      long l) -> decltype(void());
};


template<typename C>
SimCallbacks<C>::SimCallbacks(const std::shared_ptr<SimData>& spSimData, const std::shared_ptr<C>& spCallbacks)
        : wpSimData_{spSimData},
          wpCallbacks_(spCallbacks) {}

template<typename C>
void SimCallbacks<C>::windowSizeCallback(GLFWwindow *pWindow, int width, int height)
{
    if (auto spSimData = wpSimData_.lock())
    {
        spSimData->camera.setAspectRatio(static_cast<float>(width) / height);
    }
    if (auto spCallbacks = wpCallbacks_.lock())
    {
        windowSizeCallback(*spCallbacks, *this, pWindow, width, height, 0);
    }
}
template<typename C>
void SimCallbacks<C>::windowFocusCallback(GLFWwindow *pWindow, int focus)
{
    if (auto spCallbacks = wpCallbacks_.lock())
    {
        windowFocusCallback(*spCallbacks, *this, pWindow, focus, 0);
    }
}
template<typename C>
void SimCallbacks<C>::mouseButtonCallback(GLFWwindow *pWindow, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_1)
    {
        if (action == GLFW_PRESS)
        {
            leftMouseDown_ = true;
            glfwGetCursorPos(pWindow, &prevX_, &prevY_);
        }
        else if (action == GLFW_RELEASE)
        {
            leftMouseDown_ = false;
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_2)
    {
        if (action == GLFW_PRESS)
        {
            rightMouseDown_ = true;
            glfwGetCursorPos(pWindow, &prevX_, &prevY_);
        }
        else if (action == GLFW_RELEASE)
        {
            rightMouseDown_ = false;
        }
    }

    if (auto spCallbacks = wpCallbacks_.lock())
    {
        mouseButtonCallback(*spCallbacks, *this, pWindow, button, action, mods, 0);
    }
}
template<typename C>
void SimCallbacks<C>::keyCallback(GLFWwindow *pWindow, int key, int scancode, int action, int mods)
{
    switch (key)
    {
        case GLFW_KEY_ESCAPE:
            if (action == GLFW_RELEASE)
            {
                glfwSetWindowShouldClose(pWindow, GLFW_TRUE);
            }
            break;

        case GLFW_KEY_LEFT_SHIFT:
        case GLFW_KEY_RIGHT_SHIFT:
            shiftDown_ = (action == GLFW_PRESS);
            break;

        case GLFW_KEY_LEFT_CONTROL:
        case GLFW_KEY_RIGHT_CONTROL:
            ctrlDown_ = (action == GLFW_PRESS);
            break;

        case GLFW_KEY_S:
            if (action == GLFW_RELEASE && ctrlDown_)
            {
                // save something?
            }
            break;

        case GLFW_KEY_R:
            if (action == GLFW_RELEASE)
            {
//                handler_.showWorld();
            }
        default:
            break;
    }
    if (auto spCallbacks = wpCallbacks_.lock())
    {
        keyCallback(*spCallbacks, *this, pWindow, key, scancode, action, mods, 0);
    }
}
template<typename C>
void SimCallbacks<C>::cursorPosCallback(GLFWwindow *pWindow, double xpos, double ypos)
{
    if (auto spSimData = wpSimData_.lock())
    {
        if (leftMouseDown_)
        {
            spSimData->camera.yaw(static_cast<float>(prevX_ - xpos) * 0.01f);
            spSimData->camera.pitch(static_cast<float>(prevY_ - ypos) * 0.01f);
        }
        else if (rightMouseDown_)
        {
            float dist = spSimData->camera.getOrbitOffsetDistance();
            spSimData->camera.setOrbitOffsetDistance(dist + dist * 0.01f * static_cast<float>(prevY_ - ypos));
        }
    }

    prevX_ = xpos;
    prevY_ = ypos;

    if (auto spCallbacks = wpCallbacks_.lock())
    {
        cursorPosCallback(*spCallbacks, *this, pWindow, xpos, ypos, 0);
    }
}
template<typename C>
void SimCallbacks<C>::scrollCallback(GLFWwindow *pWindow, double xoffset, double yoffset)
{
    if (auto spSimData = wpSimData_.lock())
    {
        float dist = spSimData->camera.getOrbitOffsetDistance();
        spSimData->camera.setOrbitOffsetDistance(dist + dist * 0.01f * static_cast<float>(-yoffset));
    }
    if (auto spCallbacks = wpCallbacks_.lock())
    {
        scrollCallback(*spCallbacks, *this, pWindow, xoffset, yoffset, 0);
    }
}
template<typename C>
void SimCallbacks<C>::charCallback(GLFWwindow *pWindow, unsigned codepoint)
{
    if (auto spCallbacks = wpCallbacks_.lock())
    {
        charCallback(*spCallbacks, *this, pWindow, codepoint, 0);
    }
}

/////////////////////////////////////// Private implementation functions ///////////////////////////////////////

template<typename C>
template<typename T>
auto SimCallbacks<C>::windowSizeCallback(T &callbacks,
                                            const SimCallbacks<T> &parent,
                                            GLFWwindow *window,
                                            int width,
                                            int height,
                                            int i)
-> decltype(callbacks.windowSizeCallback(window, width, height, parent), void())
{
    static_assert(std::is_same<T, C>::value, "");
    callbacks.windowSizeCallback(window, width, height, parent);
}

template<typename C>
template<typename T>
auto SimCallbacks<C>::windowFocusCallback(T &callbacks,
                                             const SimCallbacks<T> &parent,
                                             GLFWwindow *window,
                                             int focus,
                                             int i)
-> decltype(callbacks.windowFocusCallback(window, focus, parent), void())
{
    static_assert(std::is_same<T, C>::value, "");
    callbacks.windowFocusCallback(window, focus, parent);
}

template<typename C>
template<typename T>
auto
SimCallbacks<C>::mouseButtonCallback(T &callbacks,
                                        const SimCallbacks<T> &parent,
                                        GLFWwindow *window,
                                        int button,
                                        int action,
                                        int mods,
                                        int i)
-> decltype(callbacks.mouseButtonCallback(window, button, action, mods, parent), void())
{
    static_assert(std::is_same<T, C>::value, "");
    callbacks.mouseButtonCallback(window, button, action, mods, parent);
}

template<typename C>
template<typename T>
auto
SimCallbacks<C>::keyCallback(T &callbacks,
                                const SimCallbacks<T> &parent,
                                GLFWwindow *window,
                                int key,
                                int scancode,
                                int action,
                                int mods,
                                int i)
-> decltype(callbacks.keyCallback(window, key, scancode, action, mods, parent), void())
{
    static_assert(std::is_same<T, C>::value, "");
    callbacks.keyCallback(window, key, scancode, action, mods, parent);
}

template<typename C>
template<typename T>
auto SimCallbacks<C>::cursorPosCallback(T &callbacks,
                                           const SimCallbacks<T> &parent,
                                           GLFWwindow *window,
                                           double xpos,
                                           double ypos,
                                           int i)
-> decltype(callbacks.cursorPosCallback(window, xpos, ypos, parent), void())
{
    static_assert(std::is_same<T, C>::value, "");
    callbacks.cursorPosCallback(window, xpos, ypos, parent);
}

template<typename C>
template<typename T>
auto SimCallbacks<C>::scrollCallback(T &callbacks,
                                        const SimCallbacks<T> &parent,
                                        GLFWwindow *window,
                                        double xoffset,
                                        double yoffset,
                                        int i)
-> decltype(callbacks.scrollCallback(window, xoffset, yoffset, parent), void())
{
    static_assert(std::is_same<T, C>::value, "");
    callbacks.scrollCallback(window, xoffset, yoffset, parent);
}

template<typename C>
template<typename T>
auto SimCallbacks<C>::charCallback(T &callbacks,
                                      const SimCallbacks<T> &parent,
                                      GLFWwindow *window,
                                      unsigned codepoint,
                                      int i)
-> decltype(callbacks.charCallback(window, codepoint, parent), void())
{
    static_assert(std::is_same<T, C>::value, "");
    callbacks.charCallback(window, codepoint, parent);
}


/////////////////////////////////////// Empty implementation functions ///////////////////////////////////////

template<typename C>
template<typename T>
auto SimCallbacks<C>::windowSizeCallback(T &callbacks,
                                            const SimCallbacks<T> &parent,
                                            GLFWwindow *window,
                                            int width,
                                            int height,
                                            long l) -> decltype(void())
{
    static_assert(std::is_same<T, C>::value, "");
}
template<typename C>
template<typename T>
auto
SimCallbacks<C>::windowFocusCallback(T &callbacks,
                                        const SimCallbacks<T> &parent,
                                        GLFWwindow *window,
                                        int focus,
                                        long l) -> decltype(void())
{
    static_assert(std::is_same<T, C>::value, "");
}
template<typename C>
template<typename T>
auto
SimCallbacks<C>::mouseButtonCallback(T &callbacks,
                                        const SimCallbacks<T> &parent,
                                        GLFWwindow *window,
                                        int button,
                                        int action,
                                        int mods,
                                        long l) -> decltype(void())
{
    static_assert(std::is_same<T, C>::value, "");
}
template<typename C>
template<typename T>
auto
SimCallbacks<C>::keyCallback(T &callbacks,
                                const SimCallbacks<T> &parent,
                                GLFWwindow *window,
                                int key,
                                int scancode,
                                int action,
                                int mods,
                                long l) -> decltype(void())
{
    static_assert(std::is_same<T, C>::value, "");
}
template<typename C>
template<typename T>
auto SimCallbacks<C>::cursorPosCallback(T &callbacks,
                                           const SimCallbacks<T> &parent,
                                           GLFWwindow *window,
                                           double xpos,
                                           double ypos,
                                           long l) -> decltype(void())
{
    static_assert(std::is_same<T, C>::value, "");
}
template<typename C>
template<typename T>
auto SimCallbacks<C>::scrollCallback(T &callbacks,
                                        const SimCallbacks<T> &parent, GLFWwindow *window,
                                        double xoffset,
                                        double yoffset,
                                        long l) -> decltype(void())
{
    static_assert(std::is_same<T, C>::value, "");
}
template<typename C>
template<typename T>
auto SimCallbacks<C>::charCallback(T &callbacks,
                                      const SimCallbacks<T> &parent,
                                      GLFWwindow *window,
                                      unsigned codepoint,
                                      long l) -> decltype(void())
{
    static_assert(std::is_same<T, C>::value, "");
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
