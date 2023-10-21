https://vkguide.dev/docs/chapter-1/vulkan_init_flow/

# Vulkan initialization
Unlike OpenGL, which allowed you to execute graphics commands near-immediately, `Vulkan has a lengthy setup phase`. As Vulkan is a very explicit API that gives very “direct” control, you need to initialize it to do things such as load extensions, select which GPU (or multiple!) you are going to use, and create the initial VkInstance and VkDevice structures that you then use with Vulkan commands.

Vulkan has `no global state`, unlike OpenGL, so you need to `send the VkDevice or VkInstance with every API function call`. 

You can load the function pointers “manually”. This is done by loading the Vulkan library, and then loading the function pointers from the library. This is done with the vkGetInstanceProcAddr function, which is a function that is always available in the Vulkan library, and returns the function pointer for any Vulkan function you want. This is how the Vulkan loader works, and how it can load the functions for the Vulkan API.

# VkInstance
The `root of everything` is the VkInstance. This represents a Vulkan API context. When creating a VkInstance, you can enable validation layers if you want, what instance extensions you need, like VK_KHR_surface, and also hook a logger of your choosing for when the Vulkan driver has errors or needs to log something. `The main thing to do during instance creation is turning on validation layers and instance extensions.`

In general, applications only need to create a `single VkInstance` for their entire run, as it’s just the global Vulkan context for your application.

# VkPhysicalDevice
Once we have created a VkInstance, we can query it for what GPUs are available in the system.

Vulkan allows us to get a `list of what GPUs are in a system, and what their capabilities are`. All of this information is represented on the `VkPhysicalDevice` handle, which is a `reference to the “actual” GPU`. For example, in a dedicated gaming PC, there will likely only be 1 VkPhysicalDevice available, which is the dedicated gaming GPU. In this case, there is no need to choose between GPUs, as there is only one.

Where things get more interesting is in devices such as a laptop. Laptops commonly have 2 GPUs, one being the CPU integrated one (low power), and other being the dedicated GPU (high power). In such a case, an application will need to `decide what GPU to use for the rendering`, and optimally, leave the choice for the user, in the case he might want to use the less powerful dedicated GPU to preserve battery life.

Apart from choosing a GPU to use, `VkPhysicalDevice lets us query the features it has, the memory size of the GPU, or what extensions are available`. This is very important for advanced applications where you want to know exactly how much VRAM you have available, and if the GPU supports advanced features.

# VkDevice
Once we have the VkPhysicalDevice of the GPU we are going to use, we can `create a VkDevice` from it. This is the `actual GPU driver on the GPU hardware, and the way we communicate with said GPU`. Most of Vulkan commands outside of debug utils or initialization stuff need the VkDevice. A device is created with a `list of extensions that you want to enable`. It is highly recommended you do not enable extensions you don’t need, as they can cause the driver to be slower due to checking extra things.

One of the most important goals of Vulkan when it was created, is that `multi-GPU can be done “manually”`. This is done by creating `a VkDevice for each of the GPUs` you want to use, and then it is possible to `share data between VkDevices`. A candidate for this would be to create a VkDevice on your main dedicated GPU for the actual graphics, but keep a VkDevice for the integrated GPU to use to run some physics calculations or other data.

# Swapchain
Initializing the GPU is nice, but we want to actually perform some `rendering into the screen`. This is where the swapchain comes. Swapchains are not in the core Vulkan spec because they are `optional`, and often unique to the different platforms. If you run Vulkan headless, for example to render images and saving them to disk, or performing offline calculations, it’s not needed that you create a swapchain, `as swapchains are just for sending the images into the screen.`

A swapchain is created on a given size, and `if the window resizes, you will have to recreate the swapchain again.`

The format that the swapchain exposes for its images can be different between platforms and even GPUs, so it’s necessary that you `store the image format that the swapchain wants`, as rendering on a different format will cause artifacts or crashes.

`Swapchains are a list of images`, accessible by the operating system for display to the screen. You can create swapchains with more or less images, but generally you will want only `2 or 3 images` to perform double-buffer or triple-buffer rendering.

The most important thing when creating a swapchain is to `select a Present Mode`, this is how the swapchain “works” logically.

- `VK_PRESENT_MODE_IMMEDIATE_KHR` Makes the swapchain not wait for anything, and accept instant pushing of images. This will likely result in tearing, generally not recommended.
- `VK_PRESENT_MODE_FIFO_KHR` This will have a queue of images to present on refresh intervals. Once the queue is full the application will have to wait until the queue is popped by displaying the image. This is the “strong VSync” present mode, and it will lock your application to the FPS of your screen.
- `VK_PRESENT_MODE_FIFO_RELAXED_KHR` . Mostly the same as Fifo VSync, but the VSync is adaptive. If the FPS of your application are lower than the optimal FPS of the screen, it will push the images immediately, likely resulting in tearing. For example, if your screen is a 60 HZ screen, and you are rendering at 55 HZ, this will not drop to the next vsync interval, making your general FPS drop to 30 like Fifo does, instead it will just display the images as still 55 FPS, but with tearing.
- `VK_PRESENT_MODE_MAILBOX_KHR` . This one has a list of images, and while one of them is being displayed by the screen, you will be continuously rendering to the others in the list. Whenever it’s time to display an image, it will select the most recent one. This is the one you use if you want Triple-buffering without hard vsync.

`VK_PRESENT_MODE_IMMEDIATE_KHR` is rarely used due to its tearing. Only in extreme low latency scenarios it might be useful to allow the tearing.

Normal applications will use either MAILBOX or one of the 2 FIFO modes. Mostly depends if you want a hard-vsync or you prefer triple-buffering.

In this guide, we will be using the FIFO_RELAXED mode, because it implements a upper cap on our rendering speed, and as we aren’t going to render many objects, it’s best if the framerate is capped and not reaching 5000 FPS which can cause issues. On real applications that have some work to do, MAILBOX is the preferred mode by default.