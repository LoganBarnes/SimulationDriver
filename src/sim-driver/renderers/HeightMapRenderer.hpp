#pragma once

#include <sim-driver/OpenGLForwardDeclarations.hpp>
#include <sim-driver/meshes/HeightMap.hpp>
#include <sim-driver/renderers/RendererHelper.hpp>
#include <ShaderConfig.hpp>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/string_cast.hpp>

//#define NORMAL_RENDER

namespace sim
{

class HeightMapRenderer
{
public:
    explicit HeightMapRenderer(sim::HeightMap heightMap);

    void render(float alpha, const Camera &camera) const;

    void configureGui();

    void resize(int width, int height);

private:
    sim::StandardPipeline glIds_;
    sim::HeightMap heightMap_;
    sim::PosRenderer renderer_{};

    int displayMode_{3};

    bool renderPointsOnly_{true};

    sim::PosData updateVboAndVao();
};

HeightMapRenderer::HeightMapRenderer(sim::HeightMap heightMap)
        : heightMap_{std::move(heightMap)}
{
#ifdef NORMAL_RENDER
    glIds_.program = sim::OpenGLHelper::createProgram(sim::SHADER_PATH + "shader.vert",
                                                      sim::SHADER_PATH + "shader.frag");
    std::vector<sim::PosNormTexVertex> vbo{
        {{-1, -1, 0}, {0, 0, 1}, {0, 1}},
        {{1,  -1, 0}, {0, 0, 1}, {1, 1}},
        {{-1, 1,  0}, {0, 0, 1}, {0, 0}},
        {{1,  1,  0}, {0, 0, 1}, {1, 0}}
    };
#else
    glIds_.program = sim::OpenGLHelper::createProgram(sim::SHADER_PATH + "heights.vert",
//                                                      sim::SHADER_PATH + "heights.geom",
#ifdef __APPLE__
                                                      sim::SHADER_PATH + "shader_mac.frag");
#else
    sim::SHADER_PATH + "shader.frag");
#endif

    std::vector<int> vbo;
#endif
    std::vector<float> tex;

    for (int yi = 0; yi < heightMap_.getHeight(); ++yi)
    {
        for (int xi = 0; xi < heightMap_.getWidth(); ++xi)
        {
#ifndef NORMAL_RENDER
            vbo.emplace_back(xi);
            vbo.emplace_back(yi);
#endif
            tex.emplace_back(heightMap_.safeGet(xi, yi));
        }
    }

    glIds_.vbo = sim::OpenGLHelper::createBuffer(vbo.data(), vbo.size());

#ifdef NORMAL_RENDER
    glIds_.vao = sim::OpenGLHelper::createVao(glIds_.program,
                                              glIds_.vbo,
                                              sizeof(sim::PosNormTexVertex),
                                              sim::posNormTexVaoElements());

    glIds_.vboSize = static_cast<int>(vbo.size());
#else
    glIds_.vao = sim::OpenGLHelper::createVao(glIds_.program, glIds_.vbo, 0, {{"indices", 2, GL_INT, 0}});
    glIds_.vboSize = static_cast<int>(vbo.size()) / 2;
#endif

    glIds_.texture = sim::OpenGLHelper::createTextureArray(
            heightMap_.getWidth(), heightMap_.getHeight(), tex.data(), GL_LINEAR, GL_CLAMP_TO_EDGE, GL_R32F, GL_RED
    );


    renderer_.setDisplayMode(3);
    renderer_.setShowingVertsOnly(true);
    renderer_.setPointSize(5);

    renderer_.setDataFun([&]() -> sim::PosData { return updateVboAndVao(); });
}

void HeightMapRenderer::render(float alpha, const Camera &camera) const
{
    if (renderPointsOnly_)
    {
        renderer_.onRender(alpha, camera);
    }
    else
    {
        glUseProgram(*glIds_.program);

#ifndef NORMAL_RENDER
        glm::ivec2 texSize{heightMap_.getWidth(), heightMap_.getHeight()};
        glm::vec3 worldOrigin{heightMap_.getWorldOrigin()};
        glm::vec3 worldDimensions{heightMap_.getWorldDimensions()};

        sim::OpenGLHelper::setMatrixUniform(
                glIds_.program, "screen_from_world", glm::value_ptr(camera.getPerspectiveScreenFromWorldMatrix())
        );
        sim::OpenGLHelper::setTextureUniform(glIds_.program, "heights", glIds_.texture, 0);
        sim::OpenGLHelper::setIntUniform(glIds_.program, "tex_size", glm::value_ptr(texSize), 2);
        sim::OpenGLHelper::setFloatUniform(glIds_.program, "world_origin", glm::value_ptr(worldOrigin), 3);
        sim::OpenGLHelper::setFloatUniform(glIds_.program, "world_dimensions", glm::value_ptr(worldDimensions), 3);
#endif
        sim::OpenGLHelper::setIntUniform(glIds_.program, "displayMode", &displayMode_);

#ifndef NORMAL_RENDER
        glDisable(GL_PROGRAM_POINT_SIZE);
        glPointSize(5);
        sim::OpenGLHelper::renderBuffer(glIds_.vao, 0, glIds_.vboSize, GL_POINTS);
        glEnable(GL_PROGRAM_POINT_SIZE);
#else
        sim::OpenGLHelper::setTextureUniform(glIds_.program, "tex", glIds_.texture, 0);
        sim::OpenGLHelper::renderBuffer(glIds_.vao, 0, glIds_.vboSize, GL_TRIANGLE_STRIP);
#endif
    }
}

void HeightMapRenderer::configureGui()
{
    ImGui::Checkbox("Render Points Only", &renderPointsOnly_);

    ImGui::Combo("Display Mode", &displayMode_,
                 " Position \0"
                         " Normal \0"
                         " Tex Coords \0"
                         " Color \0"
                         " Texture \0"
                         " Simple Shading \0"
                         " Advanced Shading \0"
                         " White \0"
                         "\0\0");
}

void HeightMapRenderer::resize(int width, int height)
{
    renderer_.onResize(width, height);
}

sim::PosData HeightMapRenderer::updateVboAndVao()
{
    sim::PosData data{};

    heightMap_.forEach([&](int x, int y, float val)
                       {
                           glm::vec3 p{heightMap_.safeGetWorld(x, y)};
                           data.vbo.push_back({{p.x, p.y, p.z}});
                       });

    data.vaoElements = sim::posVaoElements();

    return data;
}


} // namespace sim
