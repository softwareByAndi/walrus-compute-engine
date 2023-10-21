# init_sync_structures
see [notes_fence_semaphore.md](/notes/compute/synchronize/notes_fence_semaphore.md)
- fences
- semaphores
- add to destruction queue

## fences
``` cpp
sync::generics::RenderSync<VkFence> _fences{}; // rendering fences
std::vector<VkFence> _fencePool{}; // raw fences

...

VkFenceCreateInfo info{};
info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
info.pNext = nullptr;
info.flags = VK_FENCE_CREATE_SIGNALED_BIT; // signal == mutex lock

// add a null handle to the pool & instantiate
_fencePool.push_back(VK_NULL_HANDLE);
VK_CHECK(vkCreateFence(_device, &info, nullptr, &_fencePool.back()));

// attach raw fence to RenderSync
_fences.pRender = &_fencePool.back();
```

## semaphores
``` cpp
sync::generics::RenderSync<VkSemaphore> _semaphores{}; // rendering semaphores
std::vector<VkSemaphore> _semaphorePool{}; // raw semaphores

...

VkSemaphoreCreateInfo info{};
info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
info.pNext = nullptr;
info.flags = 0;

// create 2 semaphores for presentation & rendering
size_t index = _semaphorePool.size(); // defaults to 0?
_semaphorePool.push_back(VK_NULL_HANDLE);
_semaphorePool.push_back(VK_NULL_HANDLE);
VK_CHECK(vkCreateSemaphore(_device, &info, nullptr, &_semaphorePool.at(index)));
VK_CHECK(vkCreateSemaphore(_device, &info, nullptr, &_semaphorePool.at(index + 1)));

// attach raw semaphores to RenderSync
_semaphores.pPresent = &_semaphorePool.at(index);
_semaphores.pRender = &_semaphorePool.at(index + 1);
```


## add to destruction queue
``` cpp
_mainDestructionQueue.addDestructor(
    // add a lambda to the queue
    [=]() {
        vkWaitForFences(
            _device,            // device
            _fencePool.size(),  // fence count
            _fencePool.data(),  // fence references
            true,               // wait for all fences
            1'000'000'000       // timeout
        );
        for (auto &fence: _fencePool) {
            vkDestroyFence(_device, fence, nullptr);
        }
        // TODO: are we not supposed to wait for semaphores the same as for fences?
        for (auto &semaphore: _semaphorePool) {
            vkDestroySemaphore(_device, semaphore, nullptr);
        }
    }
);
```