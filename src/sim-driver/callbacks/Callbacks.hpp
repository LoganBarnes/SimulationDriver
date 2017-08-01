#pragma once

#include <sim-driver/SimDriver.hpp>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <memory>
#include <iostream>

//struct GLFWwindow;
//typedef GLFWwindow GLFWwindow;

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
{
    ImGuiIO &io = ImGui::GetIO();

    if (!io.WantCaptureMouse)
    {}
    else
    {
        if (action == GLFW_PRESS && button >= 0 && button < 3)
        {
            io.MouseDown[button] = true;
        }
    }
}

template<typename T>
void Callbacks<T>::keyCallback(GLFWwindow *pWindow, int key, int scancode, int action, int mods)
{
    ImGuiIO &io = ImGui::GetIO();

    if (!io.WantCaptureKeyboard)
    {
        switch (key)
        {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(pWindow, GLFW_TRUE);
                break;
            default:
                break;
        }
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
}

template<typename T>
void Callbacks<T>::cursorPosCallback(GLFWwindow *pWindow, double xpos, double ypos)
{
    ImGuiIO &io = ImGui::GetIO();

    if (!io.WantCaptureMouse)
    {}
}

template<typename T>
void Callbacks<T>::scrollCallback(GLFWwindow *pWindow, double xoffset, double yoffset)
{
    ImGuiIO &io = ImGui::GetIO();

    if (!io.WantCaptureMouse)
    {}
    else
    {
        io.MouseWheel += static_cast<float>(yoffset); // the fractional mouse wheel. 1.0 unit 5 lines
    }
}

template<typename T>
void Callbacks<T>::charCallback(GLFWwindow *pWindow, unsigned codepoint)
{
    ImGuiIO &io = ImGui::GetIO();

    if (io.WantCaptureKeyboard && codepoint > 0 && codepoint < 0x10000)
    {
        io.AddInputCharacter(static_cast<unsigned short>(codepoint));
    }
}

template<typename T>
void Callbacks<T>::windowRefreshCallback(GLFWwindow *pWindow)
{}


} // namespace sim
