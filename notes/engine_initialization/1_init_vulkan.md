# init_vulkan

- instance
- surface _(optional)_
- debug _(optional)_
- messenger _(optional)_
- device
- memory allocator

## instance

### define application parameters
``` cpp
VkApplicationInfo appInfo{};
appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
...
```

### compile extensions
``` cpp
std::vector<const char *> extensions = { ... }
```

### define instance parameters & attach reference to application parameters
this info struct is used to compile parameters /info for application, extensions, and validation layers
``` cpp
VkInstanceCreateInfo createInfo{};
createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
createInfo.pApplicationInfo = &appInfo;
{ // default params
  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();
  createInfo.enabledLayerCount = 0;
  createInfo.pNext = nullptr;
}
...
```

we can add validation layers if we want, too
``` cpp
// note that vkInit is a namespace with helper functions & default initializers

VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = vkInit::defaults::debugMessengerCreateInfo();
if (_enableValidationLayers) {
    // attach validation layers
    createInfo.enabledLayerCount = _validationLayers.size();
    createInfo.ppEnabledLayerNames = _validationLayers.data();
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;
}
// confirm validation support
assert(!_enableValidationLayers || vkInit::checkValidationLayerSupport(_validationLayers));
```

### create instance
``` cpp
#define VK_CHECK(x) assert(x == VK_SUCCESS)

...

VK_CHECK(vkCreateInstance(&createInfo, nullptr, &_instance));
```

<br>
<hr>

## surface
``` cpp
Window _window{800, 600, "Vulkan Window"}; // no need to create window for compute only tasks...
VkSurfaceKHR _surface = VK_NULL_HANDLE;

...

if (_task & DeviceTask::GRAPHICS) {
  _window.createWindowSurface(_instance, &_surface);
}
```

<br>
<hr>

## debug messenger
if you attached a debug validation support before, then we need to also attach a debug messenger
- `vkGetInstanceProcAddr` is used to dynamically load and retrieve the address of Vulkan instance-level functions (also known as instance-level entry points) that are not part of the Vulkan loader's static dispatch.
``` cpp
VkDebugUtilsMessengerEXT _debugMessenger = VK_NULL_HANDLE;

if (_enableValidationLayers) {
    // default params
    VkDebugUtilsMessengerCreateInfoEXT createInfo = vkInit::defaults::debugMessengerCreateInfo();
    // get non-static create function
    auto createExtension = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
        _instance,
        "vkCreateDebugUtilsMessengerEXT"
    );
    assert(createExtension != nullptr);
    vkResult success_fail = createExtension(
        _instance,
        &createInfo,
        nullptr, // <-- we can add an allocator here if we wish
        &_debugMessenger // <-- 
    )
    if (success_fail != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger");
    }
}
```

<br>
<hr>

## device
see `src/engine/compute/device/device.cpp` for more details
``` cpp
// information about a physical device
VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE; 

// a handle for interacting with a physical device
VkDevice _device = VK_NULL_HANDLE; 
```
### 1. get info about all available devices
``` cpp
// see DeviceInfo::getDeviceInfos(...) in src/engine/compute/device/device.cpp
// also see DeviceInfo::init(...) in src/engine/compute/device/device.cpp

task = deviceTask;
getPhysicalDeviceProperties(vkPhysicalDevice);
getQueueFamilyProperties(vkPhysicalDevice);
updateQueueSurfaceSupport(vkPhysicalDevice, (task & GRAPHICS) ? vkSurface : VK_NULL_HANDLE);
updateDeviceSupportSummary(vkPhysicalDevice, (task & GRAPHICS) ? vkSurface : VK_NULL_HANDLE);
calculateDeviceScore(vkPhysicalDevice);
```

### 2. select a device

### 3. create a `vkDevice` handle to reference the selected device
``` cpp
// see DeviceInfo::createLogicalDevice(...) in src/engine/compute/device/device.cpp
DeviceInfo::createLogicalDevice(_deviceInfo, _physicalDevice, &_device, _queues);
```

<br>
<hr>

## memory allocator
A memory allocator is a software component responsible for managing and allocating memory resources efficiently. Memory allocation is a crucial part of any application, especially in graphics-intensive applications like those using Vulkan.

- VMA is a popular open-source library specifically designed to help manage memory in Vulkan applications. It simplifies memory allocation and provides tools for efficient memory management, including automatic allocation strategies, defragmentation, and resource tracking.

The importance of memory allocation in Vulkan applications cannot be overstated. Vulkan gives developers low-level control over memory management, which can be very powerful but also complex. VMA and similar libraries abstract away much of this complexity, making it easier to allocate and manage memory efficiently, preventing memory leaks and fragmentation, and improving overall application performance.

``` cpp
VmaAllocatorCreateInfo info{};
info.physicalDevice = _physicalDevice;
info.device = _device;
info.instance = _instance;

vmaCreateAllocator(&info, &_allocator);
```
