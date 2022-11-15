# Add source to this project's executable.
add_executable(walrus_compute_engine
        main.cpp
        vk_types.h
        vk_initializers.cpp
        vk_initializers.h
        pretty_io.hpp
        pretty_io.cpp
        engine/vk_engine.cpp
        engine/vk_engine.h
        engine/compute/device/device.cpp
        engine/compute/device/device.hpp
        engine/compute/commands/command.cpp
        engine/compute/commands/command.hpp
        engine/rendering/window/window.cpp
        engine/rendering/window/window.hpp
        engine/rendering/swapchain/swapchain.cpp
        engine/rendering/swapchain/swapchain.hpp)

set_property(TARGET walrus_compute_engine PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:walrus_compute_engine>")

target_include_directories(walrus_compute_engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" "$ENV{vulkan_DIR}/include" "/opt/homebrew/include")
target_link_libraries(walrus_compute_engine vma tinyobjloader stb_image "/opt/homebrew/lib/libglfw.3.3.dylib")

target_link_libraries(walrus_compute_engine Vulkan::Vulkan)
target_compile_features(walrus_compute_engine PRIVATE cxx_std_14)

add_dependencies(walrus_compute_engine Shaders)