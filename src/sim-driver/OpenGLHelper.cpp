#include <sim-driver/OpenGLHelper.hpp>

#include "ShaderConfig.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <limits>

namespace sim
{

namespace
{

const std::unordered_map<std::string, GLenum> &
shaderTypes()
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

const std::unordered_map<GLenum, std::string> &
shaderTypeStrings()
{
    static std::unordered_map<GLenum, std::string> typeMap{
        {GL_VERTEX_SHADER, "GL_VERTEX_SHADER"},
        {GL_TESS_CONTROL_SHADER, "GL_TESS_CONTROL_SHADER"},
        {GL_TESS_EVALUATION_SHADER, "GL_TESS_EVALUATION_SHADER"},
        {GL_GEOMETRY_SHADER, "GL_GEOMETRY_SHADER"},
        {GL_FRAGMENT_SHADER, "GL_FRAGMENT_SHADER"},
        {GL_COMPUTE_SHADER, "GL_COMPUTE_SHADER"}
    };
    return typeMap;
}

std::string
read_file(const std::string filePath)
{
    std::ifstream file(filePath, std::ios::in);

    if (!file.is_open() || !file.good()) {
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
} // read_file


////////////////////////////////////////////////////////////////////////////////


std::shared_ptr<GLuint>
create_shader(GLenum shaderType, const std::string filePath)
{
    std::shared_ptr<GLuint> spShader(new GLuint, [](auto pID)
    {
        glDeleteShader(*pID);
        delete pID;
    });

    GLuint shader = glCreateShader(shaderType);
    *spShader = shader;

    // Load shader
    std::string shaderStr = read_file(filePath);
    const char *shaderSource = shaderStr.c_str();

    // Compile shader
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

    // Check shader
    GLint result = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE) {
        int logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> shaderError(static_cast< size_t >( logLength ));
        glGetShaderInfoLog(shader, logLength, nullptr, shaderError.data());

        // shader will get deleted when shared_ptr goes out of scope
        throw std::runtime_error("(" + shaderTypeStrings().at(shaderType) + ") " + std::string(shaderError.data()));
    }
    return spShader;
} // create_shader


std::shared_ptr<GLuint>
create_shader(const std::string filePath)
{
    size_t dot = filePath.find_last_of(".");

    std::string ext = filePath.substr(dot);

    if (shaderTypes().find(ext) == shaderTypes().end()) {
        throw std::runtime_error("Unknown shader extension: " + ext);
    }

    return create_shader(shaderTypes().at(ext), filePath);
} // create_shader


template<typename ... Shaders>
void
create_shader(IdVec *pIds, const std::string filePath)
{
    // create/compile shader and add it to list of shaders
    pIds->emplace_back(create_shader(filePath));
}

template<typename ... Shaders>
void
create_shader(IdVec *pIds, const std::string filePath, const Shaders ... shaders)
{
    // create/compile shader and add it to list of shaders
    pIds->emplace_back(create_shader(filePath));

    // repeat for the rest of the shaders
    create_shader(pIds, shaders ...);
}


////////////////////////////////////////////////////////////////////////////////


std::shared_ptr<GLuint>
create_program(const IdVec shaderIds)
{
    std::shared_ptr<GLuint> spProgram(new GLuint(glCreateProgram()), [shaderIds](auto pID)
    {
        for (auto &spShader : shaderIds) {
            glDeleteShader(*spShader);
        }

        glDeleteProgram(*pID);
        delete pID;
    });

    GLuint program = *spProgram;

    for (auto &spShader : shaderIds) {
        glAttachShader(program, *spShader);
    }

    glLinkProgram(program);

    // Check program
    GLint result = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &result);

    if (result == GL_FALSE) {
        int logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> programError(static_cast< size_t >( logLength ));
        glGetProgramInfoLog(program, logLength, NULL, programError.data());

        // shaders and programs get deleted with shared_ptr goes out of scope

        throw std::runtime_error("(Program) " + std::string(programError.data()));
    }

    for (auto &spShader : shaderIds) {
        glDetachShader(program, *spShader);
    }

