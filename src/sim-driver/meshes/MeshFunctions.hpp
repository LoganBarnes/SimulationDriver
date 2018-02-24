#pragma once

#include <sim-driver/meshes/MeshHelper.hpp>

namespace sim {

template <typename V>
sim::DrawData<V> create_sphere_mesh_data(int u_divisions, int v_divisions);

} // namespace sim
