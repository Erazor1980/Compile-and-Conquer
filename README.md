# Compile and Conquer

A real time strategy game built with modern C++ and SFML.

Build. Select. Command. Conquer.

## Current Features

- game loop with update and render
- world managing all units
- unit selection via click and drag
- selection limited to player units
- move command via right click
- stop command for selected units
- attack command on enemy units
- units follow targets and attack in range
- hitpoints, damage and attack cooldown
- dead units are removed from the world
- debug mode with hitpoint display

## Controls

- Left Mouse Button: select unit
- Left Mouse Button + drag: box selection
- Right Mouse Button on ground: move selected units
- Right Mouse Button on enemy: attack target
- S: stop selected units
- D: toggle debug info
- Escape: quit

## Architecture

- `Game` handles main loop and input
- `World` manages units, selection and commands
- `Unit` handles movement, state and combat
- one active command per unit

## Tech Stack

- C++17+
- SFML 3

## Next Steps

- visual combat feedback
- different unit stats and types
- improved movement behavior
- pathfinding
- collision handling