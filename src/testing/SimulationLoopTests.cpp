#include <sim-driver/OpenGLSimulation.hpp>
#include <gtest/gtest.h>

namespace {
struct EmptySim
{
    EmptySim(int, int, sim::SimData* data) : sim_data(*data) {}

    void onUpdate(double t, double)
    {
        sim_time = t;
        ++num_updates;
    }

    sim::SimData& sim_data;
    double sim_time{-1.f};
    std::size_t num_updates{0};
};
} // namespace

class LoopTimingTest : public ::testing::Test
{
protected:
    virtual void SetUp() { test_start_time = std::chrono::steady_clock::now(); }

    double get_duration_since_test_start()
    {
        auto current_time = std::chrono::steady_clock::now();
        return std::chrono::duration<double>{current_time - test_start_time}.count();
    }

    decltype(std::chrono::steady_clock::now()) test_start_time;
    sim::OpenGLSimulation<EmptySim> sim{{""}};
};

TEST_F(LoopTimingTest, runs_faster_than_realtime)
{
    constexpr std::size_t max_iters = 1000;
    sim.runAsFastAsPossibleLoop(max_iters); // sim timestep is
    EXPECT_GT(sim.get_child_sim().sim_time, get_duration_since_test_start());
    EXPECT_EQ(max_iters, sim.get_child_sim().num_updates);
}
