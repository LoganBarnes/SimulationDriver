#include <sim-driver/OpenGLSimulation.hpp>
#include <sim-driver/OpenGLHelper.hpp>
#include <sim-driver/renderers/RendererHelper.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <sim-driver/renderers/MeshRenderer.hpp>
#include <sim-driver/renderers/HeightMapRenderer.hpp>


class Simulator : public sim::OpenGLSimulation<Simulator>
{
public:
    Simulator() : renderer_{sim::HeightMap{3,
                                           2,
                                           glm::vec3{-1.5f, 0.0f, -0.5f},
                                           glm::vec3{3, 2, 1},
                                           std::vector<float>{0.2f, 0.7f,
                                                              0.3f, 0.6f,
                                                              0.4f, 0.5f}}}
    {
        camera_.setUsingOrbitMode(true);
        camera_.setOrbitPoint({0, 0, 0});
        camera_.setOrbitOffsetDistance(5);
        camera_.pitch(glm::half_pi<float>() * -0.15f);

        prevCam_ = camera_;
    }

    void onUpdate(double worldTime, double timeStep)
    {
        prevCam_ = camera_;
        camera_.yaw(static_cast<float>(timeStep * 0.5));
    }

    void onRender(int width, int height, double alpha)
    {
        auto a = static_cast<float>(alpha);

        sim::Camera camera;
        glm::vec3 eye{glm::mix(camera_.getEyeVector(), prevCam_.getEyeVector(), a)};
        glm::vec3 look{glm::mix(camera_.getLookVector(), prevCam_.getLookVector(), a)};
        glm::vec3 up{glm::mix(camera_.getUpVector(), prevCam_.getUpVector(), a)};

        float aspect{glm::mix(camera_.getAspectRatio(), prevCam_.getAspectRatio(), a)};

        camera.lookAt(eye, eye + look, up);
        camera.setAspectRatio(aspect);

        renderer_.render(a, camera);
    }

    void onGuiRender(int width, int height)
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        if (ImGui::Begin("Window", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Framerate: %.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
            renderer_.configureGui();
        }
        ImGui::End();
    }

private:
    sim::HeightMapRenderer renderer_;
    sim::Camera prevCam_;
};

int main()
{
    try
    {
        Simulator sim;
        sim.runNoFasterThanRealTimeLoop();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Program failed: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}