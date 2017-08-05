#pragma once

#include <sim-driver/OpenGLForwardDeclarations.hpp>
#include <sim-driver/meshes/MeshVariant.hpp>

namespace sim
{

class MeshRenderer
{
public:
    explicit MeshRenderer(sim::MeshVariant mesh);

    explicit MeshRenderer(std::vector<sim::MeshVariant> meshes);

    void onRender(float alpha, const Camera &camera) const;
    void onGuiRender();
    void onResize(int width, int height);

private:
    sim::RendererHelper<sim::PosNormTexVertex> renderer_;

    std::vector<sim::MeshVariant> meshes_;
};

} // namespace sim
