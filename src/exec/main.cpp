#include <sim-driver/OpenGLSimulation.hpp>
#include <sim-driver/OpenGLHelper.hpp>
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
        float s2 = 1.f / std::sqrt(2.0f);
        std::vector<sim::PosNormTexVertex> vbo{
            {{0,   0,   1},  {0,   0,   1},  {0.5f, 0.5f}},
            {{-s2, s2,  0},  {-s2, s2,  0},  {0,    1}},
            {{-s2, -s2, 0},  {-s2, -s2, 0},  {0,    0}},
            {{s2,  -s2, 0},  {s2,  -s2, 0},  {1,    0}},
            {{s2,  s2,  0},  {s2,  s2,  0},  {1,    1}},
            {{0,   0,   -1}, {0,   0,   -1}, {0.5f, 0.5f}},
        };
        glIds_ = sim::OpenGLHelper::createPosNormTexPipeline(vbo.data(),
                                                             vbo.size());

        std::vector<unsigned> ibo{
            0, 1, 2, 3, 4, 1, 0xFFFFFFFF, 5, 4, 3, 2, 1, 4
        };
        glIds_.ibo = sim::OpenGLHelper::createBuffer<unsigned>(ibo.data(),
                                                               ibo.size(),
                                                               GL_ELEMENT_ARRAY_BUFFER);
        glIds_.numVerts = static_cast<int>(ibo.size());

        camera_.setUsingOrbitMode(true);
        camera_.setOrbitPoint({0, 0, 0});
        camera_.setOrbitOffsetDistance(5);
    }

    void onUpdate(double worldTime, double timeStep)
    {
        camera_.yaw(static_cast<float>(timeStep));
        camera_.pitch(static_cast<float>(timeStep));
    }

    void onRender(int width, int height, double alpha)
    {
        glUseProgram(*glIds_.program);
        glm::vec3 lightDir{glm::normalize(lightDir_)};
        sim::OpenGLHelper::setMatrixUniform(glIds_.program,
                                            "projectionView",
                                            glm::value_ptr(camera_.getPerspectiveProjectionViewMatrix()));
        sim::OpenGLHelper::setIntUniform(glIds_.program, "displayMode", &displayMode_);
        sim::OpenGLHelper::setFloatUniform(glIds_.program, "shapeColor", glm::value_ptr(shapeColor_), 3);
        sim::OpenGLHelper::setFloatUniform(glIds_.program, "lightDir", glm::value_ptr(lightDir), 3);
        sim::OpenGLHelper::renderBuffer(glIds_.vao, 0, glIds_.numVerts, GL_TRIANGLE_FAN, glIds_.ibo);
    }

    void onGuiRender(int width, int height)
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        if (ImGui::Begin("Window", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Combo("Display Mode", &displayMode_,
                         " Position \0 Normal \0 Tex Coords \0 Color \0"
                             " Texture \0 Simple Shading \0 Advanced Shading \0 White \0\0");
            if (displayMode_ == 3 || displayMode_ == 5 || displayMode_ == 6)
            {
                ImGui::ColorEdit3("Shape Color", glm::value_ptr(shapeColor_));

                if (displayMode_ == 5)
                {
                    ImGui::SliderFloat3("Light Direction", glm::value_ptr(lightDir_), -1.0f, 1.0f);
                }
            }
        }
        ImGui::End();
    }

private:
    sim::StandardPipeline glIds_;
    int displayMode_{0};
    glm::vec3 shapeColor_{1.0};
    glm::vec3 lightDir_{0.7f, 0.85f, 1.0f};

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