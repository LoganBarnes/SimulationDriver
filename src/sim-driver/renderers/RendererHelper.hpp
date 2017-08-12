#pragma once

#include <sim-driver/OpenGLHelper.hpp>
#include <sim-driver/cameras/Camera.hpp>
#include <ShaderConfig.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <imgui.h>
#include <functional>

namespace sim
{

constexpr int max_point_size = 25;

template<typename Vertex>
struct DrawData
{
    std::vector<Vertex> vbo;
    std::vector<unsigned> ibo;
    std::vector<sim::VAOElement> vaoElements;
    std::vector<std::pair<int, int>> drawCalls;
};

template<typename Vertex>
class RendererHelper
{
public:
    using DataFun = std::function<DrawData<Vertex>(void)>;

    explicit RendererHelper();

    void onRender(float alpha, const Camera &camera) const;
    void onGuiRender();
    void onResize(int width, int height);

    void customRender(float alpha,
                      const Camera &camera,
                      GLenum drawMode = GL_TRIANGLE_STRIP,
                      int displayMode = 5,
                      glm::vec3 shapeColor = glm::vec3{0.7},
                      glm::vec3 lightDir = glm::vec3{0.7f, 0.85f, 1.0f},
                      bool showNormals = false,
                      float NormalScale = 0.5f,
                      std::function<void(void)> programReplacement = nullptr) const;

    void renderToFramebuffer(int width,
                             int height,
                             const std::shared_ptr<GLuint> &spColorTex = nullptr,
                             const std::shared_ptr<GLuint> &spDepthTex = nullptr);

    void rebuild_mesh();

    void addLight(glm::vec3 lightDir, float intensity);

    void setShowingVertsOnly(bool showingVertsOnly);
    void setUsingWireframe(bool usingWireframe);
    void setDisplayMode(int displayMode);
    void setShapeColor(glm::vec3 shapeColor);
    void setLightDir(glm::vec3 lightDir);
    void setPointSize(int pointSize);
    void setShowNormals(bool showNormals);
    void setNormalScale(float normalScale);

    void setDataFun(DataFun dataFun);
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
    std::shared_ptr<GLuint> spCustomProgram_;
    std::shared_ptr<GLuint> spLightSsbo_{nullptr};
    std::vector<glm::vec4> lights_;

    int fboWidth_{0};
    int fboHeight_{0};

    bool showingVertsOnly_{false};
    bool usingWireframe_{false};
    bool showNormals_{false};

    int displayMode_{5};
    glm::vec3 shapeColor_{1.0f, 0.9f, 0.7f};
    float shapeRoughness_{0.2f};
    glm::vec3 shapeIor_{1.5145f, 1.5208f, 1.5232f}; // index of refraction
    glm::vec3 lightDir_{0.7, 0.85, 1.0};

    int pointSize_{1};
    float normalScale_{0.5f};

    DataFun dataFun_{nullptr};
    GLenum drawMode_{GL_TRIANGLE_STRIP};

