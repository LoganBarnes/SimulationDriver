#pragma once

#include <sim-driver/OpenGLForwardDeclarations.hpp>
#include <sim-driver/meshes/MeshHelper.hpp>

namespace sim
{

class MeshRenderer
{
public:
    explicit MeshRenderer(sim::PosNormTexMesh mesh);

    void render(float alpha, const Camera &camera) const;
    void configureGui();
    void resize(int width, int height);

private:
    sim::PosNormTexRenderer renderer_;
    sim::PosNormTexMesh mesh_;
};

} // namespace sim
