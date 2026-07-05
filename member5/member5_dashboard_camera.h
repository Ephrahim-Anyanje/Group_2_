// MEMBER 5: Conductor dashboard, camera feed, metrics, and game-over screen.
#pragma once

#include "member3_obstacles_physics.h"

void metricBox(sf::RenderTarget& target, float x, float y, const std::string& label,
               const std::string& value, sf::Color color);

void drawLeftPanel(sf::RenderTarget& target);
void drawRightPanel(sf::RenderTarget& target);
void drawCameraPreview(sf::RenderTarget& target, float x, float y);
void drawDetectionList(sf::RenderTarget& target, float x, float y);
void drawStats(sf::RenderTarget& target, float x, float y);
void drawGameOver(sf::RenderTarget& target);