    void updateLights();
};

template<typename Vertex>
RendererHelper<Vertex>::RendererHelper()
{
    glIds_.programs = sim::OpenGLHelper::createSeparablePrograms(sim::SHADER_PATH + "shader.vert",
                                                                 sim::SHADER_PATH + "shader.geom",
                                                                 sim::SHADER_PATH + "shader.frag");
}

template<typename Vertex>
void RendererHelper<Vertex>::onRender(float alpha, const Camera &camera) const
{
    if (showNormals_)
    {
        customRender(alpha, camera, drawMode_, 1, shapeColor_, lightDir_, showNormals_, normalScale_);
    }

    customRender(alpha, camera, drawMode_, displayMode_, shapeColor_, lightDir_, false);
}

template<typename Vertex>
void RendererHelper<Vertex>::onGuiRender()
{
    ImGui::Checkbox("Show Normals", &showNormals_);

    if (showNormals_)
    {
        ImGui::SliderFloat("Normal Scale", &normalScale_, 0.1f, 1.5f);
    }

    ImGui::Checkbox("Show Verts Only", &showingVertsOnly_);

    if (showingVertsOnly_)
    {
        ImGui::SliderInt("Point Size", &pointSize_, 0, max_point_size);
    }
    else
    {
        ImGui::Checkbox("Wireframe", &usingWireframe_);
    }

    ImGui::Separator();

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
        else if (displayMode_ == 6)
        {
            ImGui::SliderFloat("Shape Roughness", &shapeRoughness_, 0.01f, 1.0f);
            ImGui::SliderFloat3("Shape Index of Refraction", glm::value_ptr(shapeIor_), 1.0f, 10.0f);

            if (ImGui::Button("Add Light"))
            {
                addLight(glm::vec3{1}, 1);
            }

            if (ImGui::CollapsingHeader("Lights", "lights", false, true))
            {
                bool lights_need_update = false;

                for (int i = 0; i < lights_.size(); ++i)
                {
                    std::string light_str = "Light " + std::to_string(i);
                    glm::vec4 &light = lights_[i];
                    lights_need_update |= ImGui::SliderFloat3(std::string(light_str + " Direction").c_str(),
                                                              glm::value_ptr(light),
                                                              -1,
                                                              1);
                    lights_need_update |= ImGui::SliderFloat(std::string(light_str + " Intensity").c_str(), &light.w, 0,
                                                             1);
                    ImGui::Separator();
                }

                if (lights_need_update)
                {
                    updateLights();
                }
            }
        }
    }
}

template<typename Vertex>
void RendererHelper<Vertex>::onResize(int width, int height)
{}

template<typename Vertex>
void RendererHelper<Vertex>::customRender(float alpha,
                                          const Camera &camera,
                                          GLenum drawMode,
                                          int displayMode,
                                          glm::vec3 shapeColor,
                                          glm::vec3 lightDir,
                                          bool showNormals,
                                          float NormalScale,
                                          std::function<void(void)> programReplacement) const
{
    if (glIds_.framebuffer)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, *glIds_.framebuffer);
        glViewport(0, 0, fboWidth_, fboHeight_);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    if (programReplacement)
    {
        programReplacement();
    }
    else
    {
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
        sim::OpenGLHelper::setFloatUniform(glIds_.programs.frag, "roughness", &shapeRoughness_);
        sim::OpenGLHelper::setFloatUniform(glIds_.programs.frag, "IOR", glm::value_ptr(shapeIor_), 3);
        sim::OpenGLHelper::setFloatUniform(glIds_.programs.frag, "eye", glm::value_ptr(camera.getEyeVector()), 3);

        if (spLightSsbo_)
        {
            sim::OpenGLHelper::setSsboUniform(glIds_.programs.frag,
                                              spLightSsbo_,
                                              "lightData",
                                              static_cast<int>(sizeof(lights_[0]) * lights_.size()),
                                              0);
        }
    }

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

template<typename Vertex>
void RendererHelper<Vertex>::renderToFramebuffer(int width,
                                                 int height,
                                                 const std::shared_ptr<GLuint> &spColorTex,
                                                 const std::shared_ptr<GLuint> &spDepthTex)
{
    fboWidth_ = std::move(width);
    fboHeight_ = std::move(height);
    glIds_.framebuffer = nullptr;
    glIds_.framebuffer = OpenGLHelper::createFramebuffer(fboWidth_, fboHeight_, spColorTex, spDepthTex);
}

template<typename Vertex>
void RendererHelper<Vertex>::rebuild_mesh()
{
    if (!dataFun_)
    {
        return;
    }

    glIds_.vbo = glIds_.vao = glIds_.ibo = nullptr;

    auto data = dataFun_();
    glIds_.vbo = OpenGLHelper::createBuffer(data.vbo.data(), data.vbo.size());

    glIds_.vao = OpenGLHelper::createVao(glIds_.programs.vert,
                                         glIds_.vbo,
                                         sizeof(Vertex),
                                         data.vaoElements);
    glIds_.vboSize = static_cast<int>(data.vbo.size());

    glIds_.ibo = sim::OpenGLHelper::createBuffer<unsigned>(data.ibo.data(),
                                                           data.ibo.size(),
                                                           GL_ELEMENT_ARRAY_BUFFER);
    glIds_.iboSize = static_cast<int>(data.ibo.size());
}

