#pragma  once

#include <sim-driver/OpenGLForwardDeclarations.hpp>

namespace sim
{

class Renderer
{
public:
    explicit Renderer() = default;
    virtual ~Renderer() = default;
    Renderer(const Renderer &) = default;
    Renderer(Renderer &&) noexcept = default;
    Renderer &operator=(const Renderer &) = default;
    Renderer &operator=(Renderer &&) noexcept = default;

    virtual void onRender(float alpha, const Camera &camera) = 0;
    virtual void onGuiRender() = 0;
    virtual void onResize(int width, int height) = 0;
};

} // namespace sim
