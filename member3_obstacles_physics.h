
#pragma once

#include "member2_matatu_controls.h"
#include <vector>

void spawnObstacle();
void updateSimulation(float dt);
void updateDetection(Obstacle& obstacle);
void checkCollision();


std::vector<Obstacle*> detectedObstacles();
