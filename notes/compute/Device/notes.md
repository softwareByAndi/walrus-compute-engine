# Device
## enumerating physical devices
```cpp
// see DeviceInfo::getDeviceInfos(...) in src/engine/compute/device/device.cpp

uint32_t deviceCount = 0;
std::vector<VkPhysicalDevice> vkPhysicalDevices{};

/// 1. call once to get the number of devices
/// 2. resize the vector to hold the number of devices
/// 3. call again to get the actual devices
vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
vkPhysicalDevices->resize(deviceCount);
vkEnumeratePhysicalDevices(instance, &deviceCount, vkPhysicalDevices->data());
```

## retrieving properties & features
```cpp
VkPhysicalDeviceProperties properties{};
VkPhysicalDeviceFeatures features{};

...

vkGetPhysicalDeviceProperties(vkPhysicalDevice, &properties);
vkGetPhysicalDeviceFeatures(vkPhysicalDevice, &features);
```


## what's the difference between properties & features? (gpt3.5)
In Vulkan, `device properties` and `device features` are two different sets of information that describe the capabilities and characteristics of a Vulkan-capable graphics device (GPU). 

In brief, `device properties` provide static information about the GPU, such as its name, type, and supported Vulkan version, while `device features` describe the functional capabilities and optional features that the GPU supports, which can affect how you configure and use Vulkan in your application.

When developing a Vulkan application, you may use both device properties and features to determine the capabilities of the GPU and make runtime decisions based on what the GPU can support. For example, you might check device features to decide whether to enable certain optional graphics features or use specific Vulkan extensions.

### physical characteristics v.s. functional capabilities

Device `properties` provide information about physical characteristics and capabilities of the GPU, and might include details such as:

- device name,
- device type (e.g., integrated or discrete),
- vendor ID,
- device ID,
- supported Vulkan API version
- etc...

Device `features` describe the functional capabilities and optional features that a GPU supports, and might include support for specific features such as:

- Vulkan extensions,
- features like geometry shaders _(optional)_,
- texture compression formats _(optional)_,
- etc...

### static v.s. dynamic information ( application level )

Device `properties` are typically static and do not change during the runtime of your application. 

Device `features`, on the other hand, can vary between GPUs and may even change based on the chosen Vulkan device and configuration.
