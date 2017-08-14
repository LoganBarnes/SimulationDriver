#pragma once

#include <sim-driver/callbacks/CallbackWrapper.hpp>
#include <memory>

namespace sim
{

template<typename C = EmptyCallbacks>
class CameraCallbacks
{
public:

    CameraCallbacks(std::shared_ptr<sim::Camera> camera, C callbacks = C{});

    void windowSizeCallback(GLFWwindow *pWindow, int width, int height);
    void windowFocusCallback(GLFWwindow *pWindow, int focus);

    void mouseButtonCallback(GLFWwindow *pWindow, int button, int action, int mods);
    void keyCallback(GLFWwindow *pWindow, int key, int scancode, int action, int mods);
    void cursorPosCallback(GLFWwindow *pWindow, double xpos, double ypos);
    void scrollCallback(GLFWwindow *pWindow, double xoffset, double yoffset);
    void charCallback(GLFWwindow *pWindow, unsigned codepoint);

private:

    template<typename T=C>
    auto windowSizeCallback(T &callbacks, GLFWwindow *window, int width, int height, int i)
    -> decltype(callbacks.windowSizeCallback(window, width, height), void());
    template<typename T=C>
    auto windowFocusCallback(T &callbacks, GLFWwindow *window, int focus, int i)
    -> decltype(callbacks.windowFocusCallback(window, focus), void());
    template<typename T=C>
    auto mouseButtonCallback(T &callbacks, GLFWwindow *window, int button, int action, int mods, int i)
    -> decltype(callbacks.mouseButtonCallback(window, button, action, mods), void());
    template<typename T=C>
    auto keyCallback(T &callbacks, GLFWwindow *window, int key, int scancode, int action, int mods, int i)
    -> decltype(callbacks.keyCallback(window, key, scancode, action, mods), void());
    template<typename T=C>
    auto cursorPosCallback(T &callbacks, GLFWwindow *window, double xpos, double ypos, int i)
    -> decltype(callbacks.cursorPosCallback(window, xpos, ypos), void());
    template<typename T=C>
    auto scrollCallback(T &callbacks, GLFWwindow *window, double xoffset, double yoffset, int i)
    -> decltype(callbacks.scrollCallback(window, xoffset, yoffset), void());
    template<typename T=C>
    auto charCallback(T &callbacks, GLFWwindow *window, unsigned codepoint, int i)
    -> decltype(callbacks.charCallback(window, codepoint), void());


    template<typename T=C>
    auto windowSizeCallback(T &callbacks, GLFWwindow *window, int width, int height, long l) -> decltype(void());
    template<typename T=C>
    auto windowFocusCallback(T &callbacks, GLFWwindow *window, int focus, long l) -> decltype(void());
    template<typename T=C>
    auto mouseButtonCallback(T &callbacks, GLFWwindow *window, int button, int action, int mods, long l)
    -> decltype(void());
    template<typename T=C>
    auto keyCallback(T &callbacks, GLFWwindow *window, int key, int scancode, int action, int mods, long l)
    -> decltype(void());
    template<typename T=C>
    auto cursorPosCallback(T &callbacks, GLFWwindow *window, double xpos, double ypos, long l) -> decltype(void());
    template<typename T=C>
    auto scrollCallback(T &callbacks, GLFWwindow *window, double xoffset, double yoffset, long l) -> decltype(void());
    template<typename T=C>
    auto charCallback(T &callbacks, GLFWwindow *window, unsigned codepoint, long l) -> decltype(void());

    std::weak_ptr<Camera> camera_;
    C callbacks_;

    bool leftMouseDown_{false};
    bool rightMouseDown_{false};

    bool shiftDown_{false};
    bool ctrlDown_{false};

    double prevX_;
    double prevY_;
};


template<typename C>
CameraCallbacks<C>::CameraCallbacks(std::shared_ptr<Camera> camera, C callbacks)
        : camera_{camera},
          callbacks_(std::move(callbacks)) {}

template<typename C>
void CameraCallbacks<C>::windowSizeCallback(GLFWwindow *pWindow, int width, int height)
{
    if (auto spCamera = camera_.lock())
    {
        spCamera->setAspectRatio(static_cast<float>(width) / height);
    }
    windowSizeCallback(callbacks_, pWindow, width, height, 0);
}
template<typename C>
void CameraCallbacks<C>::windowFocusCallback(GLFWwindow *pWindow, int focus)
{
    windowFocusCallback(callbacks_, pWindow, focus, 0);
}
template<typename C>
void CameraCallbacks<C>::mouseButtonCallback(GLFWwindow *pWindow, int button, int action, int mods)
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
    mouseButtonCallback(callbacks_, pWindow, button, action, mods, 0);
}
template<typename C>
void CameraCallbacks<C>::keyCallback(GLFWwindow *pWindow, int key, int scancode, int action, int mods)
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
    keyCallback(callbacks_, pWindow, key, scancode, action, mods, 0);
}
template<typename C>
void CameraCallbacks<C>::cursorPosCallback(GLFWwindow *pWindow, double xpos, double ypos)
{
    if (leftMouseDown_)
    {
        if (auto spCamera = camera_.lock())
        {
            spCamera->yaw(static_cast<float>(prevX_ - xpos) * 0.01f);
            spCamera->pitch(static_cast<float>(prevY_ - ypos) * 0.01f);
        }
    }
    else if (rightMouseDown_)
    {
        if (auto spCamera = camera_.lock())
        {
        }
    }

    prevX_ = xpos;
    prevY_ = ypos;

    cursorPosCallback(callbacks_, pWindow, xpos, ypos, 0);
}
template<typename C>
void CameraCallbacks<C>::scrollCallback(GLFWwindow *pWindow, double xoffset, double yoffset)
{
    if (auto spCamera = camera_.lock())
    {
//        camera_.update
    }
    scrollCallback(callbacks_, pWindow, xoffset, yoffset, 0);
}
template<typename C>
void CameraCallbacks<C>::charCallback(GLFWwindow *pWindow, unsigned codepoint)
{
    charCallback(callbacks_, pWindow, codepoint, 0);
}

