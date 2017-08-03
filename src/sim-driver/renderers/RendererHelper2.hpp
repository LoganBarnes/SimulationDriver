#pragma once

#include <sim-driver/renderers/Renderer.hpp>
#include <sim-driver/OpenGLHelper.hpp>
#include <sim-driver/cameras/Camera.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <imgui.h>
#include "../../../cmake-build-release/ShaderConfig.hpp"

namespace sim
{

constexpr int max_point_size = 25;

template<typename Vertex>
class RendererHelper2 : public Renderer
{
public:
    explicit RendererHelper2();

    void onRender(float alpha, const Camera &camera) override;
    void onGuiRender() override;
    void onResize(int width, int height) override;

    void renderToFramebuffer(int width,
                             int height,
                             const std::shared_ptr<GLuint> &spColorTex = nullptr,
                             const std::shared_ptr<GLuint> &spDepthTex = nullptr);

    void setShowingVertsOnly(bool showingVertsOnly);
    void setUsingWireframe(bool usingWireframe);
    void setDisplayMode(int displayMode);
    void setShapeColor(glm::vec3 shapeColor);
    void setLightDir(glm::vec3 lightDir);
    void setPointSize(int pointSize);

    void setDrawMode(GLenum drawMode);

    const bool &getShowingVertsOnly();
    const bool &getUsingWireframe();
    const int &getDisplayMode();
    const glm::vec3 &getShapeColor();
    const glm::vec3 &getLightDir();
    const int &getPointSize();

private:
    SeparablePipeline glIds_;

    int fboWidth_{0};
    int fboHeight_{0};

    bool showingVertsOnly_{false};
    bool usingWireframe_{false};

    int displayMode_{5};
    glm::vec3 shapeColor_{0.7f};
    glm::vec3 lightDir_{0.7, 0.85, 1.0};

    int pointSize_{1};

    GLenum drawMode_{GL_TRIANGLE_STRIP};
};

template<typename T>
RendererHelper2<T>::RendererHelper2()
{
    float s2 = 1.f / std::sqrt(2.0f);
    std::vector<sim::PosNormTexVertex> vbo{
        {{0,   0,   1},  {0,   0,   1},  {0.5f, 0.5f}},
        {{-s2, s2,  0},  {-s2, s2,  0},  {0,    1}},
        {{-s2, -s2, 0},  {-s2, -s2, 0},  {0,    0}},
        {{s2,  -s2, 0},  {s2,  -s2, 0},  {1,    0}},
        {{s2,  s2,  0},  {s2,  s2,  0},  {1,    1}},
        {{0,   0,   -1}, {0,   0,   -1}, {0.5f, 0.5f}},
    };

    std::vector<unsigned> ibo{
        0, 1, 2, 3, 4, 1, 0xFFFFFFFF, 5, 4, 3, 2, 1, 4
    };

    glIds_.programs = sim::OpenGLHelper::createSeparablePrograms(sim::SHADER_PATH + "shader.vert",
                                                                 sim::SHADER_PATH + "shader.frag");

    glIds_.ibo = sim::OpenGLHelper::createBuffer<unsigned>(ibo.data(),
                                                           ibo.size(),
                                                           GL_ELEMENT_ARRAY_BUFFER);
    glIds_.iboSize = static_cast<int>(ibo.size());
}

template<typename T>
void RendererHelper2<T>::onRender(float alpha, const Camera &camera)
{
    if (glIds_.framebuffer)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, *glIds_.framebuffer);
        glViewport(0, 0, fboWidth_, fboHeight_);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    glUseProgram(*glIds_.program);
    glm::vec3 lightDir{glm::normalize(lightDir_)};
    sim::OpenGLHelper::setMatrixUniform(glIds_.program,
                                        "projectionView",
                                        glm::value_ptr(camera.getPerspectiveProjectionViewMatrix()));
    sim::OpenGLHelper::setIntUniform(glIds_.program, "displayMode", &displayMode_);
    sim::OpenGLHelper::setFloatUniform(glIds_.program, "shapeColor", glm::value_ptr(shapeColor_), 3);
    sim::OpenGLHelper::setFloatUniform(glIds_.program, "lightDir", glm::value_ptr(lightDir), 3);

    bool culling = glIsEnabled(GL_CULL_FACE) != 0;

