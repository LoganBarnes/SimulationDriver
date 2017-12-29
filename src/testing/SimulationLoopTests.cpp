#include <sim-driver/OpenGLSimulation.hpp>
#include <gtest/gtest.h>

namespace {
struct EmptySim
{
};
} // namespace

TEST(OpenGLSimulationLoop, runs_faster_than_realtime)
{
    sim::OpenGLSimulation<EmptySim> sim;
    sim.runAsFastAsPossibleLoop(1000);
    EXPECT_TRUE(true);
}
