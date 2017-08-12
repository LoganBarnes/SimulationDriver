#pragma once

#include <sim-driver/OpenGLForwardDeclarations.hpp>
#include <sim-driver/meshes/HeightMap.hpp>
#include "RendererHelper.hpp"

namespace sim
{

template<typename T = float>
class HeightMapRenderer
{
public:
    explicit HeightMapRenderer(sim::HeightMap<T> heightMap);

    void onRender(float alpha, const Camera &camera) const;
    void onGuiRender();
    void onResize(int width, int height);

private:
    sim::StandardPipeline glIds_;
    sim::HeightMap<T> heightMap_;

    void updateVboAndVao();
};

template<typename T>
HeightMapRenderer<T>::HeightMapRenderer(sim::HeightMap<T> heightMap)
    : heightMap_{std::move(heightMap)}
{
    glIds_.program = sim::OpenGLHelper::createProgram(sim::SHADER_PATH + "heights.vert",
                                                      sim::SHADER_PATH + "heights.geom",
                                                      sim::SHADER_PATH + "shader.frag");
    updateVboAndVao();
}

template<typename T>
void HeightMapRenderer<T>::onRender(float alpha, const Camera &camera) const
{
    glUseProgram(*glIds_.program);

    sim::OpenGLHelper::renderBuffer(glIds_.vao, 0, glIds_.vboSize, GL_POINTS);
}

template<typename T>
void HeightMapRenderer<T>::onGuiRender()
{}

template<typename T>
void HeightMapRenderer<T>::onResize(int width, int height)
{}

template<typename T>
void HeightMapRenderer<T>::updateVboAndVao()
{
    glIds_.vbo = nullptr;

    int w = heightMap_.width();
    std::vector<float> vboData(static_cast<unsigned>(w * heightMap_.height() * 4));

    heightMap_.forEach([&](int x, int y, T val) {
        int index = y * w + x;
        vboData[index * 4 + 0] = val;
        vboData[index * 4 + 1] = heightMap_.safeGet(x + 0, y + 1, val);
        vboData[index * 4 + 2] = heightMap_.safeGet(x + 1, y + 1, val);
        vboData[index * 4 + 3] = heightMap_.safeGet(x + 1, y + 0, val);
    });

    glIds_.vbo = sim::OpenGLHelper::createBuffer(vboData.data(), vboData.size());
    glIds_.vao = sim::OpenGLHelper::createVao(glIds_.program, glIds_.vbo, 0, {{"inHeights", 4, GL_FLOAT, 0}});

    glIds_.vboSize = vboData.size() / 4;
}



} // namespace sim
