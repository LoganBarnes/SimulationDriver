#pragma once

#include <sim-driver/SimDriver.hpp>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include "OpenGLHelper.hpp"

namespace sim
{

template<typename Child>
class OpenGLSimulation : public SimDriver<OpenGLSimulation<Child>>
{
public:
    template<class T = Child>
    auto update(double world_time, double time_step, int i)
    -> decltype(static_cast<T *>(this)->onUpdate(world_time, time_step), void());

    template<class T = Child>
    auto update(double world_time, double time_step, long l) -> decltype(void());

    void render(int width, int height, double alpha, bool eventDriven);

    OpenGLSimulation(const OpenGLSimulation &) = delete;
    OpenGLSimulation &operator=(const OpenGLSimulation &) = delete;
protected:
    explicit OpenGLSimulation(const std::string &title = "OpenGL Simulation",
                              int width = 0,
                              int height = 0);
    ~OpenGLSimulation() override = default;
    OpenGLSimulation(OpenGLSimulation &&) noexcept = default;
    OpenGLSimulation &operator=(OpenGLSimulation &&) noexcept = default;

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
OpenGLSimulation<Child>::OpenGLSimulation(const std::string &title, int width, int height)
    : SimDriver<OpenGLSimulation>(title, width, height)
{
    sim::OpenGLHelper::setDefaults();
}

template<typename Child>
template<typename T>
auto OpenGLSimulation<Child>::update(double world_time, double time_step, int i)
-> decltype(static_cast<T *>(this)->onUpdate(world_time, time_step), void())
{
    static_assert(std::is_same<T, Child>::value, "");
    static_cast<Child *>(this)->onUpdate(world_time, time_step);
}

template<typename Child>
template<typename T>
auto OpenGLSimulation<Child>::update(double world_time, double time_step, long l) -> decltype(void())
{
    static_assert(std::is_same<T, Child>::value, "");
}


template<typename Child>
void OpenGLSimulation<Child>::render(const int width,
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
auto OpenGLSimulation<Child>::render_child(int width, int height, double alpha, int i)
-> decltype(static_cast<T *>(this)->onRender(width, height, alpha), void())
{
    static_assert(std::is_same<T, Child>::value, "");
    static_cast<Child *>(this)->onRender(width, height, alpha);
}

template<typename Child>
template<typename T>
auto OpenGLSimulation<Child>::render_child(int width, int height, double alpha, long l) -> decltype(void())
{
    static_assert(std::is_same<T, Child>::value, "");
}

template<typename Child>
template<typename T>
auto OpenGLSimulation<Child>::render_child_gui(int width, int height, int i)
-> decltype(static_cast<T *>(this)->onGuiRender(width, height), void())
{
    static_assert(std::is_same<T, Child>::value, "");
    static_cast<Child *>(this)->onGuiRender(width, height);
}

template<typename Child>
template<typename T>
auto OpenGLSimulation<Child>::render_child_gui(int width, int height, long l) -> decltype(void())
{
    static_assert(std::is_same<T, Child>::value, "");
}

} // namespace sim