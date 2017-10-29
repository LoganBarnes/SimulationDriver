#include <sim-driver/WindowManager.hpp>

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
int WindowManager::create_window(const std::string &title, int width, int height, int samples, bool resizable)
{
    if (width == 0 || height == 0) {
        const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        width = mode->width;
        height = mode->height;
    }

    glfwWindowHint(GLFW_SAMPLES, samples);
    glfwWindowHint(GLFW_RESIZABLE, resizable);

    if (title.empty()) {
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1); // highest on mac :(
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif // __APPLE__

#ifdef OFFSCREEN_MESA
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_OSMESA_CONTEXT_API);
#endif

    auto up_window = std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow *)>>(
        glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr),
        [](auto p)
        {
            if (p) {
                glfwDestroyWindow(p);
            }
        });

    if (up_window == nullptr) {
        throw std::runtime_error("GLFW window creation failed");
    }

    glfwMakeContextCurrent(up_window.get());
    glfwSwapInterval(1);

    if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == 0) {
        throw std::runtime_error("Failed to initialize OpenGL context");
    }

    /// \todo: call ImGui_ImplGlfwGL3_Init on new windows and whenever window focus changes
    if (!up_imgui_) {
        up_imgui_ = std::unique_ptr<bool, std::function<void(bool *)>>(
            new bool(ImGui_ImplGlfwGL3_Init(up_window.get(), false)), // false for no callbacks
            [](auto p)
            {
                ImGui_ImplGlfwGL3_Shutdown();
                delete p;
            });
    }

    int index = windows_.size();
    windows_.emplace_back(std::move(up_window));

    return index;
}

GLFWwindow *WindowManager::get_window(int index) const
{
    return windows_.at(index).get();
}

} // namespace sim
