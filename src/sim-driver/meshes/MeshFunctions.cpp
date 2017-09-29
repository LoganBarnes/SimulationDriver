#include <sim-driver/meshes/MeshHelper.hpp>
#include <glm/gtc/constants.hpp>

namespace sim
{

template<typename V>
V create_sphere_vertex(glm::vec3 p, glm::vec2 t);

template<>
sim::PosNormTexVertex create_sphere_vertex(glm::vec3 p, glm::vec2 t)
{
    return {{p.x, p.y, p.z},
            {p.x, p.y, p.z},
            {t.x, t.y}};
}

template<>
sim::PosVertex create_sphere_vertex(glm::vec3 p, glm::vec2)
{
    return {{p.x, p.y, p.z}};
}

template<typename V>
sim::DrawData<V> create_sphere_mesh_data(int u_divisions, int v_divisions)
{
    sim::DrawData<V> data{};

    data.vbo.reserve(static_cast<unsigned>((u_divisions + 2) * (v_divisions + 2)));

    for (int thetai = 0; thetai < (u_divisions + 2); ++thetai) {
        float u = float(thetai) / (u_divisions + 1);
        float theta = glm::two_pi<float>() * u;

        for (int phii = 0; phii < (v_divisions + 2); ++phii) {
            float v = float(phii) / (v_divisions + 1);
            float phi = glm::pi<float>() * v;
            glm::vec3 p{glm::cos(theta) * glm::sin(phi),
                        glm::cos(phi),
                        glm::sin(theta) * glm::sin(phi)};
            data.vbo.emplace_back(create_sphere_vertex<V>(p, glm::vec2{1.0f - u, v}));
        }
    }

    assert(data.vbo.size() == data.vbo.capacity());

    data.ibo.reserve(static_cast<unsigned>((u_divisions + 1) * ((v_divisions + 2) * 2 + 1)));

    unsigned index = 0;
    for (int thetai = 0; thetai < (u_divisions + 1); ++thetai) {
        for (int phii = 0; phii < (v_divisions + 2); ++phii) {
            data.ibo.push_back(index);
            data.ibo.push_back(index + v_divisions + 2);
            ++index;
        }
        data.ibo.push_back(sim::primitiveRestart());
    }

    assert(data.ibo.size() == data.ibo.capacity());

    data.vaoElements = sim::posNormTexVaoElements();
    return data;
}

template sim::PosNormTexData create_sphere_mesh_data(int u_divisions, int v_divisions);
template sim::PosData create_sphere_mesh_data(int u_divisions, int v_divisions);

} // namespace sim
