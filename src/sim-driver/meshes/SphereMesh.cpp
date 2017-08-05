#include <sim-driver/meshes/SphereMesh.hpp>

namespace sim
{

namespace
{
sim::PosNormTexData build_mesh_data(int u_divisions, int v_divisions)
{
    sim::PosNormTexData data{};

    data.vbo.reserve(static_cast<unsigned>((u_divisions + 2) * (v_divisions + 2)));

    for (int thetai = 0; thetai < (u_divisions + 2); ++thetai)
    {
        float u = float(thetai) / (u_divisions + 1);
        float theta = glm::two_pi<float>() * u;

        for (int phii = 0; phii < (v_divisions + 2); ++phii)
        {
            float v = float(phii) / (v_divisions + 1);
            float phi = glm::pi<float>() * v;
            glm::vec3 p{glm::cos(theta) * glm::sin(phi),
                        glm::cos(phi),
                        glm::sin(theta) * glm::sin(phi)};
            data.vbo.push_back({{p.x,      p.y, p.z},
                                {p.x,      p.y, p.z},
                                {1.0f - u, v}});
        }
    }

    assert(data.vbo.size() == data.vbo.capacity());

    data.ibo.reserve(static_cast<unsigned>((u_divisions + 1) * ((v_divisions + 2) * 2 + 1)));

    unsigned index = 0;
    for (int thetai = 0; thetai < (u_divisions + 1); ++thetai)
    {
        for (int phii = 0; phii < (v_divisions + 2); ++phii)
        {
            data.ibo.push_back(index);
            data.ibo.push_back(index + v_divisions + 2);
            ++index;
        }
        data.ibo.push_back(0xFFFFFFFF);
    }
    data.ibo.pop_back();

    assert(data.ibo.size() == data.ibo.capacity() - 1);

    data.vaoElements = sim::posNormTexVaoElements();
    return data;
}
}

sim::PosNormTexData SphereMesh::buildMeshData() const
{
    return build_mesh_data(uDivisions_, vDivisions_);
}

bool SphereMesh::onGuiRender()
{
    bool mesh_needs_update = false;

    ImGui::Checkbox("Link U and V", &linkDivisions_);

    mesh_needs_update |= ImGui::SliderInt("U Subdivisions", &uDivisions_, 0, 250);

    if (linkDivisions_)
    {
        mesh_needs_update |= (uDivisions_ != vDivisions_);
        vDivisions_ = uDivisions_;
    }
    else
    {
        mesh_needs_update |= ImGui::SliderInt("V Subdivisions", &vDivisions_, 0, 250);
    }

    return mesh_needs_update;
}

} // namespace sim
