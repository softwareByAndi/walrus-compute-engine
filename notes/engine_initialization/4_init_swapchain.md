# init_swapchain

- swapchain
- image views
- add to destruction queue

## swapchain

``` cpp
// see init_swapchain() in src/engine/vk_engine.cpp
VkSwapchainCreateInfoKHR createInfo{};
createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
createInfo.surface = _surface;

createInfo.minImageCount = imageCount;
createInfo.imageFormat = vkSurfaceFormat.format;
createInfo.imageColorSpace = vkSurfaceFormat.colorSpace;
createInfo.imageExtent = _swapchainExtent;
createInfo.imageArrayLayers = 1; // always 1 unless creating stereoscopic 3D application

createInfo.preTransform = _swapchainSupportDetails.capabilities.currentTransform; // 90deg, flip, etc...
createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
createInfo.presentMode = vkPresentMode;
createInfo.clipped = VK_TRUE; // delete pixels that are covered by other pixels

createInfo.oldSwapchain = VK_NULL_HANDLE;

... // more createInfo parameters not mentioned. 
```

### get support details
...
### select surface format
...
### select presentation mode
...
### select swap extent
...
### set image count
...

<br>
<hr>

## image views
``` cpp
_swapchainImageViews.resize(_swapchainImages.size());
for (size_t i = 0; i < _swapchainImages.size(); i++) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = _swapchainImages[i];
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = _swapchainImageFormat;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    
    VK_CHECK(vkCreateImageView(
        _device, 
        &createInfo, 
        nullptr, 
        &_swapchainImageViews[i]
    ));
    // no destruction queue -- image views are destroyed with the framebuffers
}
```

<br>
<hr>

## add to destruction queue
``` cpp
_mainDestructionQueue.addDestructor(
    // add a lambda to the queue
    [=]() { 
        vkDestroySwapchainKHR(
            _device, 
            _swapchain, 
            nullptr
        ); 
    }
);
```