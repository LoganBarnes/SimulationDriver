#pragma once

#include <sim-driver/CameraMover.hpp>
#include <string>

namespace sim {

struct SimData {
    CameraMover cameraMover{Camera{}};
    bool paused{false};

    Camera& camera() { return cameraMover.camera; }
};

struct SimInitData {
    std::string title{"Simulation"};
    int width{0};
    int height{0};
    int samples{4};
};
}
