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
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui.h
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_internal.h
        ${imgui_SOURCE_DIR}/imconfig.h
        ${imgui_SOURCE_DIR}/stb_rect_pack.h
        ${imgui_SOURCE_DIR}/stb_textedit.h
        ${imgui_SOURCE_DIR}/stb_truetype.h
        src/imgui_impl_glfw_gl3.cpp
        src/imgui_impl_glfw_gl3.h

        # glad
        src/glad/glad.c
        src/glad/glad.h
        src/KHR/khrplatform.h
        src/sim-driver/OpenGLSimulation.hpp

        # sim-driver
        src/sim-driver/SimDriver.cpp
        src/sim-driver/SimDriver.hpp
        src/sim-driver/OpenGLSimulation.hpp
        src/sim-driver/OpenGLForwardDeclarations.hpp
        src/sim-driver/OpenGLHelper.cpp
        src/sim-driver/OpenGLHelper.hpp
        src/sim-driver/callbacks/Callbacks.hpp
        src/sim-driver/cameras/Camera.cpp
        src/sim-driver/cameras/Camera.hpp
        src/shaders/shader.vert
        src/shaders/shader.geom
        src/shaders/shader.frag
        src/sim-driver/renderers/RendererHelper.hpp
        src/sim-driver/renderers/Renderer.hpp
        src/sim-driver/renderers/RendererHelper2.hpp)

add_library(SimDriver ${SOURCE_FILES})

target_include_directories(SimDriver PUBLIC src ${CMAKE_BINARY_DIR})
target_include_directories(SimDriver SYSTEM PUBLIC
        ${glfw_SOURCE_DIR}/include
        ${imgui_SOURCE_DIR}
        ${glm_SOURCE_DIR}
        )
target_link_libraries(SimDriver ${OPENGL_gl_LIBRARY} glfw)

set(EXEC_SOURCE_FILES
        src/exec/main.cpp
        )
add_executable(SimDriverExec ${EXEC_SOURCE_FILES})
target_link_libraries(SimDriverExec SimDriver)

set(EXEC_SOURCE_FILES2
        src/exec/main2.cpp
        )
add_executable(SimDriverExec2 ${EXEC_SOURCE_FILES2})
target_link_libraries(SimDriverExec2 SimDriver)