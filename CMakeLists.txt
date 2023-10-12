# CMakeList.txt : CMake project for walrus_compute_engine.

cmake_minimum_required (VERSION 3.8)

project ("walrus_compute_engine")

# FIXME : where to set optimization for c++ compiler?
set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(glm REQUIRED)

message("")
message("Vulkan found: ${Vulkan_FOUND}")
message("GLFW found: ${glfw3_FOUND}")
message("GLM found: ${glm_FOUND}")
message("")

message("Vulkan include: ${Vulkan_INCLUDE_DIRS}")
message("Vulkan libraries: ${Vulkan_LIBRARIES}")
message("Vulkan definitions: ${Vulkan_DEFINITIONS}")
message("Vulkan dir: ${vulkan_DIR}")
message("Vulkan SDK: $ENV{VULKAN_SDK}")

message("")

add_subdirectory(third_party)
add_subdirectory(src)

# FIXME : remove specific string here.
#include_directories($ENV{vulkan_DIR}/include "/opt/homebrew/include")
include_directories($ENV{vulkan_DIR}/include $ENV{Vulkan_INCLUDE_DIRS})


find_program(GLSL_VALIDATOR glslangValidator HINTS /usr/bin /usr/local/bin $ENV{VULKAN_SDK}/Bin/ $ENV{VULKAN_SDK}/Bin32/)
message("GLSL_VALIDATOR = ${GLSL_VALIDATOR}")
message("")

## find all the shader files under the shaders folder
file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/shaders/*.frag"
    "${PROJECT_SOURCE_DIR}/shaders/*.vert"
    "${PROJECT_SOURCE_DIR}/shaders/*.comp"
    "${PROJECT_SOURCE_DIR}/shaders/**/*.frag"
    "${PROJECT_SOURCE_DIR}/shaders/**/*.vert"
    "${PROJECT_SOURCE_DIR}/shaders/**/*.comp"
)

## iterate each shader
foreach(GLSL ${GLSL_SOURCE_FILES})
  message(STATUS "BUILDING SHADER")
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}.spv")
  message(STATUS ${GLSL})
  ##execute glslang command to compile that specific shader
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
)
