#include <sim-driver/OpenGLSimulation.hpp>
#include <sim-driver/renderers/RendererHelper.hpp>
#include <sim-driver/renderers/MeshRenderer.hpp>

class Simulator : public sim::OpenGLSimulation<Simulator>
{
public:
    Simulator()
        : renderer_{sim::MeshVariant{sim::SphereMesh()}}
    {
        camera_.setUsingOrbitMode(true);
        camera_.setOrbitPoint({0, 0, 0});
        camera_.setOrbitOffsetDistance(5);

        prevCam_ = camera_;
    }

    void onUpdate(double worldTime, double timeStep)
    {
        prevCam_ = camera_;
        camera_.yaw(static_cast<float>(timeStep));
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

        renderer_.onRender(a, camera);
    }

    void onGuiRender(int width, int height)
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        if (ImGui::Begin("Window", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Framerate: %.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
            renderer_.onGuiRender();
        }
        ImGui::End();
    }

private:
    sim::MeshRenderer renderer_;
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