# VkFence

used for GPU --> CPU communication so that the CPU can know when the GPU has finished executing operations.

```c++
// pseudocode example

VkFence myFence;

Vk_XXXXXXX(..., myFence);

VkWaitForFences(myFence);

VkResetFences(myFence);
```

# VkSemaphore

used for GPU to GPU syncronization allowing GPU order of operations to be defined for a series of commands.  
- i.e. enables commands to run one after another.  

semaphore examples include  `Signal` `Wait`.

- `Signal`: operation will immediately `lock` said semaphore when it executes and `unlock` when it finishes execution
- `Wait`: operation will `wait` until that semaphore is `unlocked` to begin execution;

```c++
// pseudocode example
// executes op_1, op_2 & op_3 in order

struct Semaphores {
  VkSemaphore first{};
  VkSemaphore second{};
};
Semaphores semaphores{};

VkOperationInfo opInfo_1{};
opInfo_1.signalSemaphore = semaphores.first;
...
Vk_XXXXXXX(..., opInfo_1);

VkOperationInfo opInfo_2{};
opInfo_2.signalSemaphore = semaphores.second;
opInfo_2.waitSemaphore = semaphores.first;
...
Vk_XXXXXXX(..., opInfo_2);

VkOperationInfo opInfo_3{};
opInfo_3.waitSemaphore = semaphores.second;
...
Vk_XXXXXXX(..., opInfo_3);
```