    return spProgram;
} // create_program

////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<GLuint>
create_separable_program(GLenum shaderType, const std::string filePath)
{
    // Load shader
    std::string shaderStr = read_file(filePath);
    const char *shaderSource = shaderStr.c_str();

    std::shared_ptr<GLuint> spProgram(new GLuint(glCreateShaderProgramv(shaderType, 1, &shaderSource)), [](auto pId)
    {
        glDeleteProgram(*pId);
        delete pId;
    });

    GLuint program = *spProgram;

    // Check program
    GLint result = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &result);

    if (result == GL_FALSE) {
        int logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> programError(static_cast< size_t >( logLength ));
        glGetProgramInfoLog(program, logLength, NULL, programError.data());

        // program deleted wh3en shared_ptr goes out of scope
        throw std::runtime_error("(ShaderProgram) " + std::string(programError.data()));
    }

    return spProgram;
} // create_separable_program


void
create_separable_program(const std::string filePath, SeparablePrograms *pSp)
{
    size_t dot = filePath.find_last_of(".");

    std::string ext = filePath.substr(dot);

    if (shaderTypes().find(ext) == shaderTypes().end()) {
        throw std::runtime_error("Unknown shader extension: " + ext);
    }

    GLenum type = shaderTypes().at(ext);
    std::shared_ptr<GLuint> program = create_separable_program(type, filePath);

    switch (type) {
        case GL_VERTEX_SHADER:pSp->vert = std::move(program);
            break;
        case GL_TESS_CONTROL_SHADER:pSp->tesc = std::move(program);
            break;
        case GL_TESS_EVALUATION_SHADER:pSp->tese = std::move(program);
            break;
        case GL_GEOMETRY_SHADER:pSp->geom = std::move(program);
            break;
        case GL_FRAGMENT_SHADER:pSp->frag = std::move(program);
            break;
        case GL_COMPUTE_SHADER:pSp->comp = std::move(program);
            break;
        default:break;
    }
} // create_separable_program

template<typename ... Shaders>
void
create_separable_program(SeparablePrograms *pSp, const std::string filePath)
{
    create_separable_program(filePath, pSp);
}

template<typename ... Shaders>
void
create_separable_program(SeparablePrograms *pSp, const std::string filePath, const Shaders ... shaders)
{
    create_separable_program(filePath, pSp);
    create_separable_program(pSp, shaders...);
}

} // namespace


const std::vector<VAOElement> &
posNormTexVaoElements()
{
    static std::vector<VAOElement> elements{
        {"local_position", 3, GL_FLOAT, reinterpret_cast<void *>(offsetof(PosNormTexVertex, position))},
        {"local_normal", 3, GL_FLOAT, reinterpret_cast<void *>(offsetof(PosNormTexVertex, normal))},
        {"tex_coords", 2, GL_FLOAT, reinterpret_cast<void *>(offsetof(PosNormTexVertex, texCoords))},
    };
    return elements;
}

const std::vector<VAOElement> &
posVaoElements()
{
    static std::vector<VAOElement> elements{
        {"local_position", 3, GL_FLOAT, reinterpret_cast<void *>(offsetof(PosVertex, position))}
    };
    return elements;
}

const unsigned &
primitiveRestart()
{
    static unsigned index = (std::numeric_limits<unsigned>::max)();
    return index;
}

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
    glPrimitiveRestartIndex(primitiveRestart());

    glEnable(GL_PROGRAM_POINT_SIZE);

    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1, -1);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glFrontFace(GL_CCW);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
} // setDefaults


template<typename ... Shaders>
std::shared_ptr<GLuint>
OpenGLHelper::createProgram(std::string firstShader, Shaders ... shaders)
{
    IdVec shaderIds;

    // create and compile all the shaders
    create_shader(&shaderIds, firstShader, shaders ...);

    // link shaders and create OpenGL program
    return create_program(shaderIds);
}

std::shared_ptr<GLuint>
OpenGLHelper::createProgram(std::vector<std::string> shaderFiles)
{
    IdVec shaders;
    for (auto &shaderFile : shaderFiles) {
        shaders.emplace_back(create_shader(shaderFile));
    }

    return create_program(shaders);
}

