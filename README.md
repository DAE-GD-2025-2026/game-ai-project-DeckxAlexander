# Game AI Project


This project was created for the Algorithms 2 course at Digital Arts & Entertainment. It explores the fundamentals of AI movement behaviour and the techniques commonly used in games to simulate autonomous agents.

## Features

The current features of the project include:
### Steering Behaviours
- Seek
- Flee
- Arrive
- Wander

### Flocking
- Separation
- Alignment
- Cohesion

### Space Partitioning
Grid-based spatial partitioning for efficient neighbor searching.

### Pathfinding
- A* pathfinding
- NavGraph
- SSFA

---

## Extra Assignment
For the extra assignment I added a **fallback path** to the A* pathfinding algorithm. 

Now, if the goal cannot be reached, the algorithm returns a path to the node closest to the target instead of failing.

---

## Tech

- Unreal Engine
- Unreal C++

---

## How to run

1. Clone or download this repository.
2. Open the project folder.
3. Locate the `.uproject` file.
4. Double-click the `.uproject` file to open the project in **Unreal Engine**.
5. Once the project is loaded, press **Play** in the Unreal Editor to run the simulation.

