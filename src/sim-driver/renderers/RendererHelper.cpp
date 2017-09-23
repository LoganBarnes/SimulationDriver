#include <sim-driver/renderers/RendererHelper.hpp>
#include <sim-driver/OpenGLHelper.hpp>
#include <sim-driver/Camera.hpp>
#include <ShaderConfig.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <imgui.h>
#include <functional>

namespace sim
{

namespace
{
constexpr int max_point_size = 25;
}

template<typename Vertex>
RendererHelper<Vertex>::RendererHelper()
{
    glIds_.programs = sim::OpenGLHelper::createSeparablePrograms(sim::SHADER_PATH + "shader.vert",
                                                                 sim::SHADER_PATH + "shader.geom",
#ifdef __APPLE__
                                                                 sim::SHADER_PATH + "shader_mac.frag");
#else
    sim::SHADER_PATH + "shader.frag");
#endif
}

template<typename Vertex>
void
RendererHelper<Vertex>::onRender(float alpha, const Camera *pCamera) const
{
    if (showNormals_) {
        customRender(alpha, pCamera, drawMode_, 1, shapeColor_, lightDir_, showNormals_, normalScale_);
    }

    customRender(alpha, pCamera, drawMode_, displayMode_, shapeColor_, lightDir_, false);
}

template<typename Vertex>
void
RendererHelper<Vertex>::onGuiRender()
{
    ImGui::Checkbox("Show Normals", &showNormals_);

    if (showNormals_) {
        ImGui::SliderFloat("Normal Scale", &normalScale_, 0.1f, 1.5f);
    }

    ImGui::Checkbox("Show Verts Only", &showingVertsOnly_);

    if (showingVertsOnly_) {
        ImGui::SliderInt("Point Size", &pointSize_, 1, max_point_size);
    }
    else {
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

    if (displayMode_ == 3 || displayMode_ == 5 || displayMode_ == 6) {
        ImGui::ColorEdit3("Shape Color", glm::value_ptr(shapeColor_));

        if (displayMode_ == 5) {
            ImGui::SliderFloat3("Light Direction", glm::value_ptr(lightDir_), -1, 1);
        }
        else if (displayMode_ == 6) {
            ImGui::SliderFloat("Shape Roughness", &shapeRoughness_, 0.01f, 1.0f);
            ImGui::SliderFloat3("Shape Index of Refraction", glm::value_ptr(shapeIor_), 1.0f, 10.0f);

#ifdef __APPLE__
            ImGui::SliderFloat3("Light Direction", glm::value_ptr(lightDir_), -1, 1);
#else
            if (ImGui::Button("Add Light"))
        {
            addLight(glm::vec3{1}, 1);
        }

        if (ImGui::CollapsingHeader("Lights", "lights", false, true))
        {
            bool lights_need_update = false;

            for (decltype(lights_.size()) i = 0; i < lights_.size(); ++i)
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
#endif
        }
    }
}

template<typename Vertex>
void
RendererHelper<Vertex>::onResize(int, int)
{
    if (glIds_.framebuffer) {
        ///\todo: resize framebuffer
    }
}

template<typename Vertex>
void
RendererHelper<Vertex>::customRender(float,
                                     const Camera *pCamera,
                                     GLenum drawMode,
                                     int displayMode,
                                     glm::vec3 shapeColor,
                                     glm::vec3 lightDir,
                                     bool showNormals,
                                     float NormalScale,
                                     std::function<void(void)> programReplacement) const
{
    if (glIds_.framebuffer) {
        glBindFramebuffer(GL_FRAMEBUFFER, *glIds_.framebuffer);
        glViewport(0, 0, fboWidth_, fboHeight_);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    if (programReplacement) {
        programReplacement();
    }
    else {
        glUseProgram(0);
        glUseProgramStages(*glIds_.programs.pipeline, GL_VERTEX_SHADER_BIT, *glIds_.programs.vert);
        glUseProgramStages(*glIds_.programs.pipeline, GL_GEOMETRY_SHADER_BIT, showNormals ? *glIds_.programs.geom : 0);
        glUseProgramStages(*glIds_.programs.pipeline, GL_FRAGMENT_SHADER_BIT, *glIds_.programs.frag);
        glBindProgramPipeline(*glIds_.programs.pipeline);

        lightDir = glm::normalize(lightDir);
        if (pCamera != nullptr) {
            sim::OpenGLHelper::setMatrixUniform(glIds_.programs.vert, "screen_from_world",
                                                glm::value_ptr(pCamera->getPerspectiveScreenFromWorldMatrix()));
            sim::OpenGLHelper::setFloatUniform(glIds_.programs.frag, "eye", glm::value_ptr(pCamera->getEyeVector()), 3);
        }
//        sim::OpenGLHelper::setMatrixUniform(glIds_.programs.vert, "world_from_local",
//                                            glm::value_ptr(modelMatrix_));
//        sim::OpenGLHelper::setMatrixUniform(glIds_.programs.vert, "world_from_local_normals",
//                                            glm::value_ptr(normalMatrix_), 3);

        if (showNormals) {
            if (pCamera != nullptr) {
                sim::OpenGLHelper::setMatrixUniform(glIds_.programs.geom, "screen_from_world",
                                                    glm::value_ptr(pCamera->getPerspectiveScreenFromWorldMatrix()));
            }
            sim::OpenGLHelper::setFloatUniform(glIds_.programs.geom, "normal_scale", &NormalScale);
        }

        if (glIds_.texture) {
            sim::OpenGLHelper::setTextureUniform(glIds_.programs.frag, "tex", glIds_.texture, 0);
        }

        sim::OpenGLHelper::setIntUniform(glIds_.programs.frag, "displayMode", &displayMode);
        sim::OpenGLHelper::setFloatUniform(glIds_.programs.frag, "shapeColor", glm::value_ptr(shapeColor), 3);
        sim::OpenGLHelper::setFloatUniform(glIds_.programs.frag, "lightDir", glm::value_ptr(lightDir), 3);
        sim::OpenGLHelper::setFloatUniform(glIds_.programs.frag, "roughness", &shapeRoughness_);
        sim::OpenGLHelper::setFloatUniform(glIds_.programs.frag, "IOR", glm::value_ptr(shapeIor_), 3);

        if (spLightSsbo_) {
            sim::OpenGLHelper::setSsboUniform(glIds_.programs.frag,
                                              spLightSsbo_,
                                              "lightData",
                                              static_cast<int>(sizeof(lights_[0]) * lights_.size()),
                                              0);
        }
    }

    bool culling = glIsEnabled(GL_CULL_FACE) != 0;

    if (showingVertsOnly_ || showNormals) {
        glPointSize(static_cast<float>(pointSize_));
        sim::OpenGLHelper::renderBuffer(glIds_.vao, 0, glIds_.vboSize, GL_POINTS);
        glPointSize(1);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    if (usingWireframe_) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_CULL_FACE);
    }

    int drawSize = glIds_.ibo ? glIds_.iboSize : glIds_.vboSize;
    sim::OpenGLHelper::renderBuffer(glIds_.vao, 0, drawSize, drawMode, glIds_.ibo);

    if (culling) {
        glEnable(GL_CULL_FACE);
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

template<typename Vertex>
void
RendererHelper<Vertex>::renderToFramebuffer(int width,
                                            int height,
                                            const std::shared_ptr<GLuint> &spColorTex,
                                            const std::shared_ptr<GLuint> &spDepthTex)
{
    fboWidth_ = width;
    fboHeight_ = height;
    glIds_.framebuffer = nullptr;
    glIds_.framebuffer = OpenGLHelper::createFramebuffer(fboWidth_, fboHeight_, spColorTex, spDepthTex);
}

template<typename Vertex>
void
RendererHelper<Vertex>::rebuild_mesh()
{
    if (!dataFun_) {
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

    if (!data.ibo.empty()) {
        glIds_.ibo = sim::OpenGLHelper::createBuffer<unsigned>(data.ibo.data(),
                                                               data.ibo.size(),
                                                               GL_ELEMENT_ARRAY_BUFFER);
        glIds_.iboSize = static_cast<int>(data.ibo.size());
    }
}

template<typename Vertex>
void
RendererHelper<Vertex>::addLight(glm::vec3 lightDir, float intensity)
{
    lights_.emplace_back(lightDir, intensity);

    spLightSsbo_ = nullptr;
    spLightSsbo_ = sim::OpenGLHelper::createBuffer(lights_.data(),
                                                   lights_.size(),
                                                   GL_SHADER_STORAGE_BUFFER,
                                                   GL_DYNAMIC_DRAW);
}

template<typename Vertex>
void
RendererHelper<Vertex>::setTexture(std::shared_ptr<GLuint> texture)
{
    glIds_.texture = texture;
}

template<typename Vertex>
int
RendererHelper<Vertex>::getFboWidth() const
{ return fboWidth_; }
template<typename Vertex>
int
RendererHelper<Vertex>::getFboHeight() const
{ return fboHeight_; }
template<typename Vertex>
bool
RendererHelper<Vertex>::isShowingVertsOnly() const
{ return showingVertsOnly_; }
template<typename Vertex>
bool
RendererHelper<Vertex>::isUsingWireframe() const
{ return usingWireframe_; }
template<typename Vertex>
bool
RendererHelper<Vertex>::isShowNormals() const
{ return showNormals_; }
template<typename Vertex>
int
RendererHelper<Vertex>::getDisplayMode() const
{ return displayMode_; }
template<typename Vertex>
const glm::vec3 &
RendererHelper<Vertex>::getShapeColor() const
{ return shapeColor_; }
template<typename Vertex>
float
RendererHelper<Vertex>::getShapeRoughness() const
{ return shapeRoughness_; }
template<typename Vertex>
const glm::vec3 &
RendererHelper<Vertex>::getShapeIor() const
{ return shapeIor_; }
template<typename Vertex>
const glm::vec3 &
RendererHelper<Vertex>::getLightDir() const
{ return lightDir_; }
template<typename Vertex>
int
RendererHelper<Vertex>::getPointSize() const
{ return pointSize_; }
template<typename Vertex>
float
RendererHelper<Vertex>::getNormalScale() const
{ return normalScale_; }
template<typename Vertex>
GLenum
RendererHelper<Vertex>::getDrawMode() const
{ return drawMode_; }

template<typename Vertex>
void
RendererHelper<Vertex>::setFboWidth(int fboWidth)
{ fboWidth_ = fboWidth; }
template<typename Vertex>
void
RendererHelper<Vertex>::setFboHeight(int fboHeight)
{ fboHeight_ = fboHeight; }
template<typename Vertex>
void
RendererHelper<Vertex>::setShowingVertsOnly(bool showingVertsOnly)
{ showingVertsOnly_ = showingVertsOnly; }
template<typename Vertex>
void
RendererHelper<Vertex>::setUsingWireframe(bool usingWireframe)
{ usingWireframe_ = usingWireframe; }
template<typename Vertex>
void
RendererHelper<Vertex>::setShowNormals(bool showNormals)
{ showNormals_ = showNormals; }
template<typename Vertex>
void
RendererHelper<Vertex>::setDisplayMode(int displayMode)
{
    if (0 > displayMode || displayMode > 8) {
        std::cerr << "Display mode should be between 0 and 8 inclusive" << std::endl;
        return;
    }
    displayMode_ = displayMode;
}
template<typename Vertex>
void
RendererHelper<Vertex>::setShapeColor(const glm::vec3 &shapeColor)
{ shapeColor_ = shapeColor; }
template<typename Vertex>
void
RendererHelper<Vertex>::setShapeRoughness(float shapeRoughness)
{ shapeRoughness_ = shapeRoughness; }
template<typename Vertex>
void
RendererHelper<Vertex>::setShapeIor(const glm::vec3 &shapeIor)
{ shapeIor_ = shapeIor; }
template<typename Vertex>
void
RendererHelper<Vertex>::setLightDir(const glm::vec3 &lightDir)
{ lightDir_ = lightDir; }
template<typename Vertex>
void
RendererHelper<Vertex>::setPointSize(int pointSize)
{
    if (0 > pointSize || pointSize > max_point_size) {
        std::cerr << "Display mode should be between 0 and " << max_point_size << " inclusive" << std::endl;
        return;
    }
    pointSize_ = pointSize;
}
template<typename Vertex>
void
RendererHelper<Vertex>::setNormalScale(float normalScale)
{ normalScale_ = normalScale; }
template<typename Vertex>
void
RendererHelper<Vertex>::setDataFun(const DataFun &dataFun)
{
    dataFun_ = dataFun;
    rebuild_mesh();
}
template<typename Vertex>
void
RendererHelper<Vertex>::setDrawMode(GLenum drawMode)
{ drawMode_ = drawMode; }

template<typename Vertex>
void
RendererHelper<Vertex>::updateLights()
{
    sim::OpenGLHelper::updateBuffer(spLightSsbo_, 0, lights_.size(), lights_.data(), GL_SHADER_STORAGE_BUFFER);
}

template<typename Vertex>
const glm::mat4 &
RendererHelper<Vertex>::getModelMatrix() const
{
    return modelMatrix_;
}

template<typename Vertex>
void
RendererHelper<Vertex>::setModelMatrix(const glm::mat4 &modelMatrix)
{
    modelMatrix_ = modelMatrix;
    normalMatrix_ = glm::transpose(glm::inverse(glm::mat3(modelMatrix_)));
}

template
class sim::RendererHelper<sim::PosNormTexVertex>;

template
class sim::RendererHelper<sim::PosVertex>;

template
struct sim::DrawData<sim::PosNormTexVertex>;

template
struct sim::DrawData<sim::PosVertex>;

} // namespace sim
