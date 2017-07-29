#include <sim-driver/SimDriver.hpp>
#include <iostream>
#include <stdexcept>
#include <cstdlib>

namespace sim
{

template<typename Child>
class OpenGLSimulation : public SimDriver<OpenGLSimulation<Child>>
{
public:
    void update(double worldTime, double timeStep);
    void render(int width, int height, double alpha, bool eventDriven);

    OpenGLSimulation(const OpenGLSimulation &) = delete;
    OpenGLSimulation &operator=(const OpenGLSimulation &) = delete;
protected:
    OpenGLSimulation();
    ~OpenGLSimulation() override = default;
    OpenGLSimulation(OpenGLSimulation &&) noexcept = default;
    OpenGLSimulation &operator=(OpenGLSimulation &&) noexcept = default;
private:
};


template<typename Child>
OpenGLSimulation<Child>::OpenGLSimulation()
{
    glEnable(GL_PROGRAM_POINT_SIZE);
}

template<typename Child>
void OpenGLSimulation<Child>::update(const double worldTime, const double timeStep)
{
    static_cast<Child *>(this)->onUpdate(worldTime, timeStep);
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
        static_cast<Child *>(this)->onGuiRender(width, height);
        ImGui::Render();
    }

    ImGui_ImplGlfwGL3_NewFrame();
    static_cast<Child *>(this)->onGuiRender(width, height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static_cast<Child *>(this)->onRender(width, height, alpha);
    ImGui::Render();
}

} // namespace sim