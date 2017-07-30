#pragma once

struct GLFWwindow;
typedef GLFWwindow GLFWwindow;

namespace sim
{

class Callbacks
{
public:

    void windowSizeCallback(GLFWwindow *pWindow, int width, int height);
    void windowFocusCallback(GLFWwindow *pWindow, int focus);

    void mouseButtonCallback(GLFWwindow *pWindow, int button, int action, int mods);
    void keyCallback(GLFWwindow *pWindow, int key, int scancode, int action, int mods);
    void cursorPosCallback(GLFWwindow *pWindow, double xpos, double ypos);
    void scrollCallback(GLFWwindow *pWindow, double yoffset, double xoffset);
    void charCallback(GLFWwindow *pWindow, unsigned codepoint);

    void windowRefreshCallback(GLFWwindow *pWindow);
};


} // namespace sim
