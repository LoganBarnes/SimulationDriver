#pragma once
#include <memory>

struct GLFWwindow;
typedef GLFWwindow GLFWwindow;

namespace sim
{

template<typename C>
class CallbackSingleton
{

public:
    CallbackSingleton(const CallbackSingleton &) = delete;
    void operator=(const CallbackSingleton &)    = delete;

    CallbackSingleton(CallbackSingleton &&) noexcept = delete;
    void operator=(CallbackSingleton &&) noexcept    = delete;

    // Singleton is accessed via getInstance()
    static CallbackSingleton &getInstance();

    // basic static callback functions
    static void errorCallback(int error, const char *description);
    static void windowSizeCallback(GLFWwindow *pWindow, int width, int height);
    static void windowFocusCallback(GLFWwindow *pWindow, int focus);

    // input static callback functions
    static void mouseButtonCallback(GLFWwindow *pWindow, int button, int action, int mods);
    static void keyCallback(GLFWwindow *pWindow, int key, int scancode, int action, int mods);
    static void cursorPositionCallback(GLFWwindow *pWindow, double xpos, double ypos);
    static void scrollCallback(GLFWwindow *pWindow, double xoffset, double yoffset);
    static void charCallback(GLFWwindow *pWindow, unsigned codepoint);

    static void windowRefreshCallback(GLFWwindow *pWindow);


    // the actual implementations of the callback methods
    void defaultErrorCallback(int error, const char *description);
    void defaultWindowSizeCallback(GLFWwindow *pWindow, int width, int height);
    void defaultWindowFocusCallback(GLFWwindow *pWindow, int focus);

    void defaultMouseButtonCallback(GLFWwindow *pWindow, int button, int action, int mods);
    void defaultKeyCallback(GLFWwindow *pWindow, int key, int scancode, int action, int mods);
    void defaultCursorPositionCallback(GLFWwindow *pWindow, double xpos, double ypos);
    void defaultScrollCallback(GLFWwindow *pWindow, double xoffset, double yoffset);
    void defaultCharCallback(GLFWwindow *pWindow, unsigned codepoint);

    void defaultWindowRefreshCallback(GLFWwindow *pWindow);

    void setCallbackClass(std::unique_ptr<C> upCallback);

private:

    std::unique_ptr<C> upCallbacks_{nullptr};

    // private constructor necessary to allow only 1 instance
    CallbackSingleton() = default;
    virtual ~CallbackSingleton() = default;
};

} // namespace sim
