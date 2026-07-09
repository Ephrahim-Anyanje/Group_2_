
#include "member2_matatu_controls.h"
#include <algorithm>

State state;
std::set<std::string> keysDown;


static sf::Clock laneClock;

float laneCenter(int lane) {
    return ROAD_X + lane * LANE_W + LANE_W / 2.f;
}

sf::FloatRect matatuRect() {
    return sf::FloatRect(state.x, MATATU_Y, MATATU_W, MATATU_H);
}

void resetSimulation() {
    state.lane = 1;
    state.x = laneCenter(1) - MATATU_W / 2.f;
    state.targetX = state.x;
    state.speed = BASE_SPEED;
    state.braking = false;
    state.lastLaneChange = 0.f;
    state.obstacles.clear();
    state.spawnTimer = 0.f;
    state.roadOffset = 0.f;
    state.distance = 0.f;
    state.elapsed = 0.f;
    state.avoided = 0;
    state.gameOver = false;
    state.collision.reset();
    state.alertFlash = 0.f;
    laneClock.restart();
}

void changeLane(int direction) {
    float now = laneClock.getElapsedTime().asMilliseconds();
    if (now - state.lastLaneChange < LANE_CHANGE_COOLDOWN) return;

    int next = std::max(0, std::min(LANE_COUNT - 1, state.lane + direction));
    if (next == state.lane) return;

    state.lane = next;
    state.targetX = laneCenter(next) - MATATU_W / 2.f;
    state.lastLaneChange = now;
}

static std::string keyName(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::Space:  return "space";
        case sf::Keyboard::A:      return "a";
        case sf::Keyboard::D:      return "d";
        case sf::Keyboard::Q:      return "q";
        case sf::Keyboard::Left:   return "left";
        case sf::Keyboard::Right:  return "right";
        case sf::Keyboard::Escape: return "escape";
        default:                   return "";
    }
}

void handleKeyPressed(sf::Keyboard::Key key) {
    std::string name = keyName(key);
    if (name.empty()) return;

    keysDown.insert(name);

    if (name == "q" || name == "escape") {
        state.gameOver = true;
        state.speed = 0.f;
        state.collision = Collision{ "SESSION ENDED" };
    } else if (state.gameOver && name == "space") {
        resetSimulation();
    } else if (!state.gameOver && (name == "a" || name == "left")) {
        changeLane(-1);
    } else if (!state.gameOver && (name == "d" || name == "right")) {
        changeLane(1);
    }
}

void handleKeyReleased(sf::Keyboard::Key key) {
    std::string name = keyName(key);
    if (!name.empty()) keysDown.erase(name);
}
