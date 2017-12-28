#pragma once

#include <sim-driver/SimDriver.hpp>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <memory>
#include <iostream>

namespace sim {

class EmptyCallbacks {};

template <typename C1 = EmptyCallbacks, typename C2 = EmptyCallbacks>
class CallbackWrapper {
public:
    CallbackWrapper(C1* pCallbacks, C2* pCallbacks2 = nullptr);

    void framebufferSizeCallback(GLFWwindow* pWindow, int width, int height);
    void windowFocusCallback(GLFWwindow* pWindow, int focus);

    void mouseButtonCallback(GLFWwindow* pWindow, int button, int action, int mods);
    void keyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods);
    void cursorPosCallback(GLFWwindow* pWindow, double xpos, double ypos);
    void scrollCallback(GLFWwindow* pWindow, double xoffset, double yoffset);
    void charCallback(GLFWwindow* pWindow, unsigned codepoint);

private:
    C1* pCallbacks1_;
    C2* pCallbacks2_;

    template <typename C>
    auto framebufferSizeCallback(C& callbacks, GLFWwindow* window, int width, int height, int i)
        -> decltype(callbacks.framebufferSizeCallback(window, width, height), void());
    template <typename C>
    auto windowFocusCallback(C& callbacks, GLFWwindow* window, int focus, int i)
        -> decltype(callbacks.windowFocusCallback(window, focus), void());
    template <typename C>
    auto mouseButtonCallback(C& callbacks, GLFWwindow* window, int button, int action, int mods, int i)
        -> decltype(callbacks.mouseButtonCallback(window, button, action, mods), void());
    template <typename C>
    auto keyCallback(C& callbacks, GLFWwindow* window, int key, int scancode, int action, int mods, int i)
        -> decltype(callbacks.keyCallback(window, key, scancode, action, mods), void());
    template <typename C>
    auto cursorPosCallback(C& callbacks, GLFWwindow* window, double xpos, double ypos, int i)
        -> decltype(callbacks.cursorPosCallback(window, xpos, ypos), void());
    template <typename C>
    auto scrollCallback(C& callbacks, GLFWwindow* window, double xoffset, double yoffset, int i)
        -> decltype(callbacks.scrollCallback(window, xoffset, yoffset), void());
    template <typename C>
    auto charCallback(C& callbacks, GLFWwindow* window, unsigned codepoint, int i)
        -> decltype(callbacks.charCallback(window, codepoint), void());

    template <typename C>
    auto framebufferSizeCallback(C& callbacks, GLFWwindow* window, int width, int height, long l) -> decltype(void());
    template <typename C>
    auto windowFocusCallback(C& callbacks, GLFWwindow* window, int focus, long l) -> decltype(void());
    template <typename C>
    auto mouseButtonCallback(C& callbacks, GLFWwindow* window, int button, int action, int mods, long l)
        -> decltype(void());
    template <typename C>
    auto keyCallback(C& callbacks, GLFWwindow* window, int key, int scancode, int action, int mods, long l)
        -> decltype(void());
    template <typename C>
    auto cursorPosCallback(C& callbacks, GLFWwindow* window, double xpos, double ypos, long l) -> decltype(void());
    template <typename C>
    auto scrollCallback(C& callbacks, GLFWwindow* window, double xoffset, double yoffset, long l) -> decltype(void());
    template <typename C>
    auto charCallback(C& callbacks, GLFWwindow* window, unsigned codepoint, long l) -> decltype(void());
};

template <typename C1, typename C2>
CallbackWrapper<C1, C2>::CallbackWrapper(C1* pCallbacks1, C2* pCallbacks2)
    : pCallbacks1_(pCallbacks1), pCallbacks2_{pCallbacks2} {}

template <typename C1, typename C2>
void CallbackWrapper<C1, C2>::framebufferSizeCallback(GLFWwindow* pWindow, int width, int height) {
    framebufferSizeCallback(*pCallbacks1_, pWindow, width, height, 0);
    framebufferSizeCallback(*pCallbacks2_, pWindow, width, height, 0);
}
template <typename C1, typename C2>
void CallbackWrapper<C1, C2>::windowFocusCallback(GLFWwindow* pWindow, int focus) {
    windowFocusCallback(*pCallbacks1_, pWindow, focus, 0);
    windowFocusCallback(*pCallbacks2_, pWindow, focus, 0);
}
template <typename C1, typename C2>
void CallbackWrapper<C1, C2>::mouseButtonCallback(GLFWwindow* pWindow, int button, int action, int mods) {
    mouseButtonCallback(*pCallbacks1_, pWindow, button, action, mods, 0);
    mouseButtonCallback(*pCallbacks2_, pWindow, button, action, mods, 0);
}
template <typename C1, typename C2>
void CallbackWrapper<C1, C2>::keyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods) {
    keyCallback(*pCallbacks1_, pWindow, key, scancode, action, mods, 0);
    keyCallback(*pCallbacks2_, pWindow, key, scancode, action, mods, 0);
}
template <typename C1, typename C2>
void CallbackWrapper<C1, C2>::cursorPosCallback(GLFWwindow* pWindow, double xpos, double ypos) {
    cursorPosCallback(*pCallbacks1_, pWindow, xpos, ypos, 0);
    cursorPosCallback(*pCallbacks2_, pWindow, xpos, ypos, 0);
}
template <typename C1, typename C2>
void CallbackWrapper<C1, C2>::scrollCallback(GLFWwindow* pWindow, double xoffset, double yoffset) {
    scrollCallback(*pCallbacks1_, pWindow, xoffset, yoffset, 0);
    scrollCallback(*pCallbacks2_, pWindow, xoffset, yoffset, 0);
}
template <typename C1, typename C2>
void CallbackWrapper<C1, C2>::charCallback(GLFWwindow* pWindow, unsigned codepoint) {
    charCallback(*pCallbacks1_, pWindow, codepoint, 0);
    charCallback(*pCallbacks2_, pWindow, codepoint, 0);
}

