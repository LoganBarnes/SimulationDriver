#include <sim-driver/renderers/MeshRenderer.hpp>
#include <imgui.h>
#include <iostream>

namespace sim
{

MeshRenderer::MeshRenderer(sim::PosNormTexMesh mesh)
    : mesh_{std::move(mesh)}
{
    renderer_.setDataFun([&]() -> const sim::DrawData<PosNormTexVertex> &
                         { return mesh_.getMeshData(); });
}

void MeshRenderer::render(float alpha, const Camera &camera) const
{
    renderer_.onRender(alpha, &camera);
}

void MeshRenderer::configureGui()
{
    std::stringstream uid;
    uid << this;
    ImGui::PushID(uid.str().c_str());
    if (ImGui::CollapsingHeader("Mesh Options", "mesh_options", false, true)) {
        bool mesh_needs_update = mesh_.configureGui();
        if (mesh_needs_update) {
            renderer_.rebuild_mesh();
        }
    }

    if (ImGui::CollapsingHeader("Render Options", "render_options", false, true)) {
        renderer_.onGuiRender();
    }
    ImGui::PopID();
}

void MeshRenderer::resize(int width, int height)
{
    renderer_.onResize(width, height);
}

void MeshRenderer::setModelMatrix(const glm::mat4 &modelMatrix)
{
    renderer_.setModelMatrix(modelMatrix);
}

} // namespace sim
