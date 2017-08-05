#pragma once

#include <sim-driver/OpenGLForwardDeclarations.hpp>
#include <sim-driver/meshes/HeightMap.hpp>
#include "RendererHelper.hpp"

namespace sim
{

template<typename T>
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
};

template<typename T>
HeightMapRenderer<T>::HeightMapRenderer(sim::HeightMap<T> heightMap)
    : heightMap_{std::move(heightMap)}
{
    glIds_.program = sim::OpenGLHelper::createProgram(sim::SHADER_PATH + "heights.vert",
                                                      sim::SHADER_PATH + "heights.geom",
                                                      sim::SHADER_PATH + "shader.frag");
}

template<typename T>
void HeightMapRenderer<T>::onRender(float alpha, const Camera &camera) const
{}

template<typename T>
void HeightMapRenderer<T>::onGuiRender()
{}

template<typename T>
void HeightMapRenderer<T>::onResize(int width, int height)
{}

} // namespace sim
