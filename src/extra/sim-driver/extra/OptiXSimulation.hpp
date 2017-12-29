#pragma once

#include <sim-driver/SimDriver.hpp>
#include <sim-driver/OpenGLHelper.hpp>
#include <optixu/optixpp_namespace.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <sim-driver/renderers/RendererHelper.hpp>

namespace sim {

template <typename T, typename... Args>
auto make_child(priority_tag<2>, optix::Context &context, int w, int h, SimData *pSimData, Args... args)
    -> decltype(T(w, h, context, pSimData, args...))
{
    return T(w, h, context, pSimData, args...);
}

template <typename T, typename... Args>
auto make_child(priority_tag<1>, optix::Context &context, int w, int h, SimData *, Args... args)
    -> decltype(T(w, h, context, args...))
{
    return T(w, h, context, args...);
}

template <typename T, typename... Args>
auto make_child(priority_tag<0>, optix::Context &context, int, int, SimData *, Args... args)
    -> decltype(T(context, args...))
{
    return T(context, args...);
}

template <typename Child>
class OptiXSimulation : public SimDriver<OptiXSimulation<Child>>
{
public:
    OptiXSimulation();

    template <typename... Args>
    explicit OptiXSimulation(const SimInitData &initData, Args... args);

    void update(double worldTime, double timeStep);
    void render(int width, int height, double alpha, bool eventDriven);
    bool paused() const;

    void framebufferSizeCallback(GLFWwindow *pWindow, int width, int height);

    optix::Context &context();

private:
    std::shared_ptr<optix::Context> spContext_;

    Child child_;

    CallbackWrapper<OptiXSimulation<Child>, Child> callbackWrapper_;
    SimCallbacks<CallbackWrapper<OptiXSimulation<Child>, Child>> callbacks_;

    sim::PosNormTexRenderer renderer_;

    std::shared_ptr<GLuint> optixVbo_;
    std::shared_ptr<GLuint> texture_;

    template <class T>
    auto updateChild(T &child, double worldTime, double timeStep, int i)
        -> decltype(child.onUpdate(worldTime, timeStep), void());

    template <class T>
    auto updateChild(T &child, double worldTime, double timeStep, long l) -> decltype(void());

    template <class T>
    auto renderChild(T &child, int width, int height, double alpha, optix::Context &context, int i)
        -> decltype(child.onRender(width, height, alpha, context), void());

    template <class T>
    auto renderChild(T &child, int width, int height, double alpha, optix::Context &context, long l)
        -> decltype(void());

    template <class T>
    auto renderChildGui(T &child, int width, int height, int i) -> decltype(child.onGuiRender(width, height), void());

