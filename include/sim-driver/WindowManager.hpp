#pragma once

#include <functional>
#include <memory>
#include <vector>

struct GLFWwindow;

typedef GLFWwindow GLFWwindow;

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
