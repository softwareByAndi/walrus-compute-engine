# init_commands

- assertions
- create command pool
- create command buffer
- add lambda to destruction queue

## command pool

### assertions

``` cpp
// FIXME : hard coded queue index??
const int queueFamilyIndex = 0;

assert(
    _device != VK_NULL_HANDLE 
    && "device not setup"
);
assert(
    !_queues.empty() 
    && !_deviceInfo.queueData.empty() 
    && "missing queues?"
);
assert(
    _deviceInfo.queueData[queueFamilyIndex].support.compute 
    && "current logic only supports queues with at least compute..."
);
assert(
    !(_task & DeviceTask::GRAPHICS) 
    || _deviceInfo.queueData[queueFamilyIndex].support.isComplete() 
    && "current logic only supports queues with complete support..."
);
```

### create
``` cpp
auto createInfo = CommandPool::createInfo(
    queueFamilyIndex, 
    VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
);
VK_CHECK(vkCreateCommandPool(
    _device, 
    &createInfo, 
    nullptr, 
    &_commandPool
));
```

## command buffer
``` cpp
auto allocInfo = CommandBuffer::allocateInfo(_commandPool);
VK_CHECK(vkAllocateCommandBuffers(
    _device, 
    &allocInfo, 
    &_commandBuffer
));
```

## destruction queue
``` cpp
// see DestrutionQueue in src/engine/vk_engine.h
DestructionQueue _mainDestructionQueue{};

...

_mainDestructionQueue.addDestructor(
    // add a lambda to the queue
    [=]() { 
        vkDestroyCommandPool(
            _device, 
            _commandPool, 
            nullptr
        ); 
    }
);
```

