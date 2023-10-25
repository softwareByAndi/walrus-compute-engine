# development resources

## vulkan:
- [vulkan-tutorial.com](https://vulkan-tutorial.com)
  - [vulkan-tutorial pdf](./notes/vulkan_tutorial_en.pdf)
- [vkguide](https://vkguide.dev/docs/introduction/vulkan_overview)
- [vma documentation (vulkan memory allocation)](./third_party/vma/documentation/html/index.html)
- __mike bailey's vulkan course content__
  - [vulkan course content](https://web.engr.oregonstate.edu/~mjb/cs519v/)
  - [vulkan course files & references](https://web.engr.oregonstate.edu/~mjb/vulkan/) 


## blog posts
- [KHRONOS new task/mesh pipeline](https://www.khronos.org/blog/mesh-shading-for-vulkan)
  __(TODO : implement this)__
- [AMD open sources V-EZ (a vulkan wrapper library)](https://hub.packtpub.com/amd-open-sources-v-ez-the-vulkan-wrapper-library)

## youtube:
- [vulkan graphics pipeline overview](https://www.youtube.com/watch?app=desktop&v=_riranMmtvI)


## third party library options:
- [ReactPhysics3D](https://www.reactphysics3d.com)

## NOTES

### engine initialization
  - [instantiating vulkan objects](./notes/engine_initialization/instantiating_vulkan_objects.md)
  - [vkguide notes instance, device & swap](./notes/engine_initialization/vkguide_instance_device_swap.md)
  - __implementation notes:__
    - [procedure /  order of operations](./notes/engine_initialization/notes_init/0_notes_init.md)
    - [init vulkan](./notes/engine_initialization/notes_init/1_init_vulkan.md)
    - [init commands](./notes/engine_initialization/notes_init/2_init_commands.md)
    - [init sync structures](./notes/engine_initialization/notes_init/3_init_sync_structures.md)
    - [init swapchain](./notes/engine_initialization/notes_init/4_init_swapchain.md)

### compute
- __device__
  - [enumerating physical devices](./notes/compute/device/notes.md?#enumerating-physical-devices)
  - [retrieving properties and features](./notes/compute/device/notes.md?#retrieving-properties-and-features)
  - [difference between properties and features](./notes/compute/device/notes.md?#whats-the-difference-between-properties-and-features-gpt35)

- __commands__
  - [vulkan command execution](./notes/compute/commands/vkguide_queue_commandbuffer.md?#vulkan-command-execution)
  - [VkQueue](./notes/compute/commands/vkguide_queue_commandbuffer.md?#vkqueue)
  - [VkCommandPool](./notes/compute/commands/vkguide_queue_commandbuffer.md?#vkcommandpool)
  - [VkCommandBuffer](./notes/compute/commands/vkguide_queue_commandbuffer.md?#vkcommandbuffer)

- __synchronize__
  - [VkFence](./notes/compute/synchronize/notes_fence_semaphore.md?#vkfence)
  - [VkSemaphore](./notes/compute/synchronize/notes_fence_semaphore.md?#vksemaphore)

### rendering
- __renderpasses__
  - [Renderpasses](./notes/rendering/renderpasses/vkguide_renderpass_imagelayouts_framebuffer.md?#renderpasses)
  - [Subpasses](./notes/rendering/renderpasses/vkguide_renderpass_imagelayouts_framebuffer.md?#subpasses)
  - [Image Layouts](./notes/rendering/renderpasses/vkguide_renderpass_imagelayouts_framebuffer.md?#image-layouts)
  - [framebuffers ??](./notes/rendering/renderpasses/vkguide_renderpass_imagelayouts_framebuffer.md?#framebuffers)
    - TODO : framebuffer note is kind of empty...