# Compile and Conquer

Minimal RTS prototype in C++ and SFML.

## Features

- Selection: single, drag, multi (Ctrl)
- Commands: move, attack, stop
- Basic combat: range, damage, cooldown
- Units follow targets and are removed on death
- Tank unit type
- Hover, selection and command preview markers
- Debug visuals (hitpoints, ranges)

## Controls

- Left Click: select
- Drag: multi select
- Ctrl + Click: add/remove selection
- Right Click: move / attack
- S: stop
- D: debug
- Esc: quit

## Architecture

- `Game`: loop, input
- `World`: units, selection, commands
- `Unit`: movement, combat

## Tech

- C++17+
- SFML 3