template<typename ... Shaders>
sim::SeparablePrograms
OpenGLHelper::createSeparablePrograms(std::string firstShader, Shaders ... shaders)
{
    SeparablePrograms sp;

    // create and compile all the shaders
    create_separable_program(&sp, firstShader, shaders ...);

    GLuint pipeline;
    glGenProgramPipelines(1, &pipeline);

    sp.pipeline = std::shared_ptr<GLuint>(new GLuint(pipeline), [](GLuint *pId)
    {
        glDeleteProgramPipelines(1, pId);
        delete pId;
    });

    return sp;
}

std::shared_ptr<GLuint>
OpenGLHelper::createTextureArray(GLsizei width,
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


void
OpenGLHelper::resetTextureArray(std::shared_ptr<GLuint> &spTexture,
                                GLsizei width,
                                GLsizei height,
                                float *pArray,
                                GLint internalFormat,
                                GLenum format)
{
    glBindTexture(GL_TEXTURE_2D, *spTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_FLOAT, pArray);
} // resetTextureArray


std::shared_ptr<GLuint>
OpenGLHelper::createVao(const std::shared_ptr<GLuint> &spProgram,
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
    glBindBuffer(GL_ARRAY_BUFFER, *spVbo);

    //
    // iterate through all elements
    //
    for (const auto &vaoElmt : elements) {
        int pos = glGetAttribLocation(*spProgram, vaoElmt.name.c_str());
        if (pos < 0) {
            std::stringstream msg;
            msg << "attrib location "
                << vaoElmt.name
                << " not found for program "
                << *spProgram;

            throw std::runtime_error(msg.str());
        }

        GLuint position = static_cast< GLuint >( pos );

        glEnableVertexAttribArray(position);
        switch (vaoElmt.type) {
            case GL_BYTE:
            case GL_UNSIGNED_BYTE:
            case GL_SHORT:
            case GL_UNSIGNED_SHORT:
            case GL_INT:
            case GL_UNSIGNED_INT:
                glVertexAttribIPointer(
                    position,
                    vaoElmt.size,     // Num coordinates per position
                    vaoElmt.type,     // Type
                    totalStride,      // Stride, 0 = tightly packed
                    vaoElmt.pointer   // Array buffer offset
                );
                break;
            case GL_DOUBLE:
                glVertexAttribLPointer(
                    position,
                    vaoElmt.size,     // Num coordinates per position
                    vaoElmt.type,     // Type
                    totalStride,      // Stride, 0 = tightly packed
                    vaoElmt.pointer   // Array buffer offset
                );
                break;
            default:
                glVertexAttribPointer(
                    position,
                    vaoElmt.size,     // Num coordinates per position
                    vaoElmt.type,     // Type
                    GL_FALSE,         // Normalized
                    totalStride,      // Stride, 0 = tightly packed
                    vaoElmt.pointer   // Array buffer offset
                );
                break;
        }
    }

    // Unbind buffers.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return spVao;
} // createVao


std::shared_ptr<GLuint>
OpenGLHelper::createFramebuffer(GLsizei width,
                                GLsizei height,
                                const std::shared_ptr<GLuint> &spColorTex,
                                const std::shared_ptr<GLuint> &spDepthTex)
{
    std::shared_ptr<GLuint> spRbo{nullptr};
    std::shared_ptr<GLuint> spFbo{nullptr};

    //
    // no depth texture; create a renderbuffer
    //
    if (!spDepthTex) {
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
    if (spColorTex) {
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

    if (spDepthTex) {
        glBindTexture(GL_TEXTURE_2D, *spDepthTex);

        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_2D,
                               *spDepthTex,
                               0);
    }
    else {
        glBindRenderbuffer(GL_RENDERBUFFER, *spRbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        // attach a renderbuffer to depth attachment point
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, *spRbo);
    }


    // Check the framebuffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Framebuffer creation failed");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return spFbo;
} // createFramebuffer


StandardPipeline
OpenGLHelper::createPosNormTexPipeline(const PosNormTexVertex *pData,
                                       const size_t numElements,
                                       std::vector<std::string> shaderFiles)
{
    if (shaderFiles.empty()) {
        shaderFiles = {sim::SHADER_PATH + "shader.vert",
#ifdef __APPLE__
                       sim::SHADER_PATH + "shader_mac.frag"
#else
            sim::SHADER_PATH + "shader.frag"
#endif
        };
    }
    sim::StandardPipeline sp = sim::OpenGLHelper::createStandardPipeline(shaderFiles,
                                                                         pData,
                                                                         numElements,
                                                                         sizeof(PosNormTexVertex),
                                                                         posNormTexVaoElements());
    return sp;
}

StandardPipeline
OpenGLHelper::createPosPipeline(const PosVertex *pData,
                                const size_t numElements,
                                std::vector<std::string> shaderFiles)
{
    if (shaderFiles.empty()) {
        shaderFiles = {sim::SHADER_PATH + "shader.vert",
#ifdef __APPLE__
                       sim::SHADER_PATH + "shader_mac.frag"
#else
            sim::SHADER_PATH + "shader.frag"
#endif
        };
    }

    StandardPipeline sp = OpenGLHelper::createStandardPipeline(shaderFiles,
                                                               pData,
                                                               numElements,
                                                               sizeof(PosVertex),
                                                               posVaoElements());
    sp.vboSize = static_cast<int>(numElements);
    return sp;
}

StandardPipeline
OpenGLHelper::createScreenspacePipeline()
{
    std::vector<sim::PosNormTexVertex> data{
        {{-1, -1, 0}, {0, 0, 1}, {0, 1}},
        {{1, -1, 0}, {0, 0, 1}, {1, 1}},
        {{-1, 1, 0}, {0, 0, 1}, {0, 0}},
        {{1, 1, 0}, {0, 0, 1}, {1, 0}}
    };

    return createPosNormTexPipeline(data.data(), data.size());
}

void
OpenGLHelper::bindBufferToTexture(const std::shared_ptr<GLuint> &spTexture,
                                  const std::shared_ptr<GLuint> &spBuffer,
                                  int alignment,
                                  int width,
                                  int height)
{
    glBindTexture(GL_TEXTURE_2D, *spTexture);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, *spBuffer);

    glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA32F,
                 width,
                 height,
                 0,
                 GL_RGBA,
                 GL_FLOAT,
                 0);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

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
OpenGLHelper::setTextureUniform(const std::shared_ptr<GLuint> &spProgram,
                                const std::string &uniform,
                                const std::shared_ptr<GLuint> &spTexture,
                                int activeTex)
{
    glActiveTexture(static_cast< GLenum >( GL_TEXTURE0 + activeTex ));
    glProgramUniform1i(*spProgram, glGetUniformLocation(*spProgram, uniform.c_str()), activeTex);
    glBindTexture(GL_TEXTURE_2D, *spTexture);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief OpenGLHelper::setIntUniform
///
/// \author Logan Barnes
////////////////////////////////////////////////////////////////////////////////
void
OpenGLHelper::setIntUniform(const std::shared_ptr<GLuint> &spProgram,
                            const std::string &uniform,
                            const int *pValue,
                            const int size,
                            const int count)
{
    switch (size) {

        case 1:glProgramUniform1iv(*spProgram, glGetUniformLocation(*spProgram, uniform.c_str()), count, pValue);
            break;

        case 2:glProgramUniform2iv(*spProgram, glGetUniformLocation(*spProgram, uniform.c_str()), count, pValue);
            break;

        case 3:glProgramUniform3iv(*spProgram, glGetUniformLocation(*spProgram, uniform.c_str()), count, pValue);
            break;

        case 4:glProgramUniform4iv(*spProgram, glGetUniformLocation(*spProgram, uniform.c_str()), count, pValue);
            break;

        default:std::stringstream msg;
            msg << "Int or vector of size " << size << " does not exist";
            throw std::runtime_error(msg.str());
            break;

    } // switch

} // setIntUniform


void
OpenGLHelper::setFloatUniform(
    const std::shared_ptr<GLuint> &spProgram,
    const std::string &uniform,
    const float *pValue,
    const int size,
    const int count
)
{
    switch (size) {

        case 1:glProgramUniform1f(*spProgram, glGetUniformLocation(*spProgram, uniform.c_str()), *pValue);
            break;

        case 2:glProgramUniform2fv(*spProgram, glGetUniformLocation(*spProgram, uniform.c_str()), count, pValue);
            break;

        case 3:glProgramUniform3fv(*spProgram, glGetUniformLocation(*spProgram, uniform.c_str()), count, pValue);
            break;

        case 4:glProgramUniform4fv(*spProgram, glGetUniformLocation(*spProgram, uniform.c_str()), count, pValue);
            break;

        default:std::stringstream msg;
            msg << "Float or vector of size " << size << " does not exist";
            throw std::runtime_error(msg.str());
            break;

    } // switch

} // setFloatUniform


void
OpenGLHelper::setMatrixUniform(const std::shared_ptr<GLuint> &spProgram,
                               const std::string &uniform,
                               const float *pValue,
                               const int size,
                               const int count)
{
    switch (size) {
        case 2:
            glProgramUniformMatrix2fv(*spProgram,
                                      glGetUniformLocation(*spProgram, uniform.c_str()),
                                      count,
                                      GL_FALSE,
                                      pValue);
            break;

        case 3:
            glProgramUniformMatrix3fv(*spProgram,
                                      glGetUniformLocation(*spProgram, uniform.c_str()),
                                      count,
                                      GL_FALSE,
                                      pValue);
            break;

        case 4:
            glProgramUniformMatrix4fv(*spProgram,
                                      glGetUniformLocation(*spProgram, uniform.c_str()),
                                      count,
                                      GL_FALSE,
                                      pValue);
            break;

        default:std::stringstream msg;
            msg << "Matrix of size " << size << " does not exist";
            throw std::runtime_error(msg.str());
            break;
    } // switch

} // OpenGLHelper::setMatrixUniform


void
OpenGLHelper::setSsboUniform(const std::shared_ptr<GLuint> &spProgram,
                             const std::shared_ptr<GLuint> &spSsbo,
                             const std::string &uniform,
                             const int sizeBytes,
                             const GLuint binding)
{
    GLuint blockIdx = glGetProgramResourceIndex(*spProgram, GL_SHADER_STORAGE_BLOCK, uniform.c_str());
    glShaderStorageBlockBinding(*spProgram, blockIdx, binding);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, *spSsbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, *spSsbo);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, binding, *spSsbo, 0, sizeBytes);
}

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

    if (spIbo) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *spIbo);
        glDrawElements(mode, verts, iboType, pOffset);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    else {
        glDrawArrays(mode, start, verts);
    }

    glBindVertexArray(0);
} // OpenGLHelper::renderBuffer

