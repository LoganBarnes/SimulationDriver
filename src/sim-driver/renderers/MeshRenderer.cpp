#include <sim-driver/renderers/MeshRenderer.hpp>

namespace sim
{

namespace
{

struct MeshBuildVisitor
{
    template<typename Mesh>
    sim::PosNormTexData operator()(const Mesh &mesh)
    {
        return mesh.buildMeshData();
    }
};


struct MeshGuiVisitor
{
    template<typename Mesh>
    bool operator()(Mesh &mesh)
    {
        return mesh.onGuiRender();
    }
};


sim::PosNormTexData build_mesh_data(const std::vector<MeshVariant> &meshes)
{
    sim::PosNormTexData totalData{};

    MeshBuildVisitor mbv;

    for (const auto &mesh : meshes)
    {
        sim::PosNormTexData data = mapbox::util::apply_visitor(mbv, mesh);

        totalData.vbo.insert(totalData.vbo.end(), data.vbo.begin(), data.vbo.end());
        totalData.ibo.insert(totalData.ibo.end(), data.ibo.begin(), data.ibo.end());
        totalData.drawCalls.insert(totalData.drawCalls.end(), data.drawCalls.begin(), data.drawCalls.end());
    }

    totalData.vaoElements = sim::posNormTexVaoElements();

    return totalData;
}

}

MeshRenderer::MeshRenderer(sim::MeshVariant mesh)
    : MeshRenderer(std::vector<sim::MeshVariant>{std::move(mesh)})
{}

MeshRenderer::MeshRenderer(std::vector<sim::MeshVariant> meshes)
    : meshes_{std::move(meshes)}
{
    renderer_.setDataFun([&]{
        return build_mesh_data(meshes_);
    });
}

void MeshRenderer::onRender(float alpha, const Camera &camera) const
{
    renderer_.onRender(alpha, camera);
}

void MeshRenderer::onGuiRender()
{
    if (ImGui::CollapsingHeader("Mesh Options", "mesh", false, true))
    {
        bool mesh_needs_update = false;
        MeshGuiVisitor mgv;

        for (auto &mesh : meshes_)
        {
            mesh_needs_update |= mapbox::util::apply_visitor(mgv, mesh);
        }

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

void MeshRenderer::onResize(int width, int height)
{
    renderer_.onResize(width, height);
}

} // namespace sim
