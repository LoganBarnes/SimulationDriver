#include <sim-driver/OpenGLSimulation.hpp>
#include <gtest/gtest.h>

namespace {
struct FirstSim
{
};

struct SecondSim
{
    SecondSim(int, int) {}
};

struct ThirdSim
{
    ThirdSim(int, int, sim::SimData *) {}
};

} // namespace

TEST(OpenGLSimulationCompiles, compiles_with_default_constructor)
{
    sim::OpenGLSimulation<FirstSim> sim{{""}};
    EXPECT_TRUE(true);
}

TEST(OpenGLSimulationCompiles, compiles_with_width_height_constructor)
{
    sim::OpenGLSimulation<SecondSim> sim{{""}};
    EXPECT_TRUE(true);
}

TEST(OpenGLSimulationCompiles, compiles_with_SimData_constructor)
{
    sim::OpenGLSimulation<ThirdSim> sim{{""}};
    EXPECT_TRUE(true);
}
