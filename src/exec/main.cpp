#include <sim-driver/OpenGLSimulation.hpp>
#include <sim-driver/OpenGLHelper.hpp>
#include <sim-driver/renderers/RendererHelper.hpp>
#include <ShaderConfig.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stdexcept>
#include <cstdlib>

class Simulator : public sim::OpenGLSimulation<Simulator>
{
public:
    Simulator()
    {
        renderer_.setDrawMode(GL_TRIANGLE_FAN);

        camera_.setUsingOrbitMode(true);
        camera_.setOrbitPoint({0, 0, 0});
        camera_.setOrbitOffsetDistance(5);
    }

    void onUpdate(double worldTime, double timeStep)
    {
        camera_.yaw(static_cast<float>(timeStep * 0.1));
    }

    void onRender(int width, int height, double alpha)
    {
        renderer_.onRender(static_cast<float>(alpha), camera_);
    }

    void onGuiRender(int width, int height)
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        if (ImGui::Begin("Window", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            renderer_.onGuiRender();
        }
        ImGui::End();
    }

private:
    sim::RendererHelper<sim::PosNormTexVertex> renderer_;
};

int main()
{
    try
    {
        Simulator sim;
        sim.runEventLoop();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Program failed: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}