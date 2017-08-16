#pragma once

#include <sim-driver/SimDriver.hpp>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <memory>
#include <iostream>

namespace sim
{

class EmptyCallbacks
{
};

template<typename C = EmptyCallbacks>
class CallbackWrapper
{
public:

    CallbackWrapper(C callbacks = C{});

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

    C callbacks_;
};


template<typename C>
CallbackWrapper<C>::CallbackWrapper(C callbacks)
        : callbacks_(std::move(callbacks)) {}

template<typename C>
void CallbackWrapper<C>::windowSizeCallback(GLFWwindow *pWindow, int width, int height)
{
    windowSizeCallback(callbacks_, pWindow, width, height, 0);
}
template<typename C>
void CallbackWrapper<C>::windowFocusCallback(GLFWwindow *pWindow, int focus)
{
    windowFocusCallback(callbacks_, pWindow, focus, 0);
}
template<typename C>
void CallbackWrapper<C>::mouseButtonCallback(GLFWwindow *pWindow, int button, int action, int mods)
{
    mouseButtonCallback(callbacks_, pWindow, button, action, mods, 0);
}
template<typename C>
void CallbackWrapper<C>::keyCallback(GLFWwindow *pWindow, int key, int scancode, int action, int mods)
{
    keyCallback(callbacks_, pWindow, key, scancode, action, mods, 0);
}
template<typename C>
void CallbackWrapper<C>::cursorPosCallback(GLFWwindow *pWindow, double xpos, double ypos)
{
    cursorPosCallback(callbacks_, pWindow, xpos, ypos, 0);
}
template<typename C>
void CallbackWrapper<C>::scrollCallback(GLFWwindow *pWindow, double xoffset, double yoffset)
{
    scrollCallback(callbacks_, pWindow, xoffset, yoffset, 0);
}
template<typename C>
void CallbackWrapper<C>::charCallback(GLFWwindow *pWindow, unsigned codepoint)
{
    charCallback(callbacks_, pWindow, codepoint, 0);
}

/////////////////////////////////////// Private implementation functions ///////////////////////////////////////

template<typename C>
template<typename T>
auto CallbackWrapper<C>::windowSizeCallback(T& callbacks, GLFWwindow* window, int width, int height, int i)
-> decltype(callbacks.windowSizeCallback(window, width, height), void()) {
    static_assert(std::is_same<T, C>::value, "");
    callbacks_.windowSizeCallback(window, width, height);
}

template<typename C>
template<typename T>
auto CallbackWrapper<C>::windowFocusCallback(T& callbacks, GLFWwindow* window, int focus, int i)
-> decltype(callbacks.windowFocusCallback(window, focus), void()) {
    static_assert(std::is_same<T, C>::value, "");
    callbacks_.windowFocusCallback(window, focus);
}

template<typename C>
template<typename T>
auto
CallbackWrapper<C>::mouseButtonCallback(T& callbacks, GLFWwindow* window, int button, int action, int mods, int i)
-> decltype(callbacks.mouseButtonCallback(window, button, action, mods), void()) {
    static_assert(std::is_same<T, C>::value, "");
    callbacks_.mouseButtonCallback(window, button, action, mods);
}

template<typename C>
template<typename T>
auto
CallbackWrapper<C>::keyCallback(T& callbacks, GLFWwindow* window, int key, int scancode, int action, int mods, int i)
-> decltype(callbacks.keyCallback(window, key, scancode, action, mods), void()) {
    static_assert(std::is_same<T, C>::value, "");
    callbacks_.keyCallback(window, key, scancode, action, mods);
}

template<typename C>
template<typename T>
auto CallbackWrapper<C>::cursorPosCallback(T& callbacks, GLFWwindow* window, double xpos, double ypos, int i)
-> decltype(callbacks.cursorPosCallback(window, xpos, ypos), void()) {
    static_assert(std::is_same<T, C>::value, "");
    callbacks_.cursorPosCallback(window, xpos, ypos);
}

template<typename C>
template<typename T>
auto CallbackWrapper<C>::scrollCallback(T& callbacks, GLFWwindow* window, double xoffset, double yoffset, int i)
-> decltype(callbacks.scrollCallback(window, xoffset, yoffset), void()) {
    static_assert(std::is_same<T, C>::value, "");
    callbacks_.scrollCallback(window, xoffset, yoffset);
}

template<typename C>
template<typename T>
auto CallbackWrapper<C>::charCallback(T& callbacks, GLFWwindow* window, unsigned codepoint, int i)
-> decltype(callbacks.charCallback(window, codepoint), void()) {
    static_assert(std::is_same<T, C>::value, "");
    callbacks_.charCallback(window, codepoint);
}


/////////////////////////////////////// Empty implementation functions ///////////////////////////////////////

template<typename C>
template<typename T>
auto CallbackWrapper<C>::windowSizeCallback(T& callbacks, GLFWwindow* window, int width, int height, long l)
-> decltype(void()) {
    static_assert(std::is_same<T, C>::value, "");
}
template<typename C>
template<typename T>
auto CallbackWrapper<C>::windowFocusCallback(T& callbacks, GLFWwindow* window, int focus, long l)
-> decltype(void()) {
    static_assert(std::is_same<T, C>::value, "");
}
template<typename C>
template<typename T>
auto
CallbackWrapper<C>::mouseButtonCallback(T& callbacks, GLFWwindow* window, int button, int action, int mods, long l)
-> decltype(void()) {
    static_assert(std::is_same<T, C>::value, "");
}
template<typename C>
template<typename T>
auto
CallbackWrapper<C>::keyCallback(T& callbacks, GLFWwindow* window, int key, int scancode, int action, int mods, long l)
-> decltype(void()) {
    static_assert(std::is_same<T, C>::value, "");
}
template<typename C>
template<typename T>
auto CallbackWrapper<C>::cursorPosCallback(T& callbacks, GLFWwindow* window, double xpos, double ypos, long l)
-> decltype(void()) {
    static_assert(std::is_same<T, C>::value, "");
}
template<typename C>
template<typename T>
auto CallbackWrapper<C>::scrollCallback(T& callbacks, GLFWwindow* window, double xoffset, double yoffset, long l)
-> decltype(void()) {
    static_assert(std::is_same<T, C>::value, "");
}
template<typename C>
template<typename T>
auto CallbackWrapper<C>::charCallback(T& callbacks, GLFWwindow* window, unsigned codepoint, long l)
-> decltype(void()) {
    static_assert(std::is_same<T, C>::value, "");
}

} // namespace sim
