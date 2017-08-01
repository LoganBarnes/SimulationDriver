#pragma once

#include <glad/glad.h>
#include <memory>

namespace sim
{

struct VAOElement;

struct VAOSettings;

class OpenGLHelper;

template<typename T>
class TCamera;

using Camera = TCamera<float>;
using CameraD = TCamera<double>;

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

} // namespace sim
