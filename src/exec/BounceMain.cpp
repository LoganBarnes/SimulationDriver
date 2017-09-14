#include <sim-driver/OpenGLSimulation.hpp>
#include <sim-driver/renderers/RendererHelper.hpp>
#include <sim-driver/renderers/MeshRenderer.hpp>
#include <sim-driver/meshes/MeshFunctions.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Simulator
{
public:
    explicit Simulator(int, int, sim::SimData *pSimData)
            : renderer_{sim::PosNormTexMesh(sim::create_sphere_mesh_data<sim::PosNormTexVertex>)},
              simData_{*pSimData}
    {
        simData_.cameraMover.setUsingOrbitMode(true);
        simData_.cameraMover.setOrbitOrigin({0, 0, 0});
        simData_.cameraMover.setOrbitOffsetDistance(5);

        simData_.paused = true;

        prevCam_ = simData_.camera();

        model_ = glm::translate(glm::mat4{1}, {0, 0, 0}) * glm::scale(glm::mat4{1}, glm::vec3(0.05f));
        renderer_.setModelMatrix(model_);
    }

    void onUpdate(double, double timeStep)
    {
        prevCam_ = simData_.camera();
        simData_.cameraMover.yaw(static_cast<float>(timeStep * 25));
    }

    void onRender(int, int, double alpha)
    {
        auto a = static_cast<float>(alpha);

        if (simData_.paused)
        {
            renderer_.render(a, simData_.camera());
        }
        else
        {
            sim::Camera camera;
            glm::vec3 eye{glm::mix(prevCam_.getEyeVector(), simData_.camera().getEyeVector(), a)};
            glm::vec3 look{glm::mix(prevCam_.getLookVector(), simData_.camera().getLookVector(), a)};
            glm::vec3 up{glm::mix(prevCam_.getUpVector(), simData_.camera().getUpVector(), a)};

            float aspect{glm::mix(prevCam_.getAspectRatio(), simData_.camera().getAspectRatio(), a)};

            camera.lookAt(eye, eye + look, up);
            camera.setAspectRatio(aspect);

            renderer_.render(a, camera);
        }
    }

    void onGuiRender(int, int)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        if (ImGui::Begin("Window", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Framerate: %.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
            renderer_.configureGui();
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void keyCallback(GLFWwindow *, int key, int, int action, int, const sim::SimCallbacks<Simulator> &)
    {
        if (key == GLFW_KEY_G && action == GLFW_RELEASE)
        {
            gravity_ = -gravity_;
        }
    }

private:
    sim::MeshRenderer renderer_;
    sim::SimData &simData_;
    sim::Camera prevCam_;

    glm::mat4 model_{1};

    glm::vec3 gravity_{0, -9.8f, 0};
};

int main()
{
    try
    {
        sim::SimInitData initData;
        initData.title = "Mesh Sim";
        sim::OpenGLSimulation<Simulator>(initData).runNoFasterThanRealTimeLoop();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Program failed: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}