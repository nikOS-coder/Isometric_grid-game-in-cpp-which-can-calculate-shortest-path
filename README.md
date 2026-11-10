# C++ SFML Isometric Grid & Heightmap Pathfinding Simulation

An interactive 2D-to-3D isometric graphics engine and pathfinding simulation built in **C++** using the **SFML** library. It features a dynamic heightmap editor, isometric matrix projection mapping, mouse picking, camera controls (zoom/pan), and an automated **A* (A-Star) pathfinding** agent navigating complex terrain heights.

## Key Features

1. **Isometric Projection & Matrix Math:** Uses $2\times2$ transformation matrices to accurately project 2D grid coordinates into isometric screen space, along with matrix inversion for precise mouse picking.
2. **Dynamic Heightmap Editor:** 
   - **Left Click:** Lower the height of individual grid tiles.
   - **Right Click:** Raise the height of individual grid tiles.
3. **A* Pathfinding Agent (`hero`):**
   - **Shift + Left Click:** Set the start position.
   - **Ctrl + Left Click:** Set the destination position.
   - Automatically computes the shortest valid path using the A* algorithm while respecting terrain climb limits (`maxClimb`).
   - **Spacebar:** Triggers smooth character interpolation along the computed path.
4. **Camera Controls:** 
   - **Arrow Keys:** Pan the camera around the world.
   - **Mouse Wheel:** Smooth zoom anchored on the cursor position.

---

## Controls Summary

| Action | Input / Command |
| :--- | :--- |
| **Edit Tile Height (Lower)** | Left Mouse Button |
| **Edit Tile Height (Raise)** | Right Mouse Button |
| **Set Start Point** | `Left Shift` + Left Click |
| **Set Destination Point`** | `Left Control` + Left Click |
| **Start Hero Movement** | `Spacebar` |
| **Pan Camera** | Arrow Keys (`Up`, `Down`, `Left`, `Right`) |
| **Zoom Camera** | Mouse Scroll Wheel |

---

## Tech Stack

- **Language:** C++ (Standard C++11)
- **Graphics Library:** **SFML** (`sf::RenderWindow`, `sf::ConvexShape`, `sf::View`, events, vector math)
- **Algorithms:** **A* Search Algorithm**, Matrix transformations & Inversions
- **Build System:** Makefile / CMake

---

## Prerequisites & Compilation

Ensure you have **SFML** installed on your system.

```bash
# Ubuntu / Debian
sudo apt-get install libsfml-dev

# Fedora / RHEL
sudo dnf install sfml-devel

# macOS (Homebrew)
brew install sfml
Build & Run using Make
This project includes a simple Makefile for easy compilation.

Build the project:

Bash
make
Run the simulation:

Bash
./isometric_sim

Author
Christopoulos Nikolas