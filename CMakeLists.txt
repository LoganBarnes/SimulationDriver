cmake_minimum_required(VERSION 3.7)
project(SimDriver)

set(CMAKE_CXX_STANDARD 14)

set(COMPILE_FLAGS ${CMAKE_CXX_FLAGS})
if (NOT MSVC)
    list(APPEND COMPILE_FLAGS "-Wall -Wextra -Werror -pedantic")
endif ()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
include(DownloadProject)

configure_file(src/ShaderConfig.hpp.in ${CMAKE_BINARY_DIR}/ShaderConfig.hpp)

# Download and unpack glfw at configure time
download_project(PROJ glfw
        PREFIX thirdparty
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
        PREFIX thirdparty
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.50
        UPDATE_DISCONNECTED 1
        QUIET
        )

# Download and unpack glm at configure time
download_project(PROJ glm
        PREFIX thirdparty
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 0.9.8.5
        UPDATE_DISCONNECTED 1
        QUIET
        )

add_library(thirdparty
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
        )
target_include_directories(thirdparty SYSTEM PUBLIC
        ${glfw_SOURCE_DIR}/include
        ${imgui_SOURCE_DIR}
        ${glm_SOURCE_DIR}
        src
        )
target_link_libraries(thirdparty glfw)

set(SOURCE_FILES
        #shaders
        src/shaders/heights.vert
        src/shaders/heights.geom
        src/shaders/shader.vert
        src/shaders/shader.geom
        src/shaders/shader.frag
        src/shaders/shader_mac.frag
        # meshes
        src/sim-driver/meshes/HeightMap
        src/sim-driver/meshes/MeshFunctions
        src/sim-driver/meshes/MeshHelper
        # renderers
        src/sim-driver/renderers/HeightMapRenderer
        src/sim-driver/renderers/MeshRenderer
        src/sim-driver/renderers/RendererHelper
        # sim-driver
        src/sim-driver/CallbackWrapper
        src/sim-driver/Camera
        src/sim-driver/OpenGLForwardDeclarations
        src/sim-driver/OpenGLHelper
        src/sim-driver/OpenGLSimulation
        src/sim-driver/SimCallbacks
        src/sim-driver/SimData
        src/sim-driver/SimDriver
        )

add_library(SimDriver ${SOURCE_FILES})

target_include_directories(SimDriver PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(SimDriver thirdparty)
set_target_properties(SimDriver PROPERTIES CMAKE_CXX_FLAGS ${COMPILE_FLAGS})

find_package(CUDA)
if (CUDA_FOUND)
    find_package(OptiX)
    if (optix_LIBRARY AND OptiX_INCLUDE AND optix_prime_LIBRARY)
        add_definitions(-Dsutil_sdk_EXPORTS -D_CRT_SECURE_NO_DEPRECATE) # windows BS

        # Auto detect compute architecture
       if (NOT ${CMAKE_VERSION} VERSION_LESS 3.7 AND NOT CUDA_ARCH_FLAGS)
            cuda_select_nvcc_arch_flags(CUDA_ARCH_FLAGS Auto)
            set(CUDA_ARCH_FLAGS ${CUDA_ARCH_FLAGS} CACHE STRING "gencode architecture flags for nvcc")
       endif ()

        set(CUDA_SOURCE
                src/extra/sim-driver/extra/OptiXCommon
                src/extra/sim-driver/extra/OptiXScene
                src/extra/sim-driver/extra/OptiXSimulation
                )

        set(OPTIX_PTX_SOURCE
                src/extra/sim-driver/extra/optix-programs/Cameras.cu
                src/extra/sim-driver/extra/optix-programs/Box.cu
                src/extra/sim-driver/extra/optix-programs/Brdf.cu
                )

        include_directories(src/extra)
        include_directories(SYSTEM ${OptiX_INCLUDE})

        cuda_add_library(SimDriverOptiX ${CUDA_SOURCE} OPTIONS ${CUDA_ARCH_FLAGS})

        set(CUDA_GENERATED_OUTPUT_DIR ${PROJECT_BINARY_DIR}/ptx)
        cuda_wrap_srcs(SimDriverOptiX PTX GENERATED_PTX
                ${OPTIX_PTX_SOURCE}
                OPTIONS -use_fast_math ${CUDA_ARCH_FLAGS}
                )

        target_link_libraries(SimDriverOptiX
                SimDriver
                ${optix_LIBRARY}
                ${optixu_LIBRARY}
                )

        target_sources(SimDriver PUBLIC ${GENERATED_PTX})
        target_link_libraries(SimDriver SimDriverOptiX)

        cuda_add_executable(OptiXExec src/exec/OptiXMain.cpp)
        target_link_libraries(OptiXExec SimDriver)
    endif ()
endif ()


add_executable(MeshExec src/exec/MeshMain)
target_link_libraries(MeshExec SimDriver)

add_executable(HeightsExec src/exec/HeightsMain)
target_link_libraries(HeightsExec SimDriver)