/////////////////////////////////////// Private implementation functions ///////////////////////////////////////

template<typename C>
template<typename T>
auto CameraCallbacks<C>::windowSizeCallback(T &callbacks, GLFWwindow *window, int width, int height, int i)
-> decltype(callbacks.windowSizeCallback(window, width, height), void())
{
    static_assert(std::is_same<T, C>::value, "");
    callbacks_.windowSizeCallback(window, width, height);
}

template<typename C>
template<typename T>
auto CameraCallbacks<C>::windowFocusCallback(T &callbacks, GLFWwindow *window, int focus, int i)
-> decltype(callbacks.windowFocusCallback(window, focus), void())
{
    static_assert(std::is_same<T, C>::value, "");
    callbacks_.windowFocusCallback(window, focus);
}

template<typename C>
template<typename T>
auto
CameraCallbacks<C>::mouseButtonCallback(T &callbacks, GLFWwindow *window, int button, int action, int mods, int i)
-> decltype(callbacks.mouseButtonCallback(window, button, action, mods), void())
{
    static_assert(std::is_same<T, C>::value, "");
    callbacks_.mouseButtonCallback(window, button, action, mods);
}

template<typename C>
template<typename T>
auto
CameraCallbacks<C>::keyCallback(T &callbacks, GLFWwindow *window, int key, int scancode, int action, int mods, int i)
-> decltype(callbacks.keyCallback(window, key, scancode, action, mods), void())
{
    static_assert(std::is_same<T, C>::value, "");
    callbacks_.keyCallback(window, key, scancode, action, mods);
}

template<typename C>
template<typename T>
auto CameraCallbacks<C>::cursorPosCallback(T &callbacks, GLFWwindow *window, double xpos, double ypos, int i)
-> decltype(callbacks.cursorPosCallback(window, xpos, ypos), void())
{
    static_assert(std::is_same<T, C>::value, "");
    callbacks_.cursorPosCallback(window, xpos, ypos);
}

template<typename C>
template<typename T>
auto CameraCallbacks<C>::scrollCallback(T &callbacks, GLFWwindow *window, double xoffset, double yoffset, int i)
-> decltype(callbacks.scrollCallback(window, xoffset, yoffset), void())
{
    static_assert(std::is_same<T, C>::value, "");
    callbacks_.scrollCallback(window, xoffset, yoffset);
}

template<typename C>
template<typename T>
auto CameraCallbacks<C>::charCallback(T &callbacks, GLFWwindow *window, unsigned codepoint, int i)
-> decltype(callbacks.charCallback(window, codepoint), void())
{
    static_assert(std::is_same<T, C>::value, "");
    callbacks_.charCallback(window, codepoint);
}


/////////////////////////////////////// Empty implementation functions ///////////////////////////////////////

template<typename C>
template<typename T>
auto CameraCallbacks<C>::windowSizeCallback(T &callbacks, GLFWwindow *window, int width, int height, long l)
-> decltype(void())
{
    static_assert(std::is_same<T, C>::value, "");
}
template<typename C>
template<typename T>
auto CameraCallbacks<C>::windowFocusCallback(T &callbacks, GLFWwindow *window, int focus, long l)
-> decltype(void())
{
    static_assert(std::is_same<T, C>::value, "");
}
template<typename C>
template<typename T>
auto
CameraCallbacks<C>::mouseButtonCallback(T &callbacks, GLFWwindow *window, int button, int action, int mods, long l)
-> decltype(void())
{
    static_assert(std::is_same<T, C>::value, "");
}
template<typename C>
template<typename T>
auto
CameraCallbacks<C>::keyCallback(T &callbacks, GLFWwindow *window, int key, int scancode, int action, int mods, long l)
-> decltype(void())
{
    static_assert(std::is_same<T, C>::value, "");
}
template<typename C>
template<typename T>
auto CameraCallbacks<C>::cursorPosCallback(T &callbacks, GLFWwindow *window, double xpos, double ypos, long l)
-> decltype(void())
{
    static_assert(std::is_same<T, C>::value, "");
}
template<typename C>
template<typename T>
auto CameraCallbacks<C>::scrollCallback(T &callbacks, GLFWwindow *window, double xoffset, double yoffset, long l)
-> decltype(void())
{
    static_assert(std::is_same<T, C>::value, "");
}
template<typename C>
template<typename T>
auto CameraCallbacks<C>::charCallback(T &callbacks, GLFWwindow *window, unsigned codepoint, long l)
-> decltype(void())
{
    static_assert(std::is_same<T, C>::value, "");
}

} // namespace sim
