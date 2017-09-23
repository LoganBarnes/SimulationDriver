// OpenGLHelper.hpp
#pragma once

#include <sim-driver/OpenGLTypes.hpp>

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <memory>

namespace sim
{

struct VAOElement
{
    std::string name;
    GLint size;
    GLenum type;
    void *pointer;
};

struct PosNormTexVertex
{
    float position[3];
    float normal[3];
    float texCoords[2];
};

struct PosVertex
{
    float position[3];
};

const std::vector<VAOElement> &posNormTexVaoElements();
const std::vector<VAOElement> &posVaoElements();
const unsigned &primitiveRestart();

typedef std::vector<std::shared_ptr<GLuint>> IdVec;

class OpenGLHelper
{

public:

    static void setDefaults();

    template<typename ... Shaders>
    static std::shared_ptr<GLuint> createProgram(std::string firstShader, Shaders ... shaders);

    static std::shared_ptr<GLuint> createProgram(std::vector<std::string> shaderFiles);

    template<typename ... Shaders>
    static SeparablePrograms createSeparablePrograms(std::string firstShader, Shaders ... shaders);

    static std::shared_ptr<GLuint> createTextureArray(GLsizei width,
                                                      GLsizei height,
                                                      float *pArray = nullptr,
                                                      GLint filterType = GL_NEAREST,
                                                      GLint wrapType = GL_REPEAT,
                                                      GLint internalFormat = GL_RGBA32F,
                                                      GLenum format = GL_RGBA);

    static void resetTextureArray(std::shared_ptr<GLuint> &spTexture,
                                  GLsizei width,
                                  GLsizei height,
                                  float *pArray = nullptr,
                                  GLint internalFormat = GL_RGBA32F,
                                  GLenum format = GL_RGBA);

    template<typename T>
    static std::shared_ptr<GLuint> createBuffer(const T *pData,
                                                const size_t numElements,
                                                const GLenum type = GL_ARRAY_BUFFER,
                                                const GLenum usage = GL_STATIC_DRAW);

    template<typename T>
    static void updateBuffer(const std::shared_ptr<GLuint> &spBuffer,
                             const size_t elementOffset,
                             const size_t numElements,
                             const T *pData,
                             const GLenum bufferType);

    static std::shared_ptr<GLuint> createVao(const std::shared_ptr<GLuint> &spProgram,
                                             const std::shared_ptr<GLuint> &spVbo,
                                             const GLsizei totalStride,
                                             const std::vector<VAOElement> &elements);

    static std::shared_ptr<GLuint> createFramebuffer(GLsizei width,
                                                     GLsizei height,
                                                     const std::shared_ptr<GLuint> &spColorTex = nullptr,
                                                     const std::shared_ptr<GLuint> &spDepthTex = nullptr);

    template<typename T>
    static StandardPipeline createStandardPipeline(const std::vector<std::string> &shaderFiles,
                                                   const T *pData,
                                                   const size_t numElements,
                                                   const GLsizei totalStride,
                                                   const std::vector<VAOElement> &elements,
                                                   const GLenum type = GL_ARRAY_BUFFER,
                                                   const GLenum usage = GL_STATIC_DRAW);

    static StandardPipeline createPosNormTexPipeline(const PosNormTexVertex *pData,
                                                     const size_t numElements,
                                                     std::vector<std::string> shaderFiles = {});

    static StandardPipeline createPosPipeline(const PosVertex *pData,
                                              const size_t numElements,
                                              std::vector<std::string> shaderFiles = {});

    static StandardPipeline createScreenspacePipeline();

    static void bindBufferToTexture(const std::shared_ptr<GLuint> &spTexture,
                                    const std::shared_ptr<GLuint> &spBuffer,
                                    int alignment,
                                    int width,
                                    int height);

    static void bindFramebuffer();

    static void bindFramebuffer(const std::shared_ptr<GLuint> &spFbo);

    static void clearFramebuffer();

    static void setTextureUniform(const std::shared_ptr<GLuint> &spProgram,
                                  const std::string &uniform,
                                  const std::shared_ptr<GLuint> &spTexture,
                                  int activeTex);

    static void setIntUniform(const std::shared_ptr<GLuint> &spProgram,
                              const std::string &uniform,
                              const int *pValue,
                              int size = 1,
                              int count = 1);

    static void setFloatUniform(const std::shared_ptr<GLuint> &spProgram,
                                const std::string &uniform,
                                const float *pValue,
                                int size = 1,
                                int count = 1);

    static void setMatrixUniform(const std::shared_ptr<GLuint> &spProgram,
                                 const std::string &uniform,
                                 const float *pValue,
                                 int size = 4,
                                 int count = 1);

    static void setSsboUniform(const std::shared_ptr<GLuint> &spProgram,
                               const std::shared_ptr<GLuint> &spSsbo,
                               const std::string &uniform,
                               int sizeBytes,
                               GLuint binding);

    static void renderBuffer(const std::shared_ptr<GLuint> &spVao,
                             const int start,
                             const int verts,
                             const GLenum mode,
                             const std::shared_ptr<GLuint> &spIbo = nullptr,
                             const void *pOffset = 0,
                             const GLenum iboType = GL_UNSIGNED_INT);

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template<typename T>
std::shared_ptr<GLuint> OpenGLHelper::createBuffer(const T *pData,
                                                   const size_t numElements,
                                                   const GLenum type,
                                                   const GLenum usage)
{
    std::shared_ptr<GLuint> upBuffer(new GLuint, [](auto pID)
    {
        glDeleteBuffers(1, pID);
        delete pID;
    });

    glGenBuffers(1, upBuffer.get());
    glBindBuffer(type, *upBuffer);
    glBufferData(type,
                 static_cast< GLsizeiptr >( numElements * sizeof(T)),
                 pData,
                 usage);

    glBindBuffer(type, 0);

    return upBuffer;
} // OpenGLHelper::addBuffer

template<typename T>
void OpenGLHelper::updateBuffer(const std::shared_ptr<GLuint> &upBuffer,
                                const size_t elementOffset,
                                const size_t numElements,
                                const T *pData,
                                const GLenum bufferType)
{
    constexpr auto typeSizeBytes = sizeof(T);

    glBindBuffer(bufferType, *upBuffer);
    glBufferSubData(bufferType,
                    static_cast< GLintptr >( elementOffset * typeSizeBytes ),
                    static_cast< GLsizeiptr >( numElements * typeSizeBytes ),
                    pData);

    glBindBuffer(bufferType, 0);
} // OpenGLHelper::updateBuffer

template<typename T>
StandardPipeline OpenGLHelper::createStandardPipeline(const std::vector<std::string> &shaderFiles,
                                                      const T *pData,
                                                      const size_t numElements,
                                                      const GLsizei totalStride,
                                                      const std::vector<VAOElement> &elements,
                                                      const GLenum type,
                                                      const GLenum usage)
{
    StandardPipeline glIds;
    glIds.program = OpenGLHelper::createProgram(shaderFiles);

    glIds.vbo = OpenGLHelper::createBuffer(pData,
                                           numElements,
                                           type,
                                           usage);

    glIds.vao = OpenGLHelper::createVao(glIds.program,
                                        glIds.vbo,
                                        totalStride,
                                        elements);
    glIds.vboSize = static_cast<int>(numElements);
    return glIds;
} // OpenGLHelper::createStandardPipeline

} // namespace sim
