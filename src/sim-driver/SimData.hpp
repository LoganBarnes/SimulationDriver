#pragma once

#include <sim-driver/Camera.hpp>

namespace sim
{

struct SimData
{
    Camera camera{};
    bool paused{false};
};

struct SimInitData
{
    std::string title{"Simulation"};
    int width{0};
    int height{0};
    int samples{4};
};

}
