# Autonomous Matatu Simulation: Python and C++ Ports

Same five-member split as the original JS version, rebuilt in Python
(Pygame) and C++ (SFML). Each member's file keeps the same name and the
same responsibility as before, just in a new language.

## Folders

```text
matatu-sim/member1/   Files to send to Member 1 (both languages)
matatu-sim/member2/   Files to send to Member 2 (both languages)
matatu-sim/member3/   Files to send to Member 3 (both languages)
matatu-sim/member4/   Files to send to Member 4 (both languages)
matatu-sim/member5/   Files to send to Member 5 (both languages)

matatu-sim-python/    Full integrated Pygame project (all files together, runnable)
matatu-sim-cpp/       Full integrated SFML project (all files together, buildable)
```

Use the `member1`–`member5` folders to hand out work to each person. Once
everyone's files are in, drop them all into one folder together (as in
`matatu-sim-python/` and `matatu-sim-cpp/`) to actually run or build the
project — that's the equivalent of assembling all the `<script>` tags
back into one working `index.html`.

`build.sh` in `member1/` assumes all five `.cpp` files sit alongside it
in the same folder, same as in `matatu-sim-cpp/` — it won't find the
other members' files if `member1/` is still standing alone.

## Send These Files To Each Member

### Member 1 — Setup / Config
- Python: `member1_setup_config.py`
- C++: `member1_setup_config.h`, `member1_setup_config.cpp`

Responsibility: window/canvas setup, shared constants, colors, shared
drawing helpers (rounded rects, circles, text, time formatting).

### Member 2 — Matatu Controls
- Python: `member2_matatu_controls.py`
- C++: `member2_matatu_controls.h`, `member2_matatu_controls.cpp`

Responsibility: matatu state, keyboard input, lane changing, braking,
restart/end-session behavior.

### Member 3 — Obstacles & Physics
- Python: `member3_obstacles_physics.py`
- C++: `member3_obstacles_physics.h`, `member3_obstacles_physics.cpp`

Responsibility: obstacle spawning, vehicle/zebra-crossing logic, the
per-frame physics update, collision detection, camera detection state.

### Member 4 — Road Rendering
- Python: `member4_road_rendering.py`
- C++: `member4_road_rendering.h`, `member4_road_rendering.cpp`

Responsibility: road rendering, lane markings, matatu drawing, obstacle
drawing, zebra crossing visuals, and the overall scene composition.

### Member 5 — Dashboard & Camera
- Python: `member5_dashboard_camera.py`
- C++: `member5_dashboard_camera.h`, `member5_dashboard_camera.cpp`

Responsibility: control panel, camera feed panel, detected-obstacle list,
session metrics, collision/game-over screen.

## How Everything Is Wired Together

The JS version used `<script>` tags in `index.html` to load files in
order and share globals between them. The ports keep the same idea:

- **Python**: `main.py` imports the five member modules in order
  (`member1` → `member5`) and runs the loop. This is the equivalent of
  `index.html`.
- **C++**: `main.cpp` includes the five member headers in order and runs
  the loop. `build.sh` compiles all five `.cpp` files together — this is
  the equivalent of `index.html`.

## Running It

### Python (Pygame)

```bash
pip install pygame
cd matatu-sim-python
python main.py
```

### C++ (SFML)

Requires SFML 2.5+ (`libsfml-dev` on Ubuntu/Debian):

```bash
# Ubuntu/Debian
sudo apt-get install libsfml-dev

cd matatu-sim-cpp
./build.sh
./matatu_sim
```

Both have been built and smoke-tested in this environment (Pygame ran
300 headless frames without error; the SFML binary compiled cleanly and
ran its render loop under Xvfb without crashing).

## Controls (same in both versions)

| Key | Action |
|-----|--------|
| SPACE | Brake / stop, or restart after a collision |
| A / Left arrow | Change lane left |
| D / Right arrow | Change lane right |
| Q / Escape | End session |

## Git Branch Names

Same convention as before, just suffixed by language:

```text
feature/member-1-setup-config-python
feature/member-1-setup-config-cpp
feature/member-2-matatu-controls-python
feature/member-2-matatu-controls-cpp
feature/member-3-obstacles-physics-python
feature/member-3-obstacles-physics-cpp
feature/member-4-road-rendering-python
feature/member-4-road-rendering-cpp
feature/member-5-dashboard-camera-python
feature/member-5-dashboard-camera-cpp
```

## Important

Keep the same file names across both languages. If a member renames
their file, `main.py` / `main.cpp` (and `build.sh` for C++) must be
updated to match.

## Notes on Behavior Parity

- All physics constants (speed, braking deceleration, spawn interval,
  detection distance, etc.) are identical to the JS version.
- Both ports draw to a fixed 1200×800 "base canvas" and then scale +
  letterbox it into the real window, matching the JS canvas's
  scale/translate logic — resizing the window keeps the whole scene
  visible and correctly proportioned.
- Fonts: the JS version used Consolas; since that's a Windows-only font,
  both ports fall back to a monospace font available on the running
  system (DejaVu Sans Mono / Liberation Mono on Linux). Swap in a
  different monospace font if you want to match Consolas more closely
  on Windows.