template std::shared_ptr<GLuint>
OpenGLHelper::createProgram(std::string);

template std::shared_ptr<GLuint>
OpenGLHelper::createProgram(std::string, std::string);

template std::shared_ptr<GLuint>
OpenGLHelper::createProgram(std::string, std::string, std::string);

template std::shared_ptr<GLuint>
OpenGLHelper::createProgram(std::string, std::string, std::string, std::string);

template std::shared_ptr<GLuint>
OpenGLHelper::createProgram(std::string, std::string, std::string, std::string, std::string);

template std::shared_ptr<GLuint>
OpenGLHelper::createProgram(std::string, std::string, std::string, std::string, std::string, std::string);

template SeparablePrograms
OpenGLHelper::createSeparablePrograms(std::string);

template SeparablePrograms
OpenGLHelper::createSeparablePrograms(std::string, std::string);

template SeparablePrograms
OpenGLHelper::createSeparablePrograms(std::string, std::string, std::string);

template SeparablePrograms
OpenGLHelper::createSeparablePrograms(std::string, std::string, std::string, std::string);

template SeparablePrograms
OpenGLHelper::createSeparablePrograms(std::string, std::string, std::string, std::string, std::string);

template SeparablePrograms
OpenGLHelper::createSeparablePrograms(std::string, std::string, std::string, std::string, std::string, std::string);

} // namespace sim
