// MEMBER 4: Road, matatu, obstacle, and scene rendering.
#pragma once

#include "member3_obstacles_physics.h"

void drawRoad(sf::RenderTarget& target);
void drawObstacle(sf::RenderTarget& target, const Obstacle& obstacle);
void drawMatatu(sf::RenderTarget& target);

// Renders the whole scene (road, obstacles, matatu, both panels, game-over
// overlay) onto the fixed-size base canvas, then scales/letterboxes that
// onto the real window. Equivalent to drawSimulation() in the JS version.
void drawSimulation();
