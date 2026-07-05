// MEMBER 3: Obstacle generation, physics update, camera detection, and collision logic.
#pragma once

#include "member2_matatu_controls.h"
#include <vector>

void spawnObstacle();
void updateSimulation(float dt);
void updateDetection(Obstacle& obstacle);
void checkCollision();

// Up to 5 nearest detected obstacles still in camera range, nearest first
// (mirrors detectedObstacles() in the JS version).
std::vector<Obstacle*> detectedObstacles();
