# walrus compute engine

this engine is still in progress.  
See `devlog.md` for project status.  
See `todo.md` for current task breakdown.  

<br>
<hr>

# task overview
Rather than completing the intended project design in one fell swoop, I will:
- build the bare minimum for a new feature, 
- implementing some sort of simulation or application of the new feature, 
- refactor,
- repeating the process until the engine is complete.

<hr>

- [ ] We'll start with a basic, traditional game engine. Using simplified physics and low-poly meshes.
    - solid colors are fine, so for now we won't worry about implementing:
        - advanced lighting (point lights, specular, ray-casting, shadows, etc...)
        - animations (these will be handled through physics)
        - textures  

- [ ] A.I. requires much less infrastructure than F.E.M. so we'll start with that.
    - cars and drones are pretty simple, so we'll start with those.
    - _units move through magic. don't worry about animating wheels or propellers_  

- Let's test and refine our A.I. with a game simulation.
    - [ ] castle defense sim / game  

- [ ] particle physics
    - with some A.I. logic prototypes built and something to show for it, let's take the first step toward implementing the core of the physics engine. _( and therefore animations via F.E.M. physics! )_
    - We'll implement some basic particle physics for fluid/air & combustion simulations  

- We need to test the new particle physics on something...
    - [ ] build an engine simulation using the new combustion physics  
      _( ** this task might take some time... ** )_
    - [ ] build a plane simulation using the new engine & fluid physics  

- [ ] replace object-level F.E.Physics computation with custom A.I.:
    - At this point, the particle physics for this engine sim will take a lot of processing power that we won't have when implemented on a mass scale.
    - So this is the time to start building our optimizer logic.  
    - __Q-Learning__ should be easy enough for this. It might get a little RAM hungry, but it will work fine for now.   
      - When we get more unique FE-Physics Objects in the scene, we might want to eventually refine / replace this with a less RAM hungry implementation, like a Neural Network.  

- [ ] optimizer performance HUD
  - it sure would be nice to evaluate the performance of our F.E.Physics object (engine) in real-time  

- Let's test out our new optimizer prototype with another game
  - [ ] dog-fight game

<br>

### what should come next?
- build open world sim
  - add NPC A.I. logic (finite number of brains?)
  - build A.I. optimizer for "muscle memory" / common tasks?
  - add basic NPC engineering & architecture
  - update optimizer
  - add player U.I. and other basic game features
  - add dungeons and quests and other NPC types
  - update F.E.Physics
  - add player crafting engine