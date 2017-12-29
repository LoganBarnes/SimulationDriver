#pragma once

#include <sim-driver/SimDriver.hpp>
#include <sim-driver/OpenGLHelper.hpp>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <memory>

namespace sim {

template <typename T, typename... Args>
auto make_child(priority_tag<2>, int w, int h, SimData* pSimData, Args... args)
    -> decltype(T(w, h, pSimData, args...), std::unique_ptr<T>())
{
    return std::make_unique<T>(w, h, pSimData, args...);
}

template <typename T, typename... Args>
auto make_child(priority_tag<1>, int w, int h, SimData*, Args... args)
    -> decltype(T(w, h, args...), std::unique_ptr<T>())
{
    return std::make_unique<T>(w, h, args...);
}

template <typename T, typename... Args>
auto make_child(priority_tag<0>, int, int, SimData*, Args... args) -> decltype(T(args...), std::unique_ptr<T>())
{
    return std::make_unique<T>(args...);
}

template <typename Child>
class OpenGLSimulation : public SimDriver<OpenGLSimulation<Child>>
{
public:
    OpenGLSimulation();

    template <typename... Args>
    explicit OpenGLSimulation(const SimInitData& initData, Args... args);

    void update(double worldTime, double timeStep);
    void render(int width, int height, double alpha, bool eventDriven);
    bool paused() const;

private:
    std::unique_ptr<Child> child_{nullptr};
    std::unique_ptr<SimCallbacks<Child>> callbacks_{nullptr};

    template <class T = Child>
    auto updateChild(T& child, double worldTime, double timeStep, int i)
        -> decltype(child.onUpdate(worldTime, timeStep), void());

    template <class T = Child>
    auto updateChild(T& child, double worldTime, double timeStep, long l) -> decltype(void());

    template <class T = Child>
    auto renderChild(T& child, int width, int height, double alpha, int i)
        -> decltype(child.onRender(width, height, alpha), void());

    template <class T = Child>
    auto renderChild(T& child, int width, int height, double alpha, long l) -> decltype(void());

    template <class T = Child>
    auto renderChildGui(T& child, int width, int height, int i) -> decltype(child.onGuiRender(width, height), void());

    template <class T = Child>
    auto renderChildGui(T& child, int width, int height, long l) -> decltype(void());
};

template <typename Child>
OpenGLSimulation<Child>::OpenGLSimulation() : OpenGLSimulation(SimInitData{})
{
}

template <typename Child>
template <typename... Args>
OpenGLSimulation<Child>::OpenGLSimulation(const SimInitData& initData, Args... args)
    : SimDriver<OpenGLSimulation<Child>>(initData)
{
    sim::OpenGLHelper::setDefaults();
    child_ = make_child<Child>(sim::priority_tag<2>{}, this->getWidth(), this->getHeight(), &this->simData, args...);

    callbacks_ = std::make_unique<SimCallbacks<Child>>(&this->simData, child_.get());
    this->setCallbackClass(callbacks_.get());
}

template <typename Child>
void OpenGLSimulation<Child>::update(const double worldTime, const double timeStep)
{
    updateChild(*child_, worldTime, timeStep, 0);
}

template <typename Child>
void OpenGLSimulation<Child>::render(const int width, const int height, const double alpha, const bool eventDriven)
{
    glViewport(0, 0, width, height);

    if (eventDriven) {
        ImGui_ImplGlfwGL3_NewFrame();
        renderChildGui(*child_, width, height, 0);
        ImGui::Render();
    }

    ImGui_ImplGlfwGL3_NewFrame();
    renderChildGui(*child_, width, height, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderChild(*child_, width, height, alpha, 0);
    ImGui::Render();
}

template <typename Child>
bool OpenGLSimulation<Child>::paused() const
{
    return this->simData.paused;
}

template <typename Child>
template <typename T>
auto OpenGLSimulation<Child>::updateChild(T& child, double worldTime, double timeStep, int)
    -> decltype(child.onUpdate(worldTime, timeStep), void())
{
    child.onUpdate(worldTime, timeStep);
}

template <typename Child>
template <typename T>
auto OpenGLSimulation<Child>::updateChild(T&, double, double, long) -> decltype(void())
{
}

template <typename Child>
template <typename T>
auto OpenGLSimulation<Child>::renderChild(T& child, int width, int height, double alpha, int)
    -> decltype(child.onRender(width, height, alpha), void())
{
    child.onRender(width, height, alpha);
}

template <typename Child>
template <typename T>
auto OpenGLSimulation<Child>::renderChild(T&, int, int, double, long) -> decltype(void())
{
}

template <typename Child>
template <typename T>
auto OpenGLSimulation<Child>::renderChildGui(T& child, int width, int height, int)
    -> decltype(child.onGuiRender(width, height), void())
{
    child.onGuiRender(width, height);
}

template <typename Child>
template <typename T>
auto OpenGLSimulation<Child>::renderChildGui(T&, int, int, long) -> decltype(void())
{
}

} // namespace sim