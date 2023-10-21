# Initialization
``` cpp
/// init required structures
init_vulkan();            /// vulkan instance, surface(opt), debug(opt), messenger, device, memory allocator
init_commands();          /// command pool, command buffers, destructor queue
init_sync_structures();   /// fences, semaphores, destructor queue

/// init graphics structures
if (_task & DeviceTask::GRAPHICS) {
    init_swapchain();     /// swapchain & images, image views, destructor queue
    init_renderpass();    /// render pass, destructor queue
    init_framebuffers();  /// framebuffers, destructor queue
    init_pipelines();     /// load shaders, pipeline-layout, pipelines, destroy shaders, destructor queue
    load_meshes();        /// test triangle
}
```

