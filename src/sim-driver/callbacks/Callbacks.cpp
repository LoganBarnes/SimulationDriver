#include <sim-driver/callbacks/Callbacks.hpp>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <imgui.h>

namespace sim
{

void Callbacks::windowSizeCallback(GLFWwindow *pWindow, int width, int height)
{}

void Callbacks::windowFocusCallback(GLFWwindow *pWindow, int focus)
{}

void Callbacks::mouseButtonCallback(GLFWwindow *pWindow, int button, int action, int mods)
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

void Callbacks::keyCallback(GLFWwindow *pWindow, int key, int scancode, int action, int mods)
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

void Callbacks::cursorPosCallback(GLFWwindow *pWindow, double xpos, double ypos)
{
    ImGuiIO &io = ImGui::GetIO();

    if (!io.WantCaptureMouse)
    {}
}

void Callbacks::scrollCallback(GLFWwindow *pWindow, double yoffset, double xoffset)
{
    ImGuiIO &io = ImGui::GetIO();

    if (!io.WantCaptureMouse)
    {}
    else
    {
        io.MouseWheel += static_cast<float>(yoffset); // the fractional mouse wheel. 1.0 unit 5 lines
    }
}

void Callbacks::charCallback(GLFWwindow *pWindow, unsigned codepoint)
{
    ImGuiIO &io = ImGui::GetIO();

    if (io.WantCaptureKeyboard && codepoint > 0 && codepoint < 0x10000)
    {
        io.AddInputCharacter(static_cast<unsigned short>(codepoint));
    }
}

void Callbacks::windowRefreshCallback(GLFWwindow *pWindow)
{}


} // namespace sim
