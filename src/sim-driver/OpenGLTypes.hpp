#pragma once

#include <glad/glad.h>
#include <memory>

namespace sim {

struct VAOElement;
struct VAOSettings;

class OpenGLHelper;

template <typename T>
class TCamera;

using Camera = TCamera<float>;
using CameraD = TCamera<double>;

template <typename T>
class RendererHelper;

template <typename T>
struct DrawData;

struct PosNormTexVertex;
struct PosVertex;

using PosNormTexRenderer = sim::RendererHelper<sim::PosNormTexVertex>;
using PosRenderer = sim::RendererHelper<sim::PosVertex>;

using PosNormTexData = sim::DrawData<sim::PosNormTexVertex>;
using PosData = sim::DrawData<sim::PosVertex>;

struct StandardPipeline
{
    std::shared_ptr<GLuint> program;
    std::shared_ptr<GLuint> vbo;
    std::shared_ptr<GLuint> ibo;
    std::shared_ptr<GLuint> vao;
    std::shared_ptr<GLuint> texture;
    std::shared_ptr<GLuint> framebuffer;
    int vboSize;
    int iboSize;
};

struct SeparablePrograms
{
    std::shared_ptr<GLuint> pipeline;
    std::shared_ptr<GLuint> vert;
    std::shared_ptr<GLuint> tesc;
    std::shared_ptr<GLuint> tese;
    std::shared_ptr<GLuint> geom;
    std::shared_ptr<GLuint> frag;
    std::shared_ptr<GLuint> comp;
};

struct SeparablePipeline
{
    SeparablePrograms programs;
    std::shared_ptr<GLuint> vbo;
    std::shared_ptr<GLuint> ibo;
    std::shared_ptr<GLuint> vao;
    std::shared_ptr<GLuint> texture;
    std::shared_ptr<GLuint> framebuffer;
    int vboSize;
    int iboSize;
};

} // namespace sim
