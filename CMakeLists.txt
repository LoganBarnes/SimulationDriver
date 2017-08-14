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

set(SOURCE_FILES
        # imgui
        ${imgui_SOURCE_DIR}/imgui
        ${imgui_SOURCE_DIR}/imgui_demo
        ${imgui_SOURCE_DIR}/imgui_draw
        ${imgui_SOURCE_DIR}/imgui_internal
        ${imgui_SOURCE_DIR}/imconfig
        ${imgui_SOURCE_DIR}/stb_rect_pack
        ${imgui_SOURCE_DIR}/stb_textedit
        ${imgui_SOURCE_DIR}/stb_truetype
        src/imgui_impl_glfw_gl3
        # glad
        src/glad/glad
        src/KHR/khrplatform
        #shaders
        src/shaders/heights.vert
        src/shaders/heights.geom
        src/shaders/shader.vert
        src/shaders/shader.geom
        src/shaders/shader.frag
        src/shaders/shader_mac.frag
        # callbacks
        src/sim-driver/callbacks/CallbackWrapper
        src/sim-driver/callbacks/CameraCallbacks
        # cameras
        src/sim-driver/cameras/Camera
        # meshes
        src/sim-driver/meshes/HeightMap
        src/sim-driver/meshes/MeshFunctions
        src/sim-driver/meshes/MeshHelper
        # renderers
        src/sim-driver/renderers/HeightMapRenderer
        src/sim-driver/renderers/MeshRenderer
        src/sim-driver/renderers/RendererHelper
        # sim-driver
        src/sim-driver/OpenGLForwardDeclarations
        src/sim-driver/OpenGLHelper
        src/sim-driver/OpenGLSimulation
        src/sim-driver/SimDriver
        )

add_library(SimDriver ${SOURCE_FILES})

target_include_directories(SimDriver PUBLIC src ${CMAKE_BINARY_DIR})
target_include_directories(SimDriver SYSTEM PUBLIC
        ${glfw_SOURCE_DIR}/include
        ${imgui_SOURCE_DIR}
        ${glm_SOURCE_DIR}
        )
target_link_libraries(SimDriver ${OPENGL_gl_LIBRARY} glfw)

add_executable(MeshExec src/exec/MeshMain)
target_link_libraries(MeshExec SimDriver)

add_executable(HeightsExec src/exec/HeightsMain)
target_link_libraries(HeightsExec SimDriver)