template<typename Vertex>
void RendererHelper<Vertex>::addLight(glm::vec3 lightDir, float intensity)
{
    lights_.emplace_back(lightDir, intensity);

    spLightSsbo_ = nullptr;
    spLightSsbo_ = sim::OpenGLHelper::createBuffer(lights_.data(),
                                                   lights_.size(),
                                                   GL_SHADER_STORAGE_BUFFER,
                                                   GL_DYNAMIC_DRAW);
}

template<typename Vertex>
void RendererHelper<Vertex>::setShowingVertsOnly(bool showingVertsOnly)
{ showingVertsOnly_ = std::move(showingVertsOnly); }

template<typename Vertex>
void RendererHelper<Vertex>::setUsingWireframe(bool usingWireframe)
{ usingWireframe_ = std::move(usingWireframe); }

template<typename Vertex>
void RendererHelper<Vertex>::setDisplayMode(int displayMode)
{
    if (0 > displayMode || displayMode > 8)
    {
        std::cerr << "Display mode should be between 0 and 7 inclusive" << std::endl;
        return;
    }
    displayMode_ = std::move(displayMode);
}

template<typename Vertex>
void RendererHelper<Vertex>::setShapeColor(glm::vec3 shapeColor)
{ shapeColor_ = std::move(shapeColor); }

template<typename Vertex>
void RendererHelper<Vertex>::setLightDir(glm::vec3 lightDir)
{ lightDir_ = std::move(lightDir); }

template<typename Vertex>
void RendererHelper<Vertex>::setPointSize(int pointSize)
{
    if (0 > pointSize || pointSize > max_point_size)
    {
        std::cerr << "Display mode should be between 0 and " << max_point_size << " inclusive" << std::endl;
        return;
    }
    pointSize_ = std::move(pointSize);
}

template<typename Vertex>
void RendererHelper<Vertex>::setShowNormals(bool showNormals)
{ showNormals_ = showNormals; }

template<typename Vertex>
void RendererHelper<Vertex>::setNormalScale(float normalScale)
{ normalScale_ = normalScale; }

template<typename Vertex>
void RendererHelper<Vertex>::setDataFun(DataFun dataFun)
{
    dataFun_ = dataFun;
    rebuild_mesh();
}

template<typename Vertex>
void RendererHelper<Vertex>::setDrawMode(GLenum drawMode)
{ drawMode_ = drawMode; }

template<typename Vertex>
const bool &RendererHelper<Vertex>::getShowingVertsOnly() const
{ return showingVertsOnly_; }

template<typename Vertex>
const bool &RendererHelper<Vertex>::getUsingWireframe() const
{ return usingWireframe_; }

template<typename Vertex>
const int &RendererHelper<Vertex>::getDisplayMode() const
{ return displayMode_; }

template<typename Vertex>
const glm::vec3 &RendererHelper<Vertex>::getShapeColor() const
{ return shapeColor_; }

template<typename Vertex>
const glm::vec3 &RendererHelper<Vertex>::getLightDir() const
{ return lightDir_; }

template<typename Vertex>
const int &RendererHelper<Vertex>::getPointSize() const
{ return pointSize_; }

template<typename Vertex>
const bool &RendererHelper<Vertex>::getShowNormals() const
{ return showNormals_; }

template<typename Vertex>
const float &RendererHelper<Vertex>::getNormalScale() const
{ return normalScale_; }

template<typename Vertex>
void RendererHelper<Vertex>::updateLights()
{
    sim::OpenGLHelper::updateBuffer(spLightSsbo_, 0, lights_.size(), lights_.data(), GL_SHADER_STORAGE_BUFFER);
}

using PosNormTexRenderer = sim::RendererHelper<sim::PosNormTexVertex>;
using PosRenderer = sim::RendererHelper<sim::PosVertex>;

using PosNormTexData = sim::DrawData<sim::PosNormTexVertex>;
using PosData = sim::DrawData<sim::PosVertex>;

} // namespace sim
