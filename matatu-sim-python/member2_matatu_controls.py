

import time
import pygame

from member1_setup_config import (
    ROAD_X, LANE_W, LANE_COUNT, MATATU_W, MATATU_H, MATATU_Y,
    BASE_SPEED, LANE_CHANGE_COOLDOWN,
)

keys_down = set()


def lane_center(lane):
    return ROAD_X + lane * LANE_W + LANE_W / 2


def _now_ms():
    return time.perf_counter() * 1000


class State:
    def __init__(self):
        self.reset()

    def reset(self):
        self.lane = 1
        self.x = lane_center(1) - MATATU_W / 2
        self.target_x = self.x
        self.speed = BASE_SPEED
        self.braking = False
        self.last_lane_change = 0
        self.obstacles = []
        self.spawn_timer = 0
        self.road_offset = 0
        self.distance = 0
        self.elapsed = 0
        self.avoided = 0
        self.game_over = False
        self.collision = None
        self.alert_flash = 0


state = State()


def matatu_rect():
    return pygame.Rect(int(state.x), int(MATATU_Y), int(MATATU_W), int(MATATU_H))


def reset_simulation():
    state.reset()


def change_lane(direction):
    now = _now_ms()
    if now - state.last_lane_change < LANE_CHANGE_COOLDOWN:
        return

    next_lane = max(0, min(LANE_COUNT - 1, state.lane + direction))
    if next_lane == state.lane:
        return

    state.lane = next_lane
    state.target_x = lane_center(next_lane) - MATATU_W / 2
    state.last_lane_change = now


def handle_keydown(event):
    key = pygame.key.name(event.key).lower()

    keys_down.add(key)

    if key in ("q", "escape"):
        state.game_over = True
        state.speed = 0
        state.collision = {"kind": "SESSION ENDED"}
    elif state.game_over and key == "space":
        reset_simulation()
    elif not state.game_over and key in ("a", "left"):
        change_lane(-1)
    elif not state.game_over and key in ("d", "right"):
        change_lane(1)


def handle_keyup(event):
    key = pygame.key.name(event.key).lower()
    keys_down.discard(key)
