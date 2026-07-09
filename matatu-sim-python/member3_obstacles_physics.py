

import random
import pygame

from member1_setup_config import (
    BASE_H, ROAD_X, ROAD_W, LANE_COUNT, C,
    BASE_SPEED, BRAKE_DECEL, RECOVERY_ACCEL, SPAWN_INTERVAL,
    DETECTION_DISTANCE, CROSSING_CLEAR_SPEED, CROSSING_CLEAR_DISTANCE,
)
from member2_matatu_controls import state, keys_down, lane_center, matatu_rect

VEHICLE_COLORS = ["#d53838", "#de4d3e", "#bc2f4b", "#e65b2d"]


def _c(hex_str):
    hex_str = hex_str.lstrip("#")
    return tuple(int(hex_str[i:i + 2], 16) for i in (0, 2, 4))


def spawn_obstacle():
    if random.random() < 0.52:
        lane = random.randint(0, LANE_COUNT - 1)
        w, h = 82, 62
        state.obstacles.append({
            "kind": "VEHICLE",
            "lane": lane,
            "x": lane_center(lane) - w / 2,
            "y": -h - 20,
            "w": w,
            "h": h,
            "color": _c(random.choice(VEHICLE_COLORS)),
            "detected": False,
            "passed": False,
            "cleared": False,
        })
    else:
        state.obstacles.append({
            "kind": "CROSSING",
            "lane": None,
            "x": ROAD_X + 18,
            "y": -94,
            "w": ROAD_W - 36,
            "h": 74,
            "color": C.white,
            "detected": False,
            "passed": False,
            "cleared": False,
        })


def update_simulation(dt):
    if state.game_over:
        return

    state.braking = "space" in keys_down
    state.elapsed += dt

    if state.braking:
        state.speed = max(0, state.speed - BRAKE_DECEL * dt)
    else:
        state.speed = min(BASE_SPEED, state.speed + RECOVERY_ACCEL * dt)

    state.x += (state.target_x - state.x) * min(1, dt * 12)
    state.distance += state.speed * dt / 12
    state.road_offset = (state.road_offset + state.speed * dt) % 80

    state.spawn_timer += dt
    if state.spawn_timer >= SPAWN_INTERVAL:
        state.spawn_timer = 0
        spawn_obstacle()

    for obstacle in state.obstacles:
        obstacle["y"] += state.speed * dt
        update_detection(obstacle)

        if not obstacle["passed"] and obstacle["y"] > BASE_H + 10:
            obstacle["passed"] = True
            state.avoided += 1

    state.alert_flash = max(0, state.alert_flash - dt)
    state.obstacles = [o for o in state.obstacles if o["y"] < BASE_H + 160]
    check_collision()


def update_detection(obstacle):
    from member1_setup_config import MATATU_Y
    distance = MATATU_Y - (obstacle["y"] + obstacle["h"])

    if 0 <= distance <= DETECTION_DISTANCE:
        obstacle["detected"] = True
        state.alert_flash = 0.2

    if (
        obstacle["kind"] == "CROSSING"
        and obstacle["detected"]
        and not obstacle["cleared"]
        and -obstacle["h"] <= distance <= CROSSING_CLEAR_DISTANCE
        and state.speed <= CROSSING_CLEAR_SPEED
    ):
        obstacle["cleared"] = True


def _rect_of(obstacle):
    return pygame.Rect(int(obstacle["x"]), int(obstacle["y"]), int(obstacle["w"]), int(obstacle["h"]))


def check_collision():
    matatu = matatu_rect()

    for obstacle in state.obstacles:
        if not matatu.colliderect(_rect_of(obstacle)):
            continue

        if obstacle["kind"] == "CROSSING":
            if obstacle["cleared"]:
                continue
            if state.speed <= CROSSING_CLEAR_SPEED:
                obstacle["cleared"] = True
                continue

        state.game_over = True
        state.collision = obstacle
        state.braking = False
        return


def detected_obstacles():
    from member1_setup_config import MATATU_Y
    result = [
        o for o in state.obstacles
        if o["detected"] and o["y"] + o["h"] < MATATU_Y + 140
    ]
    result.sort(key=lambda o: abs(MATATU_Y - (o["y"] + o["h"])))
    return result[:5]
