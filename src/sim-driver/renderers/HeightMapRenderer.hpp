#pragma once

#include <sim-driver/OpenGLForwardDeclarations.hpp>
#include <sim-driver/meshes/HeightMap.hpp>
#include <sim-driver/renderers/RendererHelper.hpp>

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

} // namespace sim
