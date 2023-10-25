# DevLog
See `README.md` for development overview.  
See `todo.md` for current task breakdown.


# Mon Nov 14 2022, 14:38
Updated todo & development plan.

# Mon Nov 14 2022, 13:08  
Still working on vulkan bare-bones. Have mostly been trying to figure out the Vulkan lifecycle, how it interacts with the GPU, and what the crap they're talking about...  It was a bit confusing, but I understand most of it now. Enough for the bare-bones project.

We'll refactor later anyway, so optimized engine code can wait until then. In which case, the Vulkan bare-bones should be finished in 2 days; There's quite a lot to do.  

After that is probably a quick software design refactor, and then a scene-builder abstraction.  

# Tues Oct 11 2023, 19:52
wow, it's been a year already? I'm back! 

Current dev environments are Ubuntu Linux (v22.04) and MacOS (Arm M1). I updated the make file to work on Linux but may have broken it for mac, I will confirm and fix once the project runs on linux. 

I also added a test script for confirming initialization of the Vulkan environment on the computer.

Current tasks:
- The M1 integrated graphics supports any graphics usage across all threads, but normal graphics cards aren't so nice. They usually have threads dedicated to specific tasks, and so I will have to refactor the queues logic to support queues of different types.
- Now would be a good time to go through the vulkan tutorial again and add detailed comments to the code. I have a rough understanding of what's going on but have forgotten too much, and the project is small enough that it shouldn't take too long.
- While I'm at it, it might be a good idea to map out the engine structure visually for future reference. I'll probably use draw.io for this.

# Wed Oct 24, 2023, 19:32
mostly commenting code and re-learning vulkan. 

Device & queue logic has been fixed. it now accepts a single queue family from any device, so long as that queue family has complete support.

todo:
- render meshes. (currently only rendering a triangle, hard coded in the shader)
- enable devices to support multiple queue families. ( eventually? )