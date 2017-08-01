#include <sim-driver/OpenGLHelper.hpp>

#include "ShaderConfig.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <stdexcept>

#ifdef _WIN32
#undef max
#endif


namespace sim
{


namespace
{

const std::unordered_map<std::string, GLenum> &shaderTypes()
{
    static std::unordered_map<std::string, GLenum> extMap{
        {".vert", GL_VERTEX_SHADER},
        {".tesc", GL_TESS_CONTROL_SHADER},
        {".tese", GL_TESS_EVALUATION_SHADER},
        {".geom", GL_GEOMETRY_SHADER},
        {".frag", GL_FRAGMENT_SHADER},
        {".comp", GL_COMPUTE_SHADER}
    };
    return extMap;
}

const std::unordered_map<GLenum, std::string> &shaderTypeStrings()
{
    static std::unordered_map<GLenum, std::string> typeMap{
        {GL_VERTEX_SHADER,          "GL_VERTEX_SHADER"},
        {GL_TESS_CONTROL_SHADER,    "GL_TESS_CONTROL_SHADER"},
        {GL_TESS_EVALUATION_SHADER, "GL_TESS_EVALUATION_SHADER"},
        {GL_GEOMETRY_SHADER,        "GL_GEOMETRY_SHADER"},
        {GL_FRAGMENT_SHADER,        "GL_FRAGMENT_SHADER"},
        {GL_COMPUTE_SHADER,         "GL_COMPUTE_SHADER"}
    };
    return typeMap;
}

const std::vector<VAOElement> &posNormTexVaoElements()
{
    static std::vector<VAOElement> elements{
        {"inPosition",  3, GL_FLOAT, reinterpret_cast<void *>(offsetof(PosNormTexVertex, position))},
        {"inNormal",    3, GL_FLOAT, reinterpret_cast<void *>(offsetof(PosNormTexVertex, normal))},
        {"inTexCoords", 2, GL_FLOAT, reinterpret_cast<void *>(offsetof(PosNormTexVertex, texCoords))},
    };
    return elements;
}

const std::vector<VAOElement> &posVaoElements()
{
    static std::vector<VAOElement> elements{
        {"inPosition", 3, GL_FLOAT, reinterpret_cast<void *>(offsetof(PosVertex, position))}
    };
    return elements;
}

} // namespace


////////////////////////////////////////////////////////////////////////////////
/// \brief OpenGLHelper::setDefaults
///
/// \author Logan Barnes
////////////////////////////////////////////////////////////////////////////////
void
OpenGLHelper::setDefaults()
{
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(std::numeric_limits<unsigned>::max());

    glEnable(GL_PROGRAM_POINT_SIZE);

    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1, -1);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glFrontFace(GL_CCW);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
} // setDefaults

std::shared_ptr<GLuint> OpenGLHelper::createTextureArray(GLsizei width,
                                                         GLsizei height,
                                                         float *pArray,
                                                         GLint filterType,
                                                         GLint wrapType,
                                                         GLint internalFormat,
                                                         GLenum format)
{
    GLuint tex;
    glGenTextures(1, &tex);
    std::shared_ptr<GLuint> spTexture(new GLuint(tex), [](auto pID)
    {
        glDeleteTextures(1, pID);
        delete pID;
    });

    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapType);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapType);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterType);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterType);

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_FLOAT, pArray);

    return spTexture;
} // addTextureArray


void OpenGLHelper::resetTextureArray(std::shared_ptr<GLuint> &spTexture,
                                     GLsizei width,
                                     GLsizei height,
                                     float *pArray,
                                     GLint internalFormat,
                                     GLenum format)
{
    glBindTexture(GL_TEXTURE_2D, *spTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_FLOAT, pArray);
} // resetTextureArray


std::shared_ptr<GLuint> OpenGLHelper::createVao(const std::shared_ptr<GLuint> &spProgram,
                                                const std::shared_ptr<GLuint> &spVbo,
                                                const GLsizei totalStride,
                                                const std::vector<VAOElement> &elements)
{
    GLuint vao;
    glGenVertexArrays(1, &vao);
    std::shared_ptr<GLuint> spVao(new GLuint(vao), [](auto pID)
    {
        glDeleteVertexArrays(1, pID);
        delete pID;
    });

    glBindVertexArray(vao);

    //
    // bind buffer and save program id for loop
    //
    glBindBuffer(GL_ARRAY_BUFFER, vao);

    //
    // iteratoe through all elements
    //
    for (const auto &vaoElmt : elements)
    {
        int pos = glGetAttribLocation(*spProgram, vaoElmt.name.c_str());

        if (pos < 0)
        {
            std::stringstream msg;
            msg << "attrib location "
                << vaoElmt.name
                << " not found for program "
                << *spProgram;

            throw std::runtime_error(msg.str());
        }

        GLuint position = static_cast< GLuint >( pos );

        glEnableVertexAttribArray(position);
        glVertexAttribPointer(
            position,
            vaoElmt.size,     // Num coordinates per position
            vaoElmt.type,     // Type
            GL_FALSE,         // Normalized
            totalStride,      // Stride, 0 = tightly packed
            vaoElmt.pointer   // Array buffer offset
        );
    }

    // Unbind buffers.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return spVao;
} // createVao


std::shared_ptr<GLuint> OpenGLHelper::createFramebuffer(GLsizei width,
                                                        GLsizei height,
                                                        const std::shared_ptr<GLuint> &spColorTex,
                                                        const std::shared_ptr<GLuint> &spDepthTex)
{
    std::shared_ptr<GLuint> spRbo{nullptr};
    std::shared_ptr<GLuint> spFbo{nullptr};

    //
    // no depth texture; create a renderbuffer
    //
    if (!spDepthTex)
    {
        GLuint rbo;
        glGenRenderbuffers(1, &rbo);
        spRbo = std::shared_ptr<GLuint>(new GLuint(rbo), [](auto pID)
        {
            glDeleteRenderbuffers(1, pID);
            delete pID;
        });
    }

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    spFbo = std::shared_ptr<GLuint>(new GLuint(fbo), [spRbo](auto pID)
    {
        glDeleteFramebuffers(1, pID);
        delete pID;
    });


    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    //
    // set color attachment if there is one
    //
    if (spColorTex)
    {
        glBindTexture(GL_TEXTURE_2D, *spColorTex);

        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D,
                               *spColorTex,
                               0);
    }
    else // no color attachment
    {
        glDrawBuffer(GL_NONE); // No color buffer is drawn to
        glReadBuffer(GL_NONE); // No color buffer is read to
    }

    if (spDepthTex)
    {
        glBindTexture(GL_TEXTURE_2D, *spDepthTex);

        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_2D,
                               *spDepthTex,
                               0);
    }
    else
    {
        glBindRenderbuffer(GL_RENDERBUFFER, *spRbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        // attach a renderbuffer to depth attachment point
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, *spRbo);
    }


    // Check the framebuffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        throw std::runtime_error("Framebuffer creation failed");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return spFbo;
} // createFramebuffer


StandardPipeline OpenGLHelper::createPosNormTexPipeline(const PosNormTexVertex *pData,
                                                        const size_t numElements,
                                                        std::vector<std::string> shaderFiles)
{
    if (shaderFiles.empty())
    {
        shaderFiles = {sim::SHADER_PATH + "shader.vert", sim::SHADER_PATH + "shader.frag"};
    }
    StandardPipeline sp = OpenGLHelper::createStandardPipeline(shaderFiles,
                                                               pData,
                                                               numElements,
                                                               sizeof(PosNormTexVertex),
                                                               posNormTexVaoElements());
    sp.vboSize = static_cast<int>(numElements);
    return sp;
}


