#include <sim-driver/WindowManager.hpp>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <iostream>

namespace sim
{

WindowManager &WindowManager::instance()
{
    static WindowManager manager;
    return manager;
}

WindowManager::WindowManager()
{
    // Set the error callback before any other GLFW calls so we get proper error reporting
    glfwSetErrorCallback([](int error, const char *description)
                         {
                             std::cerr << "ERROR: (" << error << ") " << description << std::endl;
                         });

    up_glfw_ = std::unique_ptr<int, std::function<void(int *)>>(new int(glfwInit()),
                                                                [](auto p)
                                                                {
                                                                    glfwTerminate();
                                                                    delete p;
                                                                });

    if (*up_glfw_ == 0) {
        throw std::runtime_error("GLFW init failed");
    }
}

} // namespace sim
