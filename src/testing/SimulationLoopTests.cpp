#include <sim-driver/OpenGLSimulation.hpp>
#include <gtest/gtest.h>
#include <thread>

namespace {

struct EmptySim
{
    EmptySim(int, int, sim::SimData *data) : sim_data(*data) {}

    void onUpdate(double t, double ts)
    {
        sim_time = t;
        timestep = ts;
        ++num_updates;
    }

    sim::SimData &sim_data;
    double sim_time{-1.f};
    double timestep{-1.f};
    std::size_t num_updates{0};
};

template <typename Func>
double time_it(const Func &func)
{
    auto start_time = std::chrono::steady_clock::now();
    func();
    auto end_time = std::chrono::steady_clock::now();
    return std::chrono::duration<double>{end_time - start_time}.count();
}

} // namespace

class LoopTimingTest : public ::testing::Test
{
protected:
    sim::OpenGLSimulation<EmptySim> sim{{""}};
};

TEST_F(LoopTimingTest, runs_faster_than_realtime)
{
    constexpr std::size_t max_iters = 1000;

    double duration = time_it([&] { sim.runAsFastAsPossibleLoop(max_iters); });

    auto &child = sim.get_child_sim();
    EXPECT_GT(child.sim_time, duration);
    EXPECT_EQ(max_iters, child.num_updates);
    // first update is actually time=0.0 so we subtract one
    EXPECT_NEAR((child.num_updates - 1) * child.timestep, child.sim_time, 1e-9);
}

TEST_F(LoopTimingTest, runs_at_realtime_speed)
{
    constexpr std::size_t max_iters = 240;

    double duration = time_it([&] { sim.runNoFasterThanRealTimeLoop(max_iters); });

    auto &child = sim.get_child_sim();
    EXPECT_LT(child.sim_time, duration);
    EXPECT_EQ(max_iters, child.num_updates);
    EXPECT_NEAR(child.sim_time, duration, 0.05);
    // first update is actually time=0.0 so we subtract one
    EXPECT_NEAR((child.num_updates - 1) * child.timestep, child.sim_time, 1e-9);
}

#ifdef OFFSCREEN
TEST_F(LoopTimingTest, event_loop_does_not_block_without_window)
{
    constexpr std::size_t max_iters = 1000;

    double duration = time_it([&] { sim.runAsFastAsPossibleLoop(max_iters); });

    auto &child = sim.get_child_sim();
    EXPECT_GT(child.sim_time, duration);
    EXPECT_EQ(max_iters, child.num_updates);
    // first update is actually time=0.0 so we subtract one
    EXPECT_NEAR((child.num_updates - 1) * child.timestep, child.sim_time, 1e-9);
}
#else
TEST_F(LoopTimingTest, event_loop_holds_until_event)
{
    constexpr std::size_t max_iters = 2;
    constexpr std::size_t wait_millis = 3000;

    std::thread thread([&] {
        std::this_thread::sleep_for(std::chrono::milliseconds(wait_millis));
        glfwPostEmptyEvent();
    });

    double duration = time_it([&] { sim.runEventLoop(max_iters); });

    thread.join();

    auto &child = sim.get_child_sim();
    EXPECT_LT(child.sim_time, duration);
    EXPECT_EQ(max_iters, child.num_updates);
    EXPECT_NEAR(wait_millis * 1e-3, duration, 5e-4);
    // first update is actually time=0.0 so we subtract one
    EXPECT_NEAR((child.num_updates - 1) * child.timestep, child.sim_time, 1e-9);
}
#endif
