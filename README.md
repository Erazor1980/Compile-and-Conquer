# Compile and Conquer

A real time strategy game built with modern C++ and SFML.

Build. Select. Command. Conquer.

## Current Features

- Game loop with update and render
- World managing all units
- Basic unit representation with position and rendering
- Single unit selection via left mouse click
- Hit detection based on unit radius
- Visual feedback for selected units

## Architecture

- Game  
  Handles main loop and input

- World  
  Manages units and selection logic

- Unit  
  Represents a single entity with state, rendering and hit detection

## Tech Stack

- C++17+
- SFML 3

## Next Steps

- unit movement
- basic commands (move)
- multi selection
- drag selection
- pathfinding
- collision handling