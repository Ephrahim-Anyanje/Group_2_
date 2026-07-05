#!/usr/bin/env bash
# Build the C++/SFML port of the Autonomous Matatu simulation.
# Requires: g++ with C++17 support, libsfml-dev (SFML 2.5+)
#
# Ubuntu/Debian:
#   sudo apt-get install libsfml-dev
#
set -e
g++ -std=c++17 -O2 \
    member1_setup_config.cpp \
    member2_matatu_controls.cpp \
    member3_obstacles_physics.cpp \
    member4_road_rendering.cpp \
    member5_dashboard_camera.cpp \
    main.cpp \
    -o matatu_sim \
    -lsfml-graphics -lsfml-window -lsfml-system

echo "Built ./matatu_sim"