StandardPipeline OpenGLHelper::createPosPipeline(const PosVertex *pData,
                                                 const size_t numElements,
                                                 std::vector<std::string> shaderFiles)
{
    if (shaderFiles.empty())
    {
        shaderFiles = {sim::SHADER_PATH + "shader.vert", sim::SHADER_PATH + "shader.frag"};
    }

    StandardPipeline sp = OpenGLHelper::createStandardPipeline(shaderFiles,
                                                               pData,
                                                               numElements,
                                                               sizeof(PosVertex),
                                                               posVaoElements());
    sp.vboSize = static_cast<int>(numElements);
    return sp;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief OpenGLHelper::bindFramebuffer
/// \param spFbo
///
/// \author Logan Barnes
////////////////////////////////////////////////////////////////////////////////
void
OpenGLHelper::bindFramebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


////////////////////////////////////////////////////////////////////////////////
/// \brief OpenGLHelper::bindFramebuffer
/// \param spFbo
///
/// \author Logan Barnes
////////////////////////////////////////////////////////////////////////////////
void
OpenGLHelper::bindFramebuffer(const std::shared_ptr<GLuint> &spFbo)
{
    glBindFramebuffer(GL_FRAMEBUFFER, *spFbo);
}


////////////////////////////////////////////////////////////////////////////////
/// \brief OpenGLHelper::clearFramebuffer
///
/// \author Logan Barnes
////////////////////////////////////////////////////////////////////////////////
void
OpenGLHelper::clearFramebuffer()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
} // clearWindow



////////////////////////////////////////////////////////////////////////////////
/// \brief OpenGLHelper::setTextureUniform
///
/// \author Logan Barnes
////////////////////////////////////////////////////////////////////////////////
void
OpenGLHelper::setTextureUniform(
    const std::shared_ptr<GLuint> &spProgram,
    const std::string uniform,
    const std::shared_ptr<GLuint> &spTexture,
    int activeTex
)
{
    glActiveTexture(static_cast< GLenum >( GL_TEXTURE0 + activeTex ));
    glUniform1i(glGetUniformLocation(*spProgram, uniform.c_str()), activeTex);
    glBindTexture(GL_TEXTURE_2D, *spTexture);
}


////////////////////////////////////////////////////////////////////////////////
/// \brief OpenGLHelper::setIntUniform
///
/// \author Logan Barnes
////////////////////////////////////////////////////////////////////////////////
void
OpenGLHelper::setIntUniform(
    const std::shared_ptr<GLuint> &spProgram,
    const std::string uniform,
    const int *pValue,
    const int size,
    const int count
)
{
    switch (size)
    {

        case 1:
            glUniform1i(glGetUniformLocation(*spProgram, uniform.c_str()), *pValue);
            break;

        case 2:
            glUniform2iv(glGetUniformLocation(*spProgram, uniform.c_str()), count, pValue);
            break;

        case 3:
            glUniform3iv(glGetUniformLocation(*spProgram, uniform.c_str()), count, pValue);
            break;

        case 4:
            glUniform4iv(glGetUniformLocation(*spProgram, uniform.c_str()), count, pValue);
            break;

        default:
            std::stringstream msg;
            msg << "Int or vector of size " << size << " does not exist";
            throw std::runtime_error(msg.str());
            break;

    } // switch

} // setIntUniform


void
OpenGLHelper::setFloatUniform(
    const std::shared_ptr<GLuint> &spProgram,
    const std::string uniform,
    const float *pValue,
    const int size,
    const int count
)
{
    switch (size)
    {

        case 1:
            glUniform1f(glGetUniformLocation(*spProgram, uniform.c_str()), *pValue);
            break;

        case 2:
            glUniform2fv(glGetUniformLocation(*spProgram, uniform.c_str()), count, pValue);
            break;

        case 3:
            glUniform3fv(glGetUniformLocation(*spProgram, uniform.c_str()), count, pValue);
            break;

        case 4:
            glUniform4fv(glGetUniformLocation(*spProgram, uniform.c_str()), count, pValue);
            break;

        default:
            std::stringstream msg;
            msg << "Float or vector of size " << size << " does not exist";
            throw std::runtime_error(msg.str());
            break;

    } // switch

} // setFloatUniform


void
OpenGLHelper::setMatrixUniform(
    const std::shared_ptr<GLuint> &spProgram,
    const std::string uniform,
    const float *pValue,
    const int size,
    const int count
)
{
    switch (size)
    {
        case 2:
            glUniformMatrix2fv(
                glGetUniformLocation(*spProgram, uniform.c_str()),
                count,
                GL_FALSE,
                pValue
            );
            break;

        case 3:
            glUniformMatrix3fv(
                glGetUniformLocation(*spProgram, uniform.c_str()),
                count,
                GL_FALSE,
                pValue
            );
            break;


        case 4:
            glUniformMatrix4fv(
                glGetUniformLocation(*spProgram, uniform.c_str()),
                count,
                GL_FALSE,
                pValue
            );
            break;

        default:
            std::stringstream msg;
            msg << "Matrix of size " << size << " does not exist";
            throw std::runtime_error(msg.str());
            break;
    } // switch

} // OpenGLHelper::setMatrixUniform