    if (showingVertsOnly_)
    {
        glPointSize(static_cast<float>(pointSize_));
        sim::OpenGLHelper::renderBuffer(glIds_.vao, 0, glIds_.vboSize, GL_POINTS);
        glPointSize(1);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }
    else if (usingWireframe_)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_CULL_FACE);
    }

    int drawSize = glIds_.ibo ? glIds_.iboSize : glIds_.vboSize;
    sim::OpenGLHelper::renderBuffer(glIds_.vao, 0, drawSize, drawMode_, glIds_.ibo);

    if (culling)
    {
        glEnable(GL_CULL_FACE);
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

template<typename T>
void RendererHelper2<T>::onGuiRender()
{
    ImGui::Checkbox("Show Verts Only", &showingVertsOnly_);

    if (showingVertsOnly_)
    {
        ImGui::SliderInt("Point Size", &pointSize_, 0, max_point_size);
    }
    else
    {
        ImGui::Checkbox("Wireframe", &usingWireframe_);
    }

    ImGui::Combo("Display Mode", &displayMode_,
                 " Position \0"
                     " Normal \0"
                     " Tex Coords \0"
                     " Color \0"
                     " Texture \0"
                     " Simple Shading \0"
                     " Advanced Shading \0"
                     " White \0"
                     "\0\0");

    if (displayMode_ == 3 || displayMode_ == 5 || displayMode_ == 6)
    {
        ImGui::ColorEdit3("Shape Color", glm::value_ptr(shapeColor_));

        if (displayMode_ == 5)
        {
            ImGui::SliderFloat3("Light Direction", glm::value_ptr(lightDir_), -1, 1);
        }
    }
}

template<typename T>
void RendererHelper2<T>::onResize(int width, int height)
{}

template<typename T>
void RendererHelper2<T>::renderToFramebuffer(int width,
                                             int height,
                                             const std::shared_ptr<GLuint> &spColorTex,
                                             const std::shared_ptr<GLuint> &spDepthTex)
{
    fboWidth_ = std::move(width);
    fboHeight_ = std::move(height);
    glIds_.framebuffer = nullptr;
    glIds_.framebuffer = OpenGLHelper::createFramebuffer(fboWidth_, fboHeight_, spColorTex, spDepthTex);
}


template<typename T>
void RendererHelper2<T>::setShowingVertsOnly(bool showingVertsOnly)
{ showingVertsOnly_ = std::move(showingVertsOnly); }

template<typename T>
void RendererHelper2<T>::setUsingWireframe(bool usingWireframe)
{ usingWireframe_ = std::move(usingWireframe); }

template<typename T>
void RendererHelper2<T>::setDisplayMode(int displayMode)
{
    if (0 > displayMode || displayMode < 8)
    {
        std::cerr << "Display mode should be between 0 and 7 inclusive" << std::endl;
        return;
    }
    displayMode_ = std::move(displayMode);
}

template<typename T>
void RendererHelper2<T>::setShapeColor(glm::vec3 shapeColor)
{ shapeColor_ = std::move(shapeColor); }

template<typename T>
void RendererHelper2<T>::setLightDir(glm::vec3 lightDir)
{ lightDir_ = std::move(lightDir); }

template<typename T>
void RendererHelper2<T>::setDrawMode(GLenum drawMode)
{ drawMode_ = std::move(drawMode); }

template<typename T>
void RendererHelper2<T>::setPointSize(int pointSize)
{
    if (0 > pointSize || pointSize < max_point_size)
    {
        std::cerr << "Display mode should be between 0 and " << max_point_size << " inclusive" << std::endl;
        return;
    }
    pointSize_ = std::move(pointSize);
}


template<typename T>
const bool &RendererHelper2<T>::getShowingVertsOnly()
{ return showingVertsOnly_; }

template<typename T>
const bool &RendererHelper2<T>::getUsingWireframe()
{ return usingWireframe_; }

template<typename T>
const int &RendererHelper2<T>::getDisplayMode()
{ return displayMode_; }

template<typename T>
const glm::vec3 &RendererHelper2<T>::getShapeColor()
{ return shapeColor_; }

template<typename T>
const glm::vec3 &RendererHelper2<T>::getLightDir()
{ return lightDir_; }

template<typename T>
const int &RendererHelper2<T>::getPointSize()
{ return pointSize_; }


}
