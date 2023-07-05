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
Effect: About 100% increase in particles<br>
Commut: https://github.com/klillas/SimulationTraining/commit/6095175a8be06f7c51570a1fd1b5f7784bf105aa

Original implementation<br>
![image](https://github.com/klillas/SimulationTraining/assets/7252976/ad57d70d-4a9e-489e-adf3-dd1f3be2eefe)

Simplified implementation with a data struct containing molecule properties and helper functions in a static class<br>
![image](https://github.com/klillas/SimulationTraining/assets/7252976/53390a04-e1e2-4041-8c70-27a9958cd82b)