void
OpenGLHelper::renderBuffer(
    const std::shared_ptr<GLuint> &spVao,
    const int start,
    const int verts,
    const GLenum mode,
    const std::shared_ptr<GLuint> &spIbo,
    const void *pOffset,
    const GLenum iboType
)
{
    glBindVertexArray(*spVao);

    if (spIbo)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *spIbo);
        glDrawElements(mode, verts, iboType, pOffset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    else
    {
        glDrawArrays(mode, start, verts);
    }

    glBindVertexArray(0);
} // OpenGLHelper::renderBuffer


std::string
OpenGLHelper::_readFile(const std::string filePath)
{
    std::ifstream file(filePath, std::ios::in);

    if (!file.is_open() || !file.good())
    {
        throw std::runtime_error("Could not read file: " + filePath);
    }

    // get full file size
    file.seekg(0, std::ios::end);
    size_t size = static_cast< size_t >( file.tellg());

    // allocate string of correct size
    std::string buffer(size, ' ');

    // fill string with file contents
    // note: string memory is only guaranteed contiguous in C++11 and up
    file.seekg(0);
    file.read(&buffer[0], static_cast< std::streamsize >( size ));

    file.close();

    return buffer;
} // OpenGLHelper::_readFile



std::shared_ptr<GLuint>
OpenGLHelper::_createShader(
    GLenum shaderType,
    const std::string filePath
)
{
    std::shared_ptr<GLuint> upShader(new GLuint,
                                     [](auto pID)
                                     {
                                         glDeleteShader(*pID);
                                         delete pID;
                                     });

    GLuint shader = glCreateShader(shaderType);
    *upShader = shader;

    // Load shader
    std::string shaderStr = _readFile(filePath);
    const char *shaderSource = shaderStr.c_str();

    // Compile shader
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

    // Check shader
    GLint result = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE)
    {
        int logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> shaderError(static_cast< size_t >( logLength ));
        glGetShaderInfoLog(shader, logLength, nullptr, shaderError.data());

        // shader will get deleted when shared_ptr goes out of scope

        throw std::runtime_error("(" + shaderTypeStrings().at(shaderType) + ") " + std::string(shaderError.data()));
    }

    return upShader;
} // OpenGLHelper::_createShader



std::shared_ptr<GLuint>
OpenGLHelper::_createShader(const std::string filePath)
{
    size_t dot = filePath.find_last_of(".");

    std::string ext = filePath.substr(dot);

    if (shaderTypes().find(ext) == shaderTypes().end())
    {
        throw std::runtime_error("Unknown shader extension: " + ext);
    }

    return OpenGLHelper::_createShader(shaderTypes().at(ext), filePath);
} // OpenGLHelper::_createShader



std::shared_ptr<GLuint>
OpenGLHelper::_createProgram(const IdVec shaderIds)
{
    std::shared_ptr<GLuint> upProgram(new GLuint(glCreateProgram()),
                                      [shaderIds](auto pID)
                                      {
                                          for (auto &upShader : shaderIds)
                                          {
                                              glDeleteShader(*upShader);
                                          }

                                          glDeleteProgram(*pID);
                                          delete pID;
                                      });

    GLuint program = *upProgram;

    for (auto &upShader : shaderIds)
    {
        glAttachShader(program, *upShader);
    }

    glLinkProgram(program);

    // Check program
    GLint result = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &result);

    if (result == GL_FALSE)
    {
        int logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> programError(static_cast< size_t >( logLength ));
        glGetProgramInfoLog(program, logLength, NULL, programError.data());

        // shaders and programs get deleted with shared_ptr goes out of scope

        throw std::runtime_error("(Program) " + std::string(programError.data()));
    }

    for (auto &upShader : shaderIds)
    {
        glDetachShader(program, *upShader);
    }

    return upProgram;
} // OpenGLHelper::_createProgram



} // namespace sim
