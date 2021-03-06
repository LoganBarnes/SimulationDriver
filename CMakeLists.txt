cmake_minimum_required(VERSION 3.7)
project(SimDriver)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(SIM_BUILD_CUDA "Build CUDA files if available" OFF)
option(SIM_BUILD_TESTS "Build SimulationDriver unit tests" OFF)
option(SIM_BUILD_EXAMPLES "Build visual executables" ON)
option(SIM_VERBOSE_OUTPUT "Print verbose configuration updates" OFF)
option(SIM_OFFSCREEN "Render offscreen using OS Mesa" OFF)
option(SIM_USE_DEV_FLAGS "Compile with all the flags" OFF)

if (${SIM_BUILD_EXAMPLES})
    message("-- Building visual executables")
endif ()

if (NOT ${SIM_VERBOSE_OUTPUT})
    set(QUIET_FLAG QUIET)
endif ()

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()
message("-- Build type: ${CMAKE_BUILD_TYPE}")
if (${SIM_OFFSCREEN})
    add_definitions(-DOFFSCREEN)
endif ()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

######################
### EXTERNAL REPOS ###
######################
include(DownloadProject)

configure_file(src/ShaderConfig.hpp.in ${CMAKE_BINARY_DIR}/sim-driver/ShaderConfig.hpp)

# Download and unpack glfw at configure time
download_project(PROJ glfw
        PREFIX thirdparty
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG master
        UPDATE_DISCONNECTED 1
        ${QUIET_FLAG}
        )

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_USE_OSMESA ${SIM_OFFSCREEN} CACHE BOOL "" FORCE)

# compile glfw with current project
add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})

# Download and unpack imgui at configure time
download_project(PROJ imgui
        PREFIX thirdparty
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.50
        UPDATE_DISCONNECTED 1
        ${QUIET_FLAG}
        )

# Download and unpack glm at configure time
download_project(PROJ glm
        PREFIX thirdparty
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG master
        UPDATE_DISCONNECTED 1
        ${QUIET_FLAG}
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
        src/thirdparty/imgui_impl_glfw_gl3
        # glad
        src/thirdparty/glad/glad
        src/thirdparty/KHR/khrplatform
        )
target_include_directories(thirdparty SYSTEM PUBLIC
        ${glfw_SOURCE_DIR}/include
        ${imgui_SOURCE_DIR}
        ${glm_SOURCE_DIR}
        src/thirdparty
        )
target_link_libraries(thirdparty glfw)

#############################
### Project Configuration ###
#############################
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # ignore third party warnings
    target_compile_options(glfw BEFORE PRIVATE -w)
    target_compile_options(thirdparty BEFORE PRIVATE -w)
endif ()

if (NOT MSVC)
    set(INTENSE_FLAGS -Wall -Wextra -Winit-self -Winit-self
            -Wold-style-cast -Woverloaded-virtual -Werror
            -Wmissing-declarations -pedantic -Wuninitialized)
else ()
    set(INTENSE_FLAGS -WX)
endif ()

#########################
### Simulation Driver ###
#########################
set(SOURCE_FILES
        #shaders
        src/shaders/shader.vert
        src/shaders/shader.geom
        src/shaders/shader.frag
        src/shaders/shader_mac.frag
        # meshes
        src/sim-driver/meshes/MeshFunctions.cpp
        src/sim-driver/meshes/MeshHelper.cpp
        # renderers
        src/sim-driver/renderers/MeshRenderer.cpp
        src/sim-driver/renderers/RendererHelper.cpp
        # sim-driver
        src/sim-driver/Camera.cpp
        src/sim-driver/CameraMover.cpp
        src/sim-driver/OpenGLHelper.cpp
        src/sim-driver/WindowManager.cpp
        )

set(INCLUDE_FILES
        # meshes
        src/sim-driver/meshes/MeshFunctions.hpp
        src/sim-driver/meshes/MeshHelper.hpp
        # renderers
        src/sim-driver/renderers/MeshRenderer.hpp
        src/sim-driver/renderers/RendererHelper.hpp
        # sim-driver
        src/sim-driver/CallbackWrapper.hpp
        src/sim-driver/Camera.hpp
        src/sim-driver/CameraMover.hpp
        src/sim-driver/OpenGLHelper.hpp
        src/sim-driver/OpenGLSimulation.hpp
        src/sim-driver/OpenGLTypes.hpp
        src/sim-driver/SimCallbacks.hpp
        src/sim-driver/SimData.hpp
        src/sim-driver/SimDriver.hpp
        src/sim-driver/WindowManager.hpp
        )
# can copy these to an include location on install when that is implemented

add_library(SimDriver ${SOURCE_FILES} ${INCLUDE_FILES})

