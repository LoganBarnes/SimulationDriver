#include <sim-driver/OpenGLSimulation.hpp>
#include <sim-driver/renderers/RendererHelper.hpp>
#include <sim-driver/renderers/MeshRenderer.hpp>
#include <sim-driver/meshes/MeshFunctions.hpp>


class Simulator
{
public:
    explicit Simulator(std::shared_ptr<sim::SimData> spSimData)
            : renderer_{sim::PosNormTexMesh(sim::create_sphere_mesh_data<sim::PosNormTexVertex>)},
              spSimData_{std::move(spSimData)}
    {
        spSimData_->camera.setUsingOrbitMode(true);
        spSimData_->camera.setOrbitOrigin({0, 0, 0});
        spSimData_->camera.setOrbitOffsetDistance(5);

        prevCam_ = spSimData_->camera;
    }

    void onUpdate(double worldTime, double timeStep)
    {
        prevCam_ = spSimData_->camera;
        spSimData_->camera.yaw(static_cast<float>(timeStep));
    }

    void onRender(int width, int height, double alpha)
    {
        auto a = static_cast<float>(alpha);

        sim::Camera camera;
        glm::vec3 eye{glm::mix(prevCam_.getEyeVector(), spSimData_->camera.getEyeVector(), a)};
        glm::vec3 look{glm::mix(prevCam_.getLookVector(), spSimData_->camera.getLookVector(), a)};
        glm::vec3 up{glm::mix(prevCam_.getUpVector(), spSimData_->camera.getUpVector(), a)};

        float aspect{glm::mix(prevCam_.getAspectRatio(), spSimData_->camera.getAspectRatio(), a)};

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

    void keyCallback(GLFWwindow *pWindow,
                     int key,
                     int scancode,
                     int action,
                     int mods,
                     const sim::SimCallbacks<Simulator>& parent)
    {
        spSimData_->paused = parent.isShiftDown();
    }

private:
    sim::MeshRenderer renderer_;
    std::shared_ptr<sim::SimData> spSimData_;
    sim::Camera prevCam_;
};

int main()
{
    try
    {
        sim::OpenGLSimulation<Simulator>{}.runNoFasterThanRealTimeLoop();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Program failed: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}