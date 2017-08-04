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

    void customRender(float alpha,
                      const Camera &camera,
                      GLenum drawMode = GL_TRIANGLE_STRIP,
                      int displayMode = 5,
                      glm::vec3 shapeColor = glm::vec3{0.7},
                      glm::vec3 lightDir = glm::vec3{0.7f, 0.85f, 1.0f},
                      bool showNormals = false,
                      float NormalScale = 0.5f);

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
    void setShowNormals(bool showNormals);
    void setNormalScale(float normalScale);

    void setDrawMode(GLenum drawMode);

    const bool &getShowingVertsOnly() const;
    const bool &getUsingWireframe() const;
    const int &getDisplayMode() const;
    const glm::vec3 &getShapeColor() const;
    const glm::vec3 &getLightDir() const;
    const int &getPointSize() const;
    const bool &getShowNormals() const;
    const float &getNormalScale() const;

private:
    SeparablePipeline glIds_;

    int fboWidth_{0};
    int fboHeight_{0};

    bool showingVertsOnly_{false};
    bool usingWireframe_{false};
    bool showNormals_{false};

    int displayMode_{5};
    glm::vec3 shapeColor_{0.7f};
    glm::vec3 lightDir_{0.7, 0.85, 1.0};

    int pointSize_{1};
    float normalScale_{0.5f};

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
                                                                 sim::SHADER_PATH + "shader.geom",
                                                                 sim::SHADER_PATH + "shader.frag");

    glIds_.vbo = OpenGLHelper::createBuffer(vbo.data(), vbo.size());

    glIds_.vao = OpenGLHelper::createVao(glIds_.programs.vert,
                                         glIds_.vbo,
                                         sizeof(sim::PosNormTexVertex),
                                         sim::posNormTexVaoElements());
    glIds_.vboSize = static_cast<int>(vbo.size());

    glIds_.ibo = sim::OpenGLHelper::createBuffer<unsigned>(ibo.data(),
                                                           ibo.size(),
                                                           GL_ELEMENT_ARRAY_BUFFER);
    glIds_.iboSize = static_cast<int>(ibo.size());
}

template<typename T>
void RendererHelper2<T>::onRender(float alpha, const Camera &camera)
{
    if (showNormals_)
    {
        customRender(alpha, camera, drawMode_, 1, shapeColor_, lightDir_, showNormals_, normalScale_);
    }

    customRender(alpha, camera, drawMode_, displayMode_, shapeColor_, lightDir_, false);
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
    ImGui::Separator();

    ImGui::Checkbox("Show Normals", &showNormals_);

    if (showNormals_)
    {
        ImGui::SliderFloat("Normal Scale", &normalScale_, 0.1f, 1.5f);
    }
}

template<typename T>
void RendererHelper2<T>::onResize(int width, int height)
{}

template<typename T>
void RendererHelper2<T>::customRender(float alpha,
                                      const Camera &camera,
                                      GLenum drawMode,
                                      int displayMode,
                                      glm::vec3 shapeColor,
                                      glm::vec3 lightDir,
                                      bool showNormals,
                                      float NormalScale)
{
    if (glIds_.framebuffer)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, *glIds_.framebuffer);
        glViewport(0, 0, fboWidth_, fboHeight_);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    glUseProgram(0);
    glUseProgramStages(*glIds_.programs.pipeline, GL_VERTEX_SHADER_BIT, *glIds_.programs.vert);
    glUseProgramStages(*glIds_.programs.pipeline, GL_GEOMETRY_SHADER_BIT, showNormals ? *glIds_.programs.geom : 0);
    glUseProgramStages(*glIds_.programs.pipeline, GL_FRAGMENT_SHADER_BIT, *glIds_.programs.frag);
    glBindProgramPipeline(*glIds_.programs.pipeline);

    lightDir = glm::normalize(lightDir);
    sim::OpenGLHelper::setMatrixUniform(glIds_.programs.vert, "projectionView",
                                        glm::value_ptr(camera.getPerspectiveProjectionViewMatrix()));
    if (showNormals)
    {
        sim::OpenGLHelper::setMatrixUniform(glIds_.programs.geom, "projectionView",
                                            glm::value_ptr(camera.getPerspectiveProjectionViewMatrix()));
        sim::OpenGLHelper::setFloatUniform(glIds_.programs.geom, "normalScale", &NormalScale);
    }
    sim::OpenGLHelper::setIntUniform(glIds_.programs.frag, "displayMode", &displayMode);
    sim::OpenGLHelper::setFloatUniform(glIds_.programs.frag, "shapeColor", glm::value_ptr(shapeColor), 3);
    sim::OpenGLHelper::setFloatUniform(glIds_.programs.frag, "lightDir", glm::value_ptr(lightDir), 3);

    bool culling = glIsEnabled(GL_CULL_FACE) != 0;

    if (showingVertsOnly_ || showNormals)
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
    sim::OpenGLHelper::renderBuffer(glIds_.vao, 0, drawSize, drawMode, glIds_.ibo);

    if (culling)
    {
        glEnable(GL_CULL_FACE);
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

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
    if (0 > displayMode || displayMode > 8)
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
void RendererHelper2<T>::setPointSize(int pointSize)
{
    if (0 > pointSize || pointSize > max_point_size)
    {
        std::cerr << "Display mode should be between 0 and " << max_point_size << " inclusive" << std::endl;
        return;
    }
    pointSize_ = std::move(pointSize);
}

template<typename T>
void RendererHelper2<T>::setShowNormals(bool showNormals)
{ showNormals_ = showNormals; }

template<typename T>
void RendererHelper2<T>::setNormalScale(float normalScale)
{ normalScale_ = normalScale; }

template<typename T>
void RendererHelper2<T>::setDrawMode(GLenum drawMode)
{ drawMode_ = std::move(drawMode); }


template<typename T>
const bool &RendererHelper2<T>::getShowingVertsOnly() const
{ return showingVertsOnly_; }

template<typename T>
const bool &RendererHelper2<T>::getUsingWireframe() const
{ return usingWireframe_; }

template<typename T>
const int &RendererHelper2<T>::getDisplayMode() const
{ return displayMode_; }

template<typename T>
const glm::vec3 &RendererHelper2<T>::getShapeColor() const
{ return shapeColor_; }

template<typename T>
const glm::vec3 &RendererHelper2<T>::getLightDir() const
{ return lightDir_; }

template<typename T>
const int &RendererHelper2<T>::getPointSize() const
{ return pointSize_; }

template<typename T>
const bool &RendererHelper2<T>::getShowNormals() const
{ return showNormals_; }

template<typename T>
const float &RendererHelper2<T>::getNormalScale() const
{ return normalScale_; }

}
