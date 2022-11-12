find_package(Vulkan REQUIRED)

add_library(vkbootstrap STATIC)
add_library(glm INTERFACE)
add_library(vma INTERFACE)

add_library(stb_image INTERFACE)

add_library(tinyobjloader STATIC)

target_sources(vkbootstrap PRIVATE
        vkbootstrap/VkBootstrap.h
        vkbootstrap/VkBootstrap.cpp
        )

target_include_directories(vkbootstrap PUBLIC vkbootstrap)
target_link_libraries(vkbootstrap PUBLIC Vulkan::Vulkan $<$<BOOL:UNIX>:${CMAKE_DL_LIBS}>)
target_compile_features(vkbootstrap PRIVATE cxx_std_14)

# vma is a header only lib so we only need the include path
target_include_directories(vma INTERFACE vma)

target_sources(tinyobjloader PRIVATE
        tinyobjloader/tiny_obj_loader.h
        tinyobjloader/tiny_obj_loader.cc
        )

target_include_directories(tinyobjloader PUBLIC tinyobjloader)

target_include_directories(stb_image INTERFACE stb_image)