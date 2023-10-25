# TODO:

Note that this document is subject to drastic changes

# research tasks
- [ ] read this blog post (and subsequent research to understand it): [KHRONOS new task/mesh pipeline](https://www.khronos.org/blog/mesh-shading-for-vulkan)
- [ ] read the vulkan tutorial: [vulkan-tutorial.com](https://vulkan-tutorial.com)
- [ ] read through mike bailey's vulkan course
- [ ] vulkan allocators
- [ ] swapchain : post processing
- [ ] swapchain imageSharingMode : cover ownership chapters to implement exclusive logic instead of concurrent logic

# bare-bones: 

### basic vulkan setup:

finish basic vulkan setup from ( [vulkan-tutorial.com](https://vulkan-tutorial.com) )  
- [x] presentation
- [ ] device queue abstraction
  - [x] select best queue across all devices (single queue) 
- [ ] graphics pipeline  
- [ ] drawing  
- [ ] dynamic swapchain recreation  
- [ ] vertex buffers  
- [ ] uniform buffers  
- [ ] texture mapping  
- [ ] depth buffering  
- [ ] loading models  
- [ ] generating mipmaps  
- [ ] multisampling  


### Voxel Terrain Generation
- [ ] implement the Transvoxel algorithm for generating voxel terrain (solid colors)


### scene abstraction:

- [ ] basic entity component system  
  - _nothing complicated. We'll refactor everything later_

- [ ] abstract scene building  
  - _don't bother with GUI scene builder yet._
  - [ ] add `.scene` scripts w/ live re-load  

- [ ] add basic camera controls  
  - [ ] camera movement  
  - [ ] camera rotation  
  - [ ] camera zoom  
  - [ ] camera FOV


### basic physics:

eventually this will be handled by a molecular abstraction engine, but for now, lets get some basic box physics

- [ ] add [ReactPhysics3D](https://www.reactphysics3d.com)
  - [ ] need basic collision detection _(a number of collision shapes for each object)_
    - [ ] collision detection
    - [ ] collision resolution
    - [ ] collision shapes
  - [ ] need basic physics:
    _( for now, objects with complex movements will be simulated using a set of objects tied together using anchors )_
    - [ ] gravity
    - [ ] acceleration
    - [ ] velocity
    - [ ] inertia
    - [ ] anchor points

<br>
<br>
<hr>
<br>

# bare-bones open world test:

build a low-poly infinite world with basic terrain generation, basic physics, and simple biomes w/ randomly generated trees, rocks, and other objects.
- shoot for a voxel based minecraft knock-off

  - terrain edits:
    - [ ] create
    - [ ] destroy
    - [ ] smooth
    - [ ] flatten
  - player: 
    - [ ] controls
    - [ ] inventory
    - [ ] crafting
    - [ ] building
    - [ ] combat
  - enemy:
    - [ ] logic.
    - [ ] combat
    - [ ] spawning
    - [ ] loot
    - [ ] pathfinding
  - npc:
    - [ ] logic
    - [ ] dialogue (chatGPT?)
    - [ ] trading
    - [ ] quests
    - [ ] spawning
    - [ ] loot
    - [ ] pathfinding
    - [ ] combat
    - [ ] inventory


<br>
<br>
<hr>
<br>

# bare-bones A.I. simulation test:

the engine at this point will be fine for some A.I. simulations. Let's get something working.

- car
  1. create testing environment for car
  2. basic q-learning (local state-space, no look ahead)
  3. save and load brains
  4. add look ahead
  5. update state-space to use sensor inputs
  6. test and refine parameters


- abstract for drone
  1. abstract car A.I.
  2. update for drone moves and state-space
  3. update obstacles for drone test environment
  4. test & refine params.


- upgrade drone A.I
  1. add dual camera depth map
  2. add environment mapping
  3. add image recognition
  4. update environment mapping to include recognized image location tags

<br>
<br>
<hr>
<br>

# bare-bones game test:

Simulations for simulations' sake are boring. Let's give them a quick application.

### war game: 

castle defence w/ tanks & drones in procedurally generated environments

- __Player goal (optional):__ 
  - infiltrate enemy lines & deliver payload.

- __Player side A.I. goal:__ 
  - defend castle
  - recon & update player with enemy positions.

- __Enemy side A.I. goal:__ 
  - defend castle
  - recon & update enemy positions, 
  - infiltrate enemy lines & deliver payload to player castle.

<br>
<br>
<hr>
<br>

# bare-bones performance baseline

let's get a baseline to measure the performance of any updates from here on out.

- [ ] fluid & air simulation
- [ ] combustion engine simulation 

### prop plane: 

- [ ] combine the fluid sim, combustion sim & procedural generation into a single prop engine flight simulator
- [ ] add storm environment
- [ ] how many planes can the GPU simulate?

<br>
<br>
<hr>
<br>

# bare-bones performance optimizer:

this is where it gets fun!

### replace expensive simulation calculations with cheap A.I. logic:

we'll get to this when we get closer to it.

- [ ] Q-Learning
- [ ] HUD

<br>
<br>
<hr>
<br>

# optimizer game test: dog fight game

perhaps implement this after the performance optimizer step

- [ ] update procedural generation for larger terrains
- [ ] refactor drone A.I. to work with prop plane
- [ ] A.I. goal:
  - shoot down enemy aircraft
  - dodge enemy attacks
  - don't crash
- [ ] add as many A.I. planes as the GPU can handle
  - if a plane is shot down then show explosion animation and re-spawn

<br>
<br>
<hr>
<br>

# open world sim:

todo -- we'll get to this later...

### NPC A.I.  
todo  
- [ ] refactor optimizer for human learned tasks:  
  _( i.e. muscle memory )_
  - etc..

### basic crafting:  
todo...
- [ ] refactor optimizer for crafted inventions:  
  - etc...

### mini civilization emergent behavior  
todo...  
  - [ ] genetic + random mutation
    - i.e. children are born as learned and mature as their parents
  - [ ] refactor optimizer for human behavior:  
    _( i.e. habit forming )_
    - don't disable learning & ability to break habits
    - etc...

### quests
todo...  
- [ ] player can somewhat converse with NPCs
- [ ] NPCs can issue quests

### etc...