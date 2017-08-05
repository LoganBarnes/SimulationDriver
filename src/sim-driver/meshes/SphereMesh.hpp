#pragma once


#include <sim-driver/OpenGLHelper.hpp>
#include <sim-driver/renderers/RendererHelper.hpp>

namespace sim
{

class SphereMesh
{
public:

    sim::PosNormTexData buildMeshData() const;

    bool onGuiRender();

private:
    bool linkDivisions_{false};
    int uDivisions_{100}, vDivisions_{100};
};

} // namespace sim