    template <class T>
    auto renderChildGui(T &child, int width, int height, long l) -> decltype(void());
};

template <typename Child>
OptiXSimulation<Child>::OptiXSimulation() : OptiXSimulation(SimInitData{})
{
}

template <typename Child>
template <typename... Args>
OptiXSimulation<Child>::OptiXSimulation(const SimInitData &initData, Args... args)
    : SimDriver<OptiXSimulation<Child>>(initData),
      spContext_(new optix::Context(optix::Context::create()),
                 [](auto p) {
                     DEBUG_PRINT("Destroying OptiX context");
                     static_assert(std::is_same<decltype(p), optix::Context *>::value, "");
                     (*p)->destroy();
                     delete p;
                 }),
      child_{make_child<Child>(sim::priority_tag<2>{},
                               *spContext_,
                               this->getWidth(),
                               this->getHeight(),
                               &this->simData,
                               args...)},
      callbackWrapper_{this, &child_},
      callbacks_{&this->simData, &callbackWrapper_}
{
    DEBUG_PRINT("Creating OptiX context");
    sim::OpenGLHelper::setDefaults();
    this->setCallbackClass(&callbacks_);

    renderer_.setDataFun([] {
        sim::PosNormTexData data;
        data.vbo = {{{-1, -1, 0}, {0, 0, 1}, {0, 1}},
                    {{1, -1, 0}, {0, 0, 1}, {1, 1}},
                    {{-1, 1, 0}, {0, 0, 1}, {0, 0}},
                    {{1, 1, 0}, {0, 0, 1}, {1, 0}}};
        data.vaoElements = sim::posNormTexVaoElements();
        return data;
    });

    renderer_.setDisplayMode(4);
    renderer_.onResize(this->getWidth(), this->getHeight());

    auto w = static_cast<unsigned>(this->getWidth());
    auto h = static_cast<unsigned>(this->getHeight());
    optixVbo_ = sim::OpenGLHelper::createBuffer<optix::float4>(0, w * h, GL_ARRAY_BUFFER, GL_STREAM_DRAW);
    texture_ = sim::OpenGLHelper::createTextureArray(w, h);

    optix::Buffer buffer{context()->createBufferFromGLBO(RT_BUFFER_OUTPUT, *optixVbo_)};
    buffer->setFormat(RT_FORMAT_FLOAT4);
    buffer->setSize(w, h);

    context()["output_buffer"]->set(buffer);

    // create scene first
    context()->validate();
    context()->compile();
}

template <typename Child>
void OptiXSimulation<Child>::update(const double worldTime, const double timeStep)
{
    updateChild(child_, worldTime, timeStep, 0);
}

template <typename Child>
void OptiXSimulation<Child>::render(const int width, const int height, const double alpha, const bool eventDriven)
{
    glViewport(0, 0, width, height);

    if (eventDriven) {
        ImGui_ImplGlfwGL3_NewFrame();
        renderChildGui(child_, width, height, 0);
        ImGui::Render();
    }

    ImGui_ImplGlfwGL3_NewFrame();
    renderChildGui(child_, width, height, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderChild(child_, width, height, alpha, context(), 0);
    ImGui::Render();
}

template <typename Child>
bool OptiXSimulation<Child>::paused() const
{
    return this->simData.paused;
}

template <typename Child>
template <typename T>
auto OptiXSimulation<Child>::updateChild(T &child, double worldTime, double timeStep, int)
    -> decltype(child.onUpdate(worldTime, timeStep), void())
{
    child.onUpdate(worldTime, timeStep);
}
template <typename Child>
template <typename T>
auto OptiXSimulation<Child>::updateChild(T &, double, double, long) -> decltype(void())
{
}

template <typename Child>
template <typename T>
auto OptiXSimulation<Child>::renderChild(T &child, int width, int height, double alpha, optix::Context &context, int)
    -> decltype(child.onRender(width, height, alpha, context), void())
{
    optix::Buffer buffer = context["output_buffer"]->getBuffer();
    RTsize buffer_width, buffer_height;
    buffer->getSize(buffer_width, buffer_height);

    child.onRender(width, height, alpha, context);

    RTsize elementSize = buffer->getElementSize();

    int alignmentSize = 1;

    if ((elementSize % 8) == 0) {
        alignmentSize = 8;
    } else if ((elementSize % 4) == 0) {
        alignmentSize = 4;
    } else if ((elementSize % 2) == 0) {
        alignmentSize = 2;
    }

    RTsize bufWidth, bufHeight;
    buffer->getSize(bufWidth, bufHeight);

    sim::OpenGLHelper::bindBufferToTexture(texture_,
                                           optixVbo_,
                                           alignmentSize,
                                           static_cast<int>(bufWidth),
                                           static_cast<int>(bufHeight));

    renderer_.onRender(static_cast<float>(alpha), nullptr);
}
template <typename Child>
template <typename T>
auto OptiXSimulation<Child>::renderChild(T &, int, int, double alpha, optix::Context &context, long) -> decltype(void())
{
    optix::Buffer buffer = context["output_buffer"]->getBuffer();
    RTsize buffer_width, buffer_height;
    buffer->getSize(buffer_width, buffer_height);

    context->launch(0, static_cast<unsigned>(buffer_width), static_cast<unsigned>(buffer_height));

    RTsize elementSize = buffer->getElementSize();

    int alignmentSize = 1;

    if ((elementSize % 8) == 0) {
        alignmentSize = 8;
    } else if ((elementSize % 4) == 0) {
        alignmentSize = 4;
    } else if ((elementSize % 2) == 0) {
        alignmentSize = 2;
    }

    RTsize bufWidth, bufHeight;
    buffer->getSize(bufWidth, bufHeight);

    sim::OpenGLHelper::bindBufferToTexture(texture_,
                                           optixVbo_,
                                           alignmentSize,
                                           static_cast<int>(bufWidth),
                                           static_cast<int>(bufHeight));

    renderer_.onRender(static_cast<float>(alpha), nullptr);
}

template <typename Child>
template <typename T>
auto OptiXSimulation<Child>::renderChildGui(T &child, int width, int height, int)
    -> decltype(child.onGuiRender(width, height), void())
{
    child.onGuiRender(width, height);
}
template <typename Child>
template <typename T>
auto OptiXSimulation<Child>::renderChildGui(T &, int, int, long) -> decltype(void())
{
}

template <typename Child>
void OptiXSimulation<Child>::framebufferSizeCallback(GLFWwindow *, int width, int height)
{
    optix::Buffer buffer = context()["output_buffer"]->getBuffer();

    auto uw = static_cast<unsigned>(width);
    auto uh = static_cast<unsigned>(height);

    optixVbo_ = nullptr;
    optixVbo_ = sim::OpenGLHelper::createBuffer<optix::float4>(0, uw * uh, GL_ARRAY_BUFFER, GL_STREAM_DRAW);

    sim::OpenGLHelper::resetTextureArray(texture_, width, height);

    buffer->setSize(uw, uh);

    renderer_.onResize(width, height);
}

template <typename Child>
optix::Context &OptiXSimulation<Child>::context()
{
    return *spContext_;
}

} // namespace sim