#include <sim-driver/meshes/MeshHelper.hpp>
#include <sim-driver/OpenGLHelper.hpp>
#include <sim-driver/renderers/RendererHelper.hpp>
#include <imgui.h>
#include <iostream>

namespace sim {
template <typename V>
MeshHelper<V>::MeshHelper(MeshDataFun dataFun) : dataFun_{std::move(dataFun)}
{
    updateData();
}

template <typename V>
void MeshHelper<V>::setMeshDataFunction(MeshDataFun dataFun)
{
    dataFun_ = std::move(dataFun);
    updateData();
}

template <typename V>
bool MeshHelper<V>::configureGui()
{
    bool mesh_needs_update = false;
    ImGui::Checkbox("Link U and V", &linkDivisions_);

    mesh_needs_update |= ImGui::SliderInt("U Subdivisions", &uDivisions_, 0, 250);

    if (linkDivisions_) {
        mesh_needs_update |= (uDivisions_ != vDivisions_);
        vDivisions_ = uDivisions_;
    } else {
        mesh_needs_update |= ImGui::SliderInt("V Subdivisions", &vDivisions_, 0, 250);
    }

    if (mesh_needs_update) {
        updateData();
    }

    return mesh_needs_update;
}

template <typename V>
const sim::DrawData<V> &MeshHelper<V>::getMeshData() const
{
    return data_;
}

template <typename V>
void MeshHelper<V>::updateData()
{
    if (dataFun_) {
        data_ = dataFun_(uDivisions_, vDivisions_);
    }
}

template class MeshHelper<sim::PosNormTexVertex>;
template class MeshHelper<sim::PosVertex>;

} // namespace sim
