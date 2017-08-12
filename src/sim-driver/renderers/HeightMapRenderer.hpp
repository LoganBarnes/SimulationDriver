#pragma once

#include <sim-driver/OpenGLForwardDeclarations.hpp>
#include <sim-driver/meshes/HeightMap.hpp>
#include "RendererHelper.hpp"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/string_cast.hpp>

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

    sim::PosData updateVboAndVao();
};

HeightMapRenderer::HeightMapRenderer(sim::HeightMap heightMap)
    : heightMap_{std::move(heightMap)}
{
//    glIds_.program = sim::OpenGLHelper::createProgram(sim::SHADER_PATH + "heights.vert",
//                                                      sim::SHADER_PATH + "heights.geom",
//                                                      sim::SHADER_PATH + "shader.frag");

    renderer_.setDisplayMode(3);
    renderer_.setShowingVertsOnly(true);
    renderer_.setPointSize(5);

    renderer_.setDataFun([&]() -> sim::PosData
                         { return updateVboAndVao(); });
}

void HeightMapRenderer::render(float alpha, const Camera &camera) const
{
//    glUseProgram(*glIds_.program);
//
//    sim::OpenGLHelper::renderBuffer(glIds_.vao, 0, glIds_.vboSize, GL_POINTS);
    renderer_.onRender(alpha, camera);
}

void HeightMapRenderer::configureGui()
{
    renderer_.onGuiRender();
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

//    int w = heightMap_.width();
//    std::vector<float> vboData(static_cast<unsigned>(w * heightMap_.height() * 4));
//
//    heightMap_.forEach([&](int x, int y, T val) {
//        int index = y * w + x;
//        vboData[index * 4 + 0] = val;
//        vboData[index * 4 + 1] = heightMap_.safeGet(x + 0, y + 1, val);
//        vboData[index * 4 + 2] = heightMap_.safeGet(x + 1, y + 1, val);
//        vboData[index * 4 + 3] = heightMap_.safeGet(x + 1, y + 0, val);
//    });

//    glIds_.vbo = sim::OpenGLHelper::createBuffer(vboData.data(), vboData.size());
//    glIds_.vao = sim::OpenGLHelper::createVao(glIds_.program, glIds_.vbo, 0, {{"inHeights", 4, GL_FLOAT, 0}});

//    glIds_.vboSize = static_cast<int>(vboData.size()) / 4;
}


} // namespace sim
