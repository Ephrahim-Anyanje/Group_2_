
#pragma once

#include "member1_setup_config.h"
#include <vector>
#include <set>
#include <optional>
#include <string>

struct Obstacle {
    std::string kind;   // "VEHICLE" or "CROSSING"
    int lane = -1;       // -1 means "no single lane" (CROSSING spans all lanes)
    float x = 0, y = 0, w = 0, h = 0;
    sf::Color color;
    bool detected = false;
    bool passed = false;
    bool cleared = false;
};

struct Collision {
    std::string kind;
};

struct State {
    int lane = 1;
    float x = 0, targetX = 0;
    float speed = BASE_SPEED;
    bool braking = false;
    float lastLaneChange = 0.f; // milliseconds, using an internal clock
    std::vector<Obstacle> obstacles;
    float spawnTimer = 0.f;
    float roadOffset = 0.f;
    float distance = 0.f;
    float elapsed = 0.f;
    int avoided = 0;
    bool gameOver = false;
    std::optional<Collision> collision;
    float alertFlash = 0.f;
};

extern State state;
extern std::set<std::string> keysDown; // currently-held key names, e.g. "space", "a", "left"

float laneCenter(int lane);
sf::FloatRect matatuRect();
void resetSimulation();
void changeLane(int direction);


void handleKeyPressed(sf::Keyboard::Key key);
void handleKeyReleased(sf::Keyboard::Key key);
