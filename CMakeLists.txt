cmake_minimum_required(VERSION 3.7)
project(SimDriver)

set(CMAKE_CXX_STANDARD 14)

include(cmake/DownloadProject.cmake)

configure_file(src/ShaderConfig.hpp.in ${CMAKE_BINARY_DIR}/ShaderConfig.hpp)

# Download and unpack glfw at configure time
download_project(PROJ glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG latest
        UPDATE_DISCONNECTED 1
        QUIET
        )

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# compile glfw with current project
add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})


# Download and unpack imgui at configure time
download_project(PROJ imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG master
        UPDATE_DISCONNECTED 1
        QUIET
        )

# Download and unpack glm at configure time
download_project(PROJ glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG master
        UPDATE_DISCONNECTED 1
        QUIET
        )
# Download and unpack variant at configure time
download_project(PROJ variant
        GIT_REPOSITORY https://github.com/mapbox/variant.git
        GIT_TAG v1.1.5
        UPDATE_DISCONNECTED 1
        QUIET
        )

set(SOURCE_FILES
        # imgui
        ${imgui_SOURCE_DIR}/imgui
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_internal.h
        ${imgui_SOURCE_DIR}/imconfig.h
        ${imgui_SOURCE_DIR}/stb_rect_pack.h
        ${imgui_SOURCE_DIR}/stb_textedit.h
        ${imgui_SOURCE_DIR}/stb_truetype.h
        src/imgui_impl_glfw_gl3

        # glad
        src/glad/glad
        src/KHR/khrplatform.h

        #shaders
        src/shaders/shader.vert
        src/shaders/shader.geom
        src/shaders/shader.frag

        # callbacks
        src/sim-driver/callbacks/Callbacks
        # cameras
        src/sim-driver/cameras/Camera
        # meshes
        src/sim-driver/meshes/MeshVariant
        src/sim-driver/meshes/SphereMesh
        # renderers
        src/sim-driver/renderers/MeshRenderer
        src/sim-driver/renderers/RendererHelper
        src/sim-driver/renderers/Renderer
        # sim-driver
        src/sim-driver/OpenGLForwardDeclarations
        src/sim-driver/OpenGLHelper
        src/sim-driver/OpenGLSimulation
        src/sim-driver/SimDriver
        src/sim-driver/meshes/MeshVariant.hpp src/sim-driver/meshes/SphereMesh.cpp src/sim-driver/meshes/SphereMesh.hpp)

add_library(SimDriver ${SOURCE_FILES})

target_include_directories(SimDriver PUBLIC src ${CMAKE_BINARY_DIR})
target_include_directories(SimDriver SYSTEM PUBLIC
        ${glfw_SOURCE_DIR}/include
        ${imgui_SOURCE_DIR}
        ${glm_SOURCE_DIR}
        ${variant_SOURCE_DIR}/include
        )
target_link_libraries(SimDriver ${OPENGL_gl_LIBRARY} glfw)

set(EXEC_SOURCE_FILES src/exec/main)
add_executable(SimDriverExec ${EXEC_SOURCE_FILES})
target_link_libraries(SimDriverExec SimDriver)