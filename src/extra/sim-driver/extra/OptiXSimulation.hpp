#pragma once

#include <sim-driver/SimDriver.hpp>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <sim-driver/OpenGLHelper.hpp>
#include <optixu/optixpp_namespace.h>

namespace sim
{

template<typename Child>
class OptiXSimulation : public SimDriver<OptiXSimulation<Child>>
{
public:
    template<class T = Child>
    auto update(double world_time, double time_step, int i)
    -> decltype(static_cast<T *>(this)->onUpdate(world_time, time_step), void());

    template<class T = Child>
    auto update(double world_time, double time_step, long l) -> decltype(void());

    void render(int width, int height, double alpha, bool eventDriven);

    OptiXSimulation(const OptiXSimulation &) = delete;
    OptiXSimulation &operator=(const OptiXSimulation &) = delete;

protected:
    explicit OptiXSimulation(const std::string &title = "OpenGL Simulation",
                              int width = 0,
                              int height = 0);

    ~OptiXSimulation() override = default;

    OptiXSimulation(OptiXSimulation &&) = default; // no-except doesn't compile with llvm
    OptiXSimulation &operator=(OptiXSimulation &&) = default; // no-except doesn't compile with llvm

private:
    template<class T = Child>
    auto render_child(int width, int height, double alpha, int i)
    -> decltype(static_cast<T *>(this)->onRender(width, height, alpha), void());

    template<class T = Child>
    auto render_child(int width, int height, double alpha, long l) -> decltype(void());

    template<class T = Child>
    auto render_child_gui(int width, int height, int i)
    -> decltype(static_cast<T *>(this)->onGuiRender(width, height), void());

    template<class T = Child>
    auto render_child_gui(int width, int height, long l) -> decltype(void());
};


template<typename Child>
OptiXSimulation<Child>::OptiXSimulation(const std::string &title, int width, int height)
        : SimDriver<OptiXSimulation>(title, width, height)
{
    sim::OpenGLHelper::setDefaults();
}

template<typename Child>
template<typename T>
auto OptiXSimulation<Child>::update(double world_time, double time_step, int i)
-> decltype(static_cast<T *>(this)->onUpdate(world_time, time_step), void())
{
    static_assert(std::is_same<T, Child>::value, "");
    static_cast<Child *>(this)->onUpdate(world_time, time_step);
}

template<typename Child>
template<typename T>
auto OptiXSimulation<Child>::update(double world_time, double time_step, long l) -> decltype(void())
{
    static_assert(std::is_same<T, Child>::value, "");
}


template<typename Child>
void OptiXSimulation<Child>::render(const int width,
                                     const int height,
                                     const double alpha,
                                     const bool eventDriven)
{
    glViewport(0, 0, width, height);

    if (eventDriven)
    {
        ImGui_ImplGlfwGL3_NewFrame();
        render_child_gui(width, height, 0);
        ImGui::Render();
    }

    ImGui_ImplGlfwGL3_NewFrame();
    render_child_gui(width, height, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    render_child(width, height, alpha, 0);
    ImGui::Render();
}

template<typename Child>
template<typename T>
auto OptiXSimulation<Child>::render_child(int width, int height, double alpha, int i)
-> decltype(static_cast<T *>(this)->onRender(width, height, alpha), void())
{
    static_assert(std::is_same<T, Child>::value, "");
    static_cast<Child *>(this)->onRender(width, height, alpha);
}

template<typename Child>
template<typename T>
auto OptiXSimulation<Child>::render_child(int width, int height, double alpha, long l) -> decltype(void())
{
    static_assert(std::is_same<T, Child>::value, "");
}

template<typename Child>
template<typename T>
auto OptiXSimulation<Child>::render_child_gui(int width, int height, int i)
-> decltype(static_cast<T *>(this)->onGuiRender(width, height), void())
{
    static_assert(std::is_same<T, Child>::value, "");
    static_cast<Child *>(this)->onGuiRender(width, height);
}

template<typename Child>
template<typename T>
auto OptiXSimulation<Child>::render_child_gui(int width, int height, long l) -> decltype(void())
{
    static_assert(std::is_same<T, Child>::value, "");
}

} // namespace sim
