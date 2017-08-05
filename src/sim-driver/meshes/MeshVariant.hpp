#pragma once

#include <mapbox/variant.hpp>
#include <sim-driver/meshes/SphereMesh.hpp>

namespace sim
{

using MeshVariant = mapbox::util::variant<sim::SphereMesh>;

} // namespace ray
