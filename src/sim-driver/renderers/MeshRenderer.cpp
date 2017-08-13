#include <sim-driver/renderers/MeshRenderer.hpp>
#include <imgui.h>

namespace sim
{

MeshRenderer::MeshRenderer(sim::PosNormTexMesh mesh)
        : mesh_{std::move(mesh)}
{
    renderer_.setDataFun([&]
                         {
                             return mesh_.getMeshData();
                         });
}

void MeshRenderer::render(float alpha, const Camera &camera) const
{
    renderer_.onRender(alpha, camera);
}

void MeshRenderer::configureGui()
{
    if (ImGui::CollapsingHeader("Mesh Options", "mesh", false, true))
    {
        bool mesh_needs_update = mesh_.configureGui();
        if (mesh_needs_update)
        {
            renderer_.rebuild_mesh();
        }
    }

    if (ImGui::CollapsingHeader("Render Options", "render", false, true))
    {
        renderer_.onGuiRender();
    }
}

void MeshRenderer::resize(int width, int height)
{
    renderer_.onResize(width, height);
}

} // namespace sim
