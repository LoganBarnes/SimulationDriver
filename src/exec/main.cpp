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
        std::vector<sim::PosNormTexVertex> vbo{
                {{-0.4f, -0.6f, 0}, {0, 0, 1}, {0, 0}},
                {{0.6f,  -0.4f, 0}, {0, 0, 1}, {1, 0}},
                {{-0.6f, 0.4f,  0}, {0, 0, 1}, {0, 1}},
                {{0.4f,  0.6f,  0}, {0, 0, 1}, {1, 1}}
        };
        glIds_ = sim::OpenGLHelper::createPosNormTexPipeline(vbo.data(),
                                                             vbo.size());
    }

    void onUpdate(double worldTime, double timeStep)
    {}

    void onRender(int width, int height, double alpha)
    {
        glUseProgram(*glIds_.program);
        glm::vec3 lightDir{glm::normalize(lightDir_)};
        sim::OpenGLHelper::setIntUniform(glIds_.program, "displayMode", &displayMode_);
        sim::OpenGLHelper::setFloatUniform(glIds_.program, "shapeColor", glm::value_ptr(shapeColor_), 3);
        sim::OpenGLHelper::setFloatUniform(glIds_.program, "lightDir", glm::value_ptr(lightDir), 3);
        sim::OpenGLHelper::renderBuffer(glIds_.vao, 0, glIds_.numVerts, GL_TRIANGLE_STRIP);
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

                if (displayMode_ == 5) {
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