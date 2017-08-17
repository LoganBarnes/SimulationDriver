#pragma once

#include <sim-driver/SimDriver.hpp>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include "OpenGLHelper.hpp"

namespace sim
{

template<typename T, typename ... Args>
auto make_child(int i, const std::shared_ptr<SimData> &simData, Args ...args)
-> decltype(T(simData, args...), std::shared_ptr<T>())
{
    return std::make_shared<T>(simData, args...);
}

template<typename T, typename ... Args>
auto make_child(long i, const std::shared_ptr<SimData> &simData, Args ...args)
-> decltype(T(args...), std::shared_ptr<T>())
{
    return std::make_shared<T>(args...);
}

template<typename Child>
class OpenGLSimulation : public SimDriver<OpenGLSimulation<Child>>
{
public:
    template<typename ... Args>
    explicit OpenGLSimulation(const SimInitData &initData = {}, Args ...args);

    void update(double worldTime, double timeStep);

    void render(int width, int height, double alpha, bool eventDriven);

private:

    std::shared_ptr<Child> spChild_;
    SimCallbacks<Child> callbacks_;

    template<class T = Child>
    auto updateChild(T &child, double worldTime, double timeStep, int i)
    -> decltype(child.onUpdate(worldTime, timeStep), void());

    template<class T = Child>
    auto updateChild(T &child, double worldTime, double timeStep, long l) -> decltype(void());

    template<class T = Child>
    auto renderChild(T &child, int width, int height, double alpha, int i)
    -> decltype(child.onRender(width, height, alpha), void());

    template<class T = Child>
    auto renderChild(T &child, int width, int height, double alpha, long l) -> decltype(void());

    template<class T = Child>
    auto renderChildGui(T &child, int width, int height, int i)
    -> decltype(child.onGuiRender(width, height), void());

    template<class T = Child>
    auto renderChildGui(T &child, int width, int height, long l) -> decltype(void());
};


template<typename Child>
template<typename ... Args>
OpenGLSimulation<Child>::OpenGLSimulation(const SimInitData &initData, Args ...args)
        : SimDriver<OpenGLSimulation<Child>>(initData),
          spChild_{make_child<Child>(0, this->getSimData(), args...)},
          callbacks_{this->getSimData(), spChild_}
{
    sim::OpenGLHelper::setDefaults();
    this->setCallbackClass(&callbacks_);
}

template<typename Child>
void OpenGLSimulation<Child>::update(const double worldTime, const double timeStep)
{
    updateChild(*spChild_, worldTime, timeStep, 0);
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
        renderChildGui(*spChild_, width, height, 0);
        ImGui::Render();
    }

    ImGui_ImplGlfwGL3_NewFrame();
    renderChildGui(*spChild_, width, height, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderChild(*spChild_, width, height, alpha, 0);
    ImGui::Render();
}

template<typename Child>
template<typename T>
auto OpenGLSimulation<Child>::updateChild(T &child, double worldTime, double timeStep, int i)
-> decltype(child.onUpdate(worldTime, timeStep), void())
{
    static_assert(std::is_same<T, Child>::value, "");
    child.onUpdate(worldTime, timeStep);
}

template<typename Child>
template<typename T>
auto OpenGLSimulation<Child>::updateChild(T &child, double worldTime, double timeStep, long l) -> decltype(void())
{
    static_assert(std::is_same<T, Child>::value, "");
}

template<typename Child>
template<typename T>
auto OpenGLSimulation<Child>::renderChild(T &child, int width, int height, double alpha, int i)
-> decltype(child.onRender(width, height, alpha), void())
{
    static_assert(std::is_same<T, Child>::value, "");
    child.onRender(width, height, alpha);
}

template<typename Child>
template<typename T>
auto OpenGLSimulation<Child>::renderChild(T &child, int width, int height, double alpha, long l) -> decltype(void())
{
    static_assert(std::is_same<T, Child>::value, "");
}

template<typename Child>
template<typename T>
auto OpenGLSimulation<Child>::renderChildGui(T &child, int width, int height, int i)
-> decltype(child.onGuiRender(width, height), void())
{
    static_assert(std::is_same<T, Child>::value, "");
    child.onGuiRender(width, height);
}

template<typename Child>
template<typename T>
auto OpenGLSimulation<Child>::renderChildGui(T &child, int width, int height, long l) -> decltype(void())
{
    static_assert(std::is_same<T, Child>::value, "");
}

} // namespace sim