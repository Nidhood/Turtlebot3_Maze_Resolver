# turtlebot3_maze_gazebo

Static Gazebo Sim spawner for the 16x6 maze used by `turtlebot3_maze_resolver`.

## Geometry

- Grid: 16 x 6
- Cell size: 0.50 m
- Footprint: 8.00 m x 3.00 m
- Wall thickness: 0.03 m
- Wall height: 0.30 m
- Wall material: near-black

The package does **not** start Gazebo. Start your simulation first, then:

```bash
ros2 launch turtlebot3_maze_gazebo spawn_maze.launch.py
```

## Local cell-center coordinates

For zero maze pose / zero yaw:

```text
x = -4.0 + (column + 0.5) * 0.5
y = +1.5 - (row + 0.5) * 0.5
```

Example:

```text
cell(row=0, col=0)  -> (-3.75, +1.25)
cell(row=5, col=15) -> (+3.75, -1.25)
```
