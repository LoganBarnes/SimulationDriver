#pragma once

#include <sim-driver/OpenGLHelper.hpp>
#include <sim-driver/renderers/RendererHelper.hpp>

namespace sim {

template <typename V>
class MeshHelper
{
public:
    using MeshDataFun = std::function<sim::DrawData<V>(int, int)>;

    explicit MeshHelper(MeshDataFun dataFun = nullptr);

    bool configureGui();

    void setMeshDataFunction(MeshDataFun dataFun);

    const sim::DrawData<V> &getMeshData() const;

    void updateData();

private:
    sim::DrawData<V> data_;
    MeshDataFun dataFun_;

    bool linkDivisions_{false};
    int uDivisions_{100}, vDivisions_{100};
};

using PosNormTexMesh = MeshHelper<sim::PosNormTexVertex>;
using PosMesh = MeshHelper<sim::PosVertex>;

} // namespace sim
