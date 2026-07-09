
#include "member3_obstacles_physics.h"
#include <algorithm>
#include <cstdlib>
#include <cmath>

static sf::Color randomVehicleColor() {
    static const sf::Color colors[] = {
        C::fromHex(0xd53838), C::fromHex(0xde4d3e), C::fromHex(0xbc2f4b), C::fromHex(0xe65b2d)
    };
    return colors[std::rand() % 4];
}

static float randf() {
    return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
}

void spawnObstacle() {
    if (randf() < 0.52f) {
        int lane = std::rand() % LANE_COUNT;
        float w = 82.f, h = 62.f;

        Obstacle obstacle;
        obstacle.kind = "VEHICLE";
        obstacle.lane = lane;
        obstacle.x = laneCenter(lane) - w / 2.f;
        obstacle.y = -h - 20.f;
        obstacle.w = w;
        obstacle.h = h;
        obstacle.color = randomVehicleColor();
        state.obstacles.push_back(obstacle);
    } else {
        Obstacle obstacle;
        obstacle.kind = "CROSSING";
        obstacle.lane = -1;
        obstacle.x = ROAD_X + 18.f;
        obstacle.y = -94.f;
        obstacle.w = ROAD_W - 36.f;
        obstacle.h = 74.f;
        obstacle.color = C::white;
        state.obstacles.push_back(obstacle);
    }
}

void updateDetection(Obstacle& obstacle) {
    float distance = MATATU_Y - (obstacle.y + obstacle.h);

    if (distance >= 0.f && distance <= DETECTION_DISTANCE) {
        obstacle.detected = true;
        state.alertFlash = 0.2f;
    }

    if (obstacle.kind == "CROSSING" &&
        obstacle.detected &&
        !obstacle.cleared &&
        distance >= -obstacle.h &&
        distance <= CROSSING_CLEAR_DISTANCE &&
        state.speed <= CROSSING_CLEAR_SPEED) {
        obstacle.cleared = true;
    }
}

static bool intersects(const sf::FloatRect& a, const sf::FloatRect& b) {
    return a.intersects(b);
}

void checkCollision() {
    sf::FloatRect matatu = matatuRect();

    for (auto& obstacle : state.obstacles) {
        sf::FloatRect obstacleRect(obstacle.x, obstacle.y, obstacle.w, obstacle.h);
        if (!intersects(matatu, obstacleRect)) continue;

        if (obstacle.kind == "CROSSING") {
            if (obstacle.cleared) continue;
            if (state.speed <= CROSSING_CLEAR_SPEED) {
                obstacle.cleared = true;
                continue;
            }
        }

        state.gameOver = true;
        state.collision = Collision{ obstacle.kind };
        state.braking = false;
        return;
    }
}

void updateSimulation(float dt) {
    if (state.gameOver) return;

    state.braking = keysDown.count("space") > 0;
    state.elapsed += dt;

    if (state.braking) {
        state.speed = std::max(0.f, state.speed - BRAKE_DECEL * dt);
    } else {
        state.speed = std::min(BASE_SPEED, state.speed + RECOVERY_ACCEL * dt);
    }

    state.x += (state.targetX - state.x) * std::min(1.f, dt * 12.f);
    state.distance += state.speed * dt / 12.f;
    state.roadOffset = std::fmod(state.roadOffset + state.speed * dt, 80.f);

    state.spawnTimer += dt;
    if (state.spawnTimer >= SPAWN_INTERVAL) {
        state.spawnTimer = 0.f;
        spawnObstacle();
    }

    for (auto& obstacle : state.obstacles) {
        obstacle.y += state.speed * dt;
        updateDetection(obstacle);

        if (!obstacle.passed && obstacle.y > BASE_H + 10.f) {
            obstacle.passed = true;
            state.avoided += 1;
        }
    }

    state.alertFlash = std::max(0.f, state.alertFlash - dt);

    state.obstacles.erase(
        std::remove_if(state.obstacles.begin(), state.obstacles.end(),
                        [](const Obstacle& o) { return o.y >= BASE_H + 160.f; }),
        state.obstacles.end());

    checkCollision();
}

std::vector<Obstacle*> detectedObstacles() {
    std::vector<Obstacle*> result;
    for (auto& obstacle : state.obstacles) {
        if (obstacle.detected && obstacle.y + obstacle.h < MATATU_Y + 140.f) {
            result.push_back(&obstacle);
        }
    }
    std::sort(result.begin(), result.end(), [](Obstacle* a, Obstacle* b) {
        return std::abs(MATATU_Y - (a->y + a->h)) < std::abs(MATATU_Y - (b->y + b->h));
    });
    if (result.size() > 5) result.resize(5);
    return result;
}
