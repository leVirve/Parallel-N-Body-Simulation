# Parallel N-Body Simulation

Multiple parallel and fast implementations of N-body simulation.


![Demo example](report/imgs/demo2.png)

Here are four implementations:
- Sequential
- Parallel in `OpenMP`
- Parallel in `pthread`
- Use `Barnes-Hut` algorithm and parallel in `pthread`

# Compare each version
![](report/imgs/strong_scalability.png)

# Cost in each Barnes-Hut phase
![](report/imgs/barnes_hut_phases.png)
