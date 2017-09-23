#pragma once

#include <sim-driver/OpenGLTypes.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <functional>

namespace sim
{

template<typename Vertex>
struct DrawData
{
    std::vector<Vertex> vbo;
    std::vector<unsigned> ibo;
    std::vector<sim::VAOElement> vaoElements;
};

template<typename Vertex>
class RendererHelper
{
public:
    using DataFun = std::function<DrawData<Vertex>(void)>;

    explicit RendererHelper();

    void onRender(float alpha, const Camera *pCamera) const;

    void onGuiRender();

    void onResize(int width, int height);

    void customRender(float alpha,
                      const Camera *pCamera,
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

    void setTexture(std::shared_ptr<GLuint> texture);

    int getFboWidth() const;
    int getFboHeight() const;
    bool isShowingVertsOnly() const;
    bool isUsingWireframe() const;
    bool isShowNormals() const;
    int getDisplayMode() const;
    const glm::vec3 &getShapeColor() const;
    float getShapeRoughness() const;
    const glm::vec3 &getShapeIor() const;
    const glm::vec3 &getLightDir() const;
    int getPointSize() const;
    float getNormalScale() const;
    GLenum getDrawMode() const;

    void setFboWidth(int fboWidth);
    void setFboHeight(int fboHeight);
    void setShowingVertsOnly(bool showingVertsOnly);
    void setUsingWireframe(bool usingWireframe);
    void setShowNormals(bool showNormals);
    void setDisplayMode(int displayMode);
    void setShapeColor(const glm::vec3 &shapeColor);
    void setShapeRoughness(float shapeRoughness);
    void setShapeIor(const glm::vec3 &shapeIor);
    void setLightDir(const glm::vec3 &lightDir);
    void setPointSize(int pointSize);
    void setNormalScale(float normalScale);
    void setDataFun(const DataFun &dataFun);
    void setDrawMode(GLenum drawMode);

    const glm::mat4 &getModelMatrix() const;
    void setModelMatrix(const glm::mat4 &modelMatrix);

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

    glm::mat4 modelMatrix_{1};
    glm::mat3 normalMatrix_{1};

    DataFun dataFun_{nullptr};
    GLenum drawMode_{GL_TRIANGLE_STRIP};

    void updateLights();
};

using PosNormTexRenderer = sim::RendererHelper<sim::PosNormTexVertex>;
using PosRenderer = sim::RendererHelper<sim::PosVertex>;

using PosNormTexData = sim::DrawData<sim::PosNormTexVertex>;
using PosData = sim::DrawData<sim::PosVertex>;

} // namespace sim