target_include_directories(SimDriver PUBLIC ${CMAKE_BINARY_DIR} src)
target_link_libraries(SimDriver thirdparty)

if (${SIM_USE_DEV_FLAGS})
    target_compile_options(SimDriver PUBLIC ${INTENSE_FLAGS})
endif ()

find_package(CUDA)
if (${SIM_BUILD_CUDA} AND CUDA_FOUND)
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

        include_directories(src/extra src ${CMAKE_BINARY_DIR})
        include_directories(SYSTEM ${OptiX_INCLUDE})

        cuda_add_library(SimDriverOptiX ${CUDA_SOURCE} OPTIONS ${CUDA_ARCH_FLAGS})

        set(CUDA_GENERATED_OUTPUT_DIR ${PROJECT_BINARY_DIR}/ptx)
        cuda_wrap_srcs(SimDriverOptiX PTX GENERATED_PTX
                ${OPTIX_PTX_SOURCE}
                OPTIONS -use_fast_math ${CUDA_ARCH_FLAGS}
                )
        target_sources(SimDriverOptiX PUBLIC ${GENERATED_PTX})

        target_link_libraries(SimDriverOptiX
                thirdparty
                ${optix_LIBRARY}
                ${optixu_LIBRARY}
                )

        target_link_libraries(SimDriver SimDriverOptiX)

        if (${SIM_BUILD_EXAMPLES})
            cuda_add_executable(OptiXExec src/exec/OptiXMain.cpp)
            target_link_libraries(OptiXExec SimDriver)
        endif ()
    endif ()
endif ()

if (${SIM_BUILD_EXAMPLES})
    add_executable(MeshExec src/exec/MeshMain.cpp)
    target_link_libraries(MeshExec SimDriver)

    add_executable(BounceExec src/exec/BounceMain.cpp)
    target_link_libraries(BounceExec SimDriver)
endif ()


if (${SIM_BUILD_TESTS})
    # Download and unpack googletest at configure time
    download_project(PROJ googletest
            PREFIX thirdparty
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG master
            UPDATE_DISCONNECTED 1
            ${QUIET_FLAG}
            )

    # Prevent overriding the parent project's compiler/linker settings on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    # compile googletest with current project
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)

    target_include_directories(gmock_main SYSTEM BEFORE INTERFACE
            "${googletest_SOURCE_DIR}/googletest/include"
            "${googletest_SOURCE_DIR}/googlemock/include")

    # add test coverage capabilities
    if (CMAKE_COMPILER_IS_GNUCXX AND CMAKE_BUILD_TYPE MATCHES "Debug")
        include(CodeCoverage)
        setup_target_for_coverage(${PROJECT_NAME}_coverage SimDriverTests coverage)
        set(PROFILE_FLAGS -fprofile-arcs -ftest-coverage)
        set(PROFILE_LIBS gcov)
        target_compile_options(SimDriver PRIVATE ${PROFILE_FLAGS})
        target_link_libraries(SimDriver ${PROFILE_LIBS})
    endif ()

    include(CTest)
    enable_testing()

    set(TEST_SOURCE_FILES
            src/testing/include_checks/meshes/MeshFunctionsIncludeTest.cpp
            src/testing/include_checks/meshes/MeshHelperIncludeTest.cpp
            src/testing/include_checks/renderers/MeshRendererIncludeTest.cpp
            src/testing/include_checks/renderers/RendererHelperIncludeTest.cpp
            src/testing/include_checks/CallbackWrapperIncludeTest.cpp
            src/testing/include_checks/CameraIncludeTest.cpp
            src/testing/include_checks/CameraMoverIncludeTest.cpp
            src/testing/include_checks/OpenGLHelperIncludeTest.cpp
            src/testing/include_checks/OpenGLSimulationIncludeTest.cpp
            src/testing/include_checks/OpenGLTypesIncludeTest.cpp
            src/testing/include_checks/SimCallbacksIncludeTest.cpp
            src/testing/include_checks/SimDataIncludeTest.cpp
            src/testing/include_checks/SimDriverIncludeTest.cpp
            src/testing/include_checks/WindowManagerIncludeTest.cpp

            src/testing/SimulationLoopTests.cpp
            src/testing/TemplateCompilationTests.cpp
            )

    add_executable(SimDriverTests ${TEST_SOURCE_FILES})
    target_link_libraries(SimDriverTests SimDriver gmock_main thirdparty ${PROFILE_LIBS})
    target_compile_options(SimDriverTests PRIVATE ${INTENSE_FLAGS} ${PROFILE_FLAGS})
    add_test(NAME sim-driver-test COMMAND SimDriverTests)
endif ()

