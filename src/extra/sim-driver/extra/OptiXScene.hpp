#pragma once

#include <sim-driver/extra/OptiXSimulation.hpp>

namespace sim {

class OptiXScene
{

public:
    explicit OptiXScene(optix::Context &context);

private:
    optix::Material material_;
};

} // namespace sim
