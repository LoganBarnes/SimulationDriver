#pragma once
#define GLFW_INCLUDE_NONE

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifdef OFFSCREEN_MESA
#define GLFW_EXPOSE_NATIVE_OSMESA
#include <GLFW/glfw3native.h>
#endif

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#include <iostream>

#include <functional>
#include <memory>
#include <vector>
#include <string>

namespace sim
{

class WindowManager
{
public:
    static WindowManager &instance();

    ~WindowManager() = default;

    WindowManager(const WindowManager &) = delete;
    WindowManager(WindowManager &&) noexcept = delete;
    WindowManager &operator=(const WindowManager &) = delete;
    WindowManager &operator=(WindowManager &&) noexcept = delete;

    int create_window(const std::string &title = "Window",
                      int width = 0,
                      int height = 0,
                      int samples = 4,
                      bool resizable = true);

    GLFWwindow *get_window(int index) const;

private:
    WindowManager();
    std::unique_ptr<int, std::function<void(int *)>> up_glfw_{nullptr};

    std::vector<std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow *)>>> windows_;

    std::unique_ptr<bool, std::function<void(bool *)>> up_imgui_{nullptr};
};

} // namespace sim
