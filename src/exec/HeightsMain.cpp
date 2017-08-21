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


class Simulator
{
public:
    Simulator(int, int, sim::SimData *pSimData)
            : renderer_{sim::HeightMap{3,
                                       2,
                                       glm::vec3{-1.5f, -1.0f, -0.5f},
                                       glm::vec3{3, 2, 1},
                                       std::vector<float>{0.2f, 0.7f,
                                                          0.3f, 0.6f,
                                                          0.4f, 0.5f}}},
              simData_{*pSimData}
    {
        simData_.camera.setUsingOrbitMode(true);
        simData_.camera.setOrbitOrigin({0, 0, 0});
        simData_.camera.setOrbitOffsetDistance(5);
        simData_.camera.pitch(glm::half_pi<float>() * -0.15f);

        prevCam_ = simData_.camera;
    }

    void onUpdate(double, double timeStep)
    {
        prevCam_ = simData_.camera;
        simData_.camera.yaw(static_cast<float>(timeStep * 0.5));
    }

    void onRender(int, int, double alpha)
    {
        auto a = static_cast<float>(alpha);

        sim::Camera camera;
        glm::vec3 eye{glm::mix(simData_.camera.getEyeVector(), prevCam_.getEyeVector(), a)};
        glm::vec3 look{glm::mix(simData_.camera.getLookVector(), prevCam_.getLookVector(), a)};
        glm::vec3 up{glm::mix(simData_.camera.getUpVector(), prevCam_.getUpVector(), a)};

        float aspect{glm::mix(simData_.camera.getAspectRatio(), prevCam_.getAspectRatio(), a)};

        camera.lookAt(eye, eye + look, up);
        camera.setAspectRatio(aspect);

        renderer_.render(a, camera);
    }

    void onGuiRender(int, int)
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

    sim::SimData &simData_;
};

int main()
{
    try
    {
        sim::OpenGLSimulation<Simulator>{{"Heights Sim"}}.runNoFasterThanRealTimeLoop();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Program failed: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}