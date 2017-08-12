#pragma once

#include <sim-driver/SimDriver.hpp>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <memory>
#include <iostream>

namespace sim
{
template<typename Child>
class SimDriver;

template<typename T>
class Callbacks
{
public:

    Callbacks(SimDriver<T> &simDriver);

    void windowSizeCallback(GLFWwindow *pWindow, int width, int height);
    void windowFocusCallback(GLFWwindow *pWindow, int focus);

    void mouseButtonCallback(GLFWwindow *pWindow, int button, int action, int mods);
    void keyCallback(GLFWwindow *pWindow, int key, int scancode, int action, int mods);
    void cursorPosCallback(GLFWwindow *pWindow, double xpos, double ypos);
    void scrollCallback(GLFWwindow *pWindow, double xoffset, double yoffset);
    void charCallback(GLFWwindow *pWindow, unsigned codepoint);

    void windowRefreshCallback(GLFWwindow *pWindow);

private:
    SimDriver<T> &simDriver_;
};


template<typename T>
Callbacks<T>::Callbacks(SimDriver<T> &simDriver)
    : simDriver_(simDriver)
{}

template<typename T>
void Callbacks<T>::windowSizeCallback(GLFWwindow *pWindow, int width, int height)
{}

template<typename T>
void Callbacks<T>::windowFocusCallback(GLFWwindow *pWindow, int focus)
{}

template<typename T>
void Callbacks<T>::mouseButtonCallback(GLFWwindow *pWindow, int button, int action, int mods)
{}

template<typename T>
void Callbacks<T>::keyCallback(GLFWwindow *pWindow, int key, int scancode, int action, int mods)
{}

template<typename T>
void Callbacks<T>::cursorPosCallback(GLFWwindow *pWindow, double xpos, double ypos)
{}

template<typename T>
void Callbacks<T>::scrollCallback(GLFWwindow *pWindow, double xoffset, double yoffset)
{}

template<typename T>
void Callbacks<T>::charCallback(GLFWwindow *pWindow, unsigned codepoint)
{}

template<typename T>
void Callbacks<T>::windowRefreshCallback(GLFWwindow *pWindow)
{}


} // namespace sim