/////////////////////////////////////// Private implementation functions ///////////////////////////////////////

template <typename C1, typename C2>
template <typename C>
auto CallbackWrapper<C1, C2>::framebufferSizeCallback(C& callbacks, GLFWwindow* window, int width, int height, int)
    -> decltype(callbacks.framebufferSizeCallback(window, width, height), void()) {
    callbacks.framebufferSizeCallback(window, width, height);
}

template <typename C1, typename C2>
template <typename C>
auto CallbackWrapper<C1, C2>::windowFocusCallback(C& callbacks, GLFWwindow* window, int focus, int)
    -> decltype(callbacks.windowFocusCallback(window, focus), void()) {
    callbacks.windowFocusCallback(window, focus);
}

template <typename C1, typename C2>
template <typename C>
auto CallbackWrapper<C1, C2>::mouseButtonCallback(
    C& callbacks, GLFWwindow* window, int button, int action, int mods, int)
    -> decltype(callbacks.mouseButtonCallback(window, button, action, mods), void()) {
    callbacks.mouseButtonCallback(window, button, action, mods);
}

template <typename C1, typename C2>
template <typename C>
auto CallbackWrapper<C1, C2>::keyCallback(
    C& callbacks, GLFWwindow* window, int key, int scancode, int action, int mods, int)
    -> decltype(callbacks.keyCallback(window, key, scancode, action, mods), void()) {
    callbacks.keyCallback(window, key, scancode, action, mods);
}

template <typename C1, typename C2>
template <typename C>
auto CallbackWrapper<C1, C2>::cursorPosCallback(C& callbacks, GLFWwindow* window, double xpos, double ypos, int)
    -> decltype(callbacks.cursorPosCallback(window, xpos, ypos), void()) {
    callbacks.cursorPosCallback(window, xpos, ypos);
}

template <typename C1, typename C2>
template <typename C>
auto CallbackWrapper<C1, C2>::scrollCallback(C& callbacks, GLFWwindow* window, double xoffset, double yoffset, int)
    -> decltype(callbacks.scrollCallback(window, xoffset, yoffset), void()) {
    callbacks.scrollCallback(window, xoffset, yoffset);
}

template <typename C1, typename C2>
template <typename C>
auto CallbackWrapper<C1, C2>::charCallback(C& callbacks, GLFWwindow* window, unsigned codepoint, int)
    -> decltype(callbacks.charCallback(window, codepoint), void()) {
    callbacks.charCallback(window, codepoint);
}

/////////////////////////////////////// Empty implementation functions ///////////////////////////////////////

template <typename C1, typename C2>
template <typename C>
auto CallbackWrapper<C1, C2>::framebufferSizeCallback(C&, GLFWwindow*, int, int, long) -> decltype(void()) {}
template <typename C1, typename C2>
template <typename C>
auto CallbackWrapper<C1, C2>::windowFocusCallback(C&, GLFWwindow*, int, long) -> decltype(void()) {}
template <typename C1, typename C2>
template <typename C>
auto CallbackWrapper<C1, C2>::mouseButtonCallback(C&, GLFWwindow*, int, int, int, long) -> decltype(void()) {}
template <typename C1, typename C2>
template <typename C>
auto CallbackWrapper<C1, C2>::keyCallback(C&, GLFWwindow*, int, int, int, int, long) -> decltype(void()) {}
template <typename C1, typename C2>
template <typename C>
auto CallbackWrapper<C1, C2>::cursorPosCallback(C&, GLFWwindow*, double, double, long) -> decltype(void()) {}
template <typename C1, typename C2>
template <typename C>
auto CallbackWrapper<C1, C2>::scrollCallback(C&, GLFWwindow*, double, double, long) -> decltype(void()) {}
template <typename C1, typename C2>
template <typename C>
auto CallbackWrapper<C1, C2>::charCallback(C&, GLFWwindow*, unsigned, long) -> decltype(void()) {}

} // namespace sim
