#include <sim-driver/extra/OptiXSimulation.hpp>
#include <sim-driver/extra/OptiXScene.hpp>

class Simulator
{
public:
    explicit Simulator(int, int, optix::Context &context, sim::SimData *pSimData)
            : simData_{*pSimData},
              scene_{context}
    {
        simData_.camera.setUsingOrbitMode(true);
        simData_.camera.setOrbitOrigin({0, 0, 0});
        simData_.camera.setOrbitOffsetDistance(5);

        prevCam_ = simData_.camera;
    }

    void onUpdate(double, double timeStep)
    {
        prevCam_ = simData_.camera;
        simData_.camera.yaw(static_cast<float>(timeStep));
    }

    void onRender(int width, int height, double alpha, optix::Context &context)
    {
        auto a = static_cast<float>(alpha);

        sim::Camera camera;
        glm::vec3 eye{glm::mix(prevCam_.getEyeVector(), simData_.camera.getEyeVector(), a)};
        glm::vec3 look{glm::mix(prevCam_.getLookVector(), simData_.camera.getLookVector(), a)};
        glm::vec3 up{glm::mix(prevCam_.getUpVector(), simData_.camera.getUpVector(), a)};

        float aspect{glm::mix(prevCam_.getAspectRatio(), simData_.camera.getAspectRatio(), a)};

        camera.lookAt(eye, eye + look, up);
        camera.setAspectRatio(aspect);

        context->launch(0, static_cast<unsigned>(width), static_cast<unsigned>(height));
    }

    void onGuiRender(int, int)
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        if (ImGui::Begin("Window", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Framerate: %.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
        }
        ImGui::End();
    }

    void keyCallback(GLFWwindow *, int, int, int, int mods)
    {
        simData_.paused = (mods == GLFW_MOD_SHIFT);
    }

private:
    sim::SimData &simData_;
    sim::Camera prevCam_;

    sim::OptiXScene scene_;
};

int main()
{
    try
    {
        sim::OptiXSimulation<Simulator>{{"OptiX Test"}}.runNoFasterThanRealTimeLoop();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Program failed: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}