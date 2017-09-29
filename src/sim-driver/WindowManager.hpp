#pragma once

#include <functional>
#include <memory>

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
private:
    WindowManager();

    std::unique_ptr<int, std::function<void(int *)>> up_glfw_{nullptr};
};

} // namespace sim
