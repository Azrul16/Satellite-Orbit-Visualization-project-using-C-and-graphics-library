# Satellite Orbit Visualization

**Course:** Computer Graphics and Image Processing Sessional  
**Course Teacher:** Md Mahabubur Rahman  
**Author:** Azrul Amaline

---

If you find this project helpful, please **⭐ give it a star** and **fork** the repository to save it to your account.

---

A C++ project that visualizes satellite orbits around Earth using a graphics library. It includes two programs: a simple single-satellite demo and a multi-satellite system with the Moon and orbital trails.

## Programs

### 1. `singleOrbit.cpp`

A minimal visualization with one satellite in a circular orbit around Earth.

- **Earth**: Blue filled circle at the center (fixed 640×480-style layout).
- **Orbit**: White circular path.
- **Satellite**: Red filled circle moving along the orbit, with a line from Earth to the satellite.
- **Controls**: Press any key to exit.
- **Animation**: Angle updates at 0.03 rad/frame with ~40 ms delay.

### 2. `multi_satellite_orbit_system.cpp`

An extended visualization with Earth, the Moon, and three satellites with different orbit types.

| Object   | Description                    | Color   |
|----------|--------------------------------|---------|
| Earth    | Center body with continent-like patches | Blue/Green |
| Moon     | Circular orbit around Earth    | Light gray |
| Sat-1    | Circular orbit                 | Yellow  |
| Sat-2    | Elliptical orbit               | Green   |
| Sat-3    | High circular orbit            | Red     |

**Features:**

- Starfield background (random white pixels).
- Info panel listing Earth, Moon, and the three satellites.
- Orbit paths drawn (circular and elliptical).
- Satellite sprites (body + cyan solar panels).
- Trail points left along orbits.
- Decorative lines between satellites.
- On-screen display of orbital angles for Sat-1, Sat-2, Sat-3.
- Different angular speeds per object; Moon and satellites animate independently.

**Controls:** Press any key to stop and exit.

## Requirements

- **Language**: C++
- **Graphics**: Uses the classic BGI-style API (`graphics.h`, `conio.h`, etc.), typically from:
  - **Turbo C/C++** (DOS), or
  - **WinBGI** / **SDLBGI** / similar ports for Windows or other platforms

Headers used: `graphics.h`, `conio.h`, `math.h`, `dos.h`, `stdlib.h`, `time.h`, `stdio.h`.

## Building & Running

### Clone the repository

```bash
git clone https://github.com/YOUR_USERNAME/Satellite-Orbit-Visualization-project-using-C-anf-graphics-library.git
cd "Satellite-Orbit-Visualization-project using C++ anf graphics library"
```

**Windows (PowerShell):**

```powershell
git clone https://github.com/YOUR_USERNAME/Satellite-Orbit-Visualization-project-using-C-anf-graphics-library.git
cd "Satellite-Orbit-Visualization-project using C++ anf graphics library"
```

### Build & run from shell

**Option 1 — Turbo C (DOS)**  
Open the project in Turbo C, add the relevant `.cpp` file, then compile and run.

**Option 2 — WinBGI (Windows)**  
Install WinBGI, then from the project folder:

```bash
# Example: compile single-orbit demo (adjust paths to your WinBGI install)
g++ singleOrbit.cpp -o singleOrbit -I"C:\path\to\winbgim" -L"C:\path\to\winbgim" -lgraphics -lgdi32 -limm32 -lmsimg32 -lole32 -loleaut32 -lwinmm -luuid
./singleOrbit
```

```bash
# Example: compile multi-satellite demo
g++ multi_satellite_orbit_system.cpp -o multi_satellite -I"C:\path\to\winbgim" -L"C:\path\to\winbgim" -lgraphics -lgdi32 -limm32 -lmsimg32 -lole32 -loleaut32 -lwinmm -luuid
./multi_satellite
```

**Option 3 — Linux (e.g. SDL-BGI / libgraph):**

```bash
# Install graphics lib (e.g. libgraph on some distros)
# Then compile and run:
g++ singleOrbit.cpp -o singleOrbit -lgraph
./singleOrbit
```

**Note:** Replace `YOUR_USERNAME` with your GitHub username. Only one of the two programs should be built and run at a time (each has its own `main()`).

## Project Structure

```
.
├── singleOrbit.cpp                  # Single satellite orbit demo
├── multi_satellite_orbit_system.cpp # Earth, Moon, and 3 satellites
└── README.md                        # This file
```

## License

This project is open source. Use and modify as needed for learning or further development.
