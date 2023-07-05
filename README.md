# SimulationTraining

## Introduction
The intent of this repository is to see how much a simple 2d physics simulation can be optimized, and document the optimization steps.

## Projects
### Physics01
Physics01 contains a basic CPU-based 2d physics simulation with basic particle collision logic. It can handle a very low amount (50-100) of particles before the screen refresh rate is affected.

### Physics02
Optimization 1: Check if circles intersect without using glm::distance<br>
Effect: About 30% increase in particles<br>
Commit: https://github.com/klillas/SimulationTraining/commit/cabf469506984e8fdfccbfb0a8a72d67105cbdab<br>
![image](https://github.com/klillas/SimulationTraining/assets/7252976/4f2cb550-d8fe-4364-a21b-28a6896e50f6)

Optimization 2: Use a simple struct to contain the molecule properties rather than a move involved multi-level class model with inheritance<br>
