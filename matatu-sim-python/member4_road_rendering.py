

import pygame

from member1_setup_config import (
    BASE_W, BASE_H, ROAD_X, ROAD_W, ROAD_RIGHT, LANE_COUNT, LANE_W, C,
    MATATU_W, MATATU_H, MATATU_Y,
    rounded_rect, circle, draw_text,
)
from member2_matatu_controls import state, matatu_rect

# All drawing happens on this fixed-size "base" surface, which is then
# scaled and letterboxed onto the real window (mirrors the JS canvas scale/translate logic).
base_surface = pygame.Surface((BASE_W, BASE_H))


def draw_road(surface):
    pygame.draw.rect(surface, C.roadShadow, (ROAD_X - 10, 0, ROAD_W + 20, BASE_H))
    pygame.draw.rect(surface, C.road, (ROAD_X, 0, ROAD_W, BASE_H))

    pygame.draw.rect(surface, C.white, (ROAD_X + 12, 0, 5, BASE_H))
    pygame.draw.rect(surface, C.white, (ROAD_RIGHT - 16, 0, 5, BASE_H))

    for lane_line in range(1, LANE_COUNT):
        x = ROAD_X + lane_line * LANE_W - 3
        y = -80 + state.road_offset
        while y < BASE_H:
            rounded_rect(surface, x, y, 6, 44, 3, C.yellow)
            y += 80

    rounded_rect(surface, ROAD_X + 30, BASE_H - 34, ROAD_W - 60, 26, 13, (0x20, 0x25, 0x2b))
    draw_text(surface, "AUTONOMOUS LANE CAMERA SIMULATION", ROAD_X + ROAD_W / 2, BASE_H - 27, 15, C.muted, "400", "center")


def draw_obstacle(surface, obstacle):
    if obstacle["kind"] == "VEHICLE":
        x, y, w, h = obstacle["x"], obstacle["y"], obstacle["w"], obstacle["h"]
        rounded_rect(surface, x, y, w, h, 7, obstacle["color"], (0x82, 0x1e, 0x1e), 2)
        rounded_rect(surface, x + 12, y + 9, w - 24, 17, 3, (0x96, 0xdc, 0xeb))
        rounded_rect(surface, x + 15, y + 36, 18, 12, 2, (0xeb, 0xf5, 0xff))
        rounded_rect(surface, x + w - 33, y + 36, 18, 12, 2, (0xeb, 0xf5, 0xff))
        circle(surface, x + 13, y + h - 10, 8, C.black)
        circle(surface, x + w - 13, y + h - 10, 8, C.black)
        if obstacle["detected"]:
            rounded_rect(surface, x - 5, y - 5, w + 10, h + 10, 8, None, C.red, 3)
        return

    x, y, w, h = obstacle["x"], obstacle["y"], obstacle["w"], obstacle["h"]
    rounded_rect(surface, x, y, w, h, 2, (0x2a, 0x2d, 0x31))
    stripe_x = x + 12
    while stripe_x < x + w - 12:
        rounded_rect(surface, stripe_x, y + 9, 28, h - 18, 3, C.white)
        stripe_x += 44
    rounded_rect(surface, x + w / 2 - 82, y + h / 2 - 14, 164, 28, 4, C.white)
    label = "CLEARED" if obstacle["cleared"] else "ZEBRA CROSSING"
    draw_text(surface, label, x + w / 2, y + h / 2 - 8, 15, C.black, "700", "center")

    if obstacle["cleared"]:
        rounded_rect(surface, x - 1, y - 3, w + 2, h + 6, 4, None, C.green, 3)
    elif obstacle["detected"]:
        rounded_rect(surface, x - 1, y - 3, w + 2, h + 6, 4, None, C.red, 3)


def draw_matatu(surface):
    matatu = matatu_rect()
    mx, my = matatu.x, matatu.y
    rounded_rect(surface, mx + 8, my + 10, MATATU_W, MATATU_H, 12, (0, 0, 0))
    rounded_rect(surface, mx, my, MATATU_W, MATATU_H, 12, C.matatuYellow, (0x8c, 0x68, 0x0a), 3)
    rounded_rect(surface, mx + 10, my + 14, MATATU_W - 20, 38, 8, C.matatuBlue)
    rounded_rect(surface, mx + 17, my + 20, MATATU_W - 34, 26, 5, C.cyan)

    for i in range(3):
        rounded_rect(surface, mx + 12 + i * 22, my + 62, 16, 18, 3, (0xa6, 0xe5, 0xee))

    rounded_rect(surface, mx + 16, my + MATATU_H - 28, 18, 10, 3, (0xff, 0xee, 0x75))
    rounded_rect(surface, mx + MATATU_W - 34, my + MATATU_H - 28, 18, 10, 3, (0xff, 0xee, 0x75))
    circle(surface, mx + 14, my + 34, 8, C.black)
    circle(surface, mx + MATATU_W - 14, my + 34, 8, C.black)
    circle(surface, mx + 14, my + MATATU_H - 24, 8, C.black)
    circle(surface, mx + MATATU_W - 14, my + MATATU_H - 24, 8, C.black)

    rounded_rect(surface, mx, my - 16, MATATU_W, 8, 4, C.red if state.braking else C.green)
    draw_text(surface, "ISUZU PSV", mx + MATATU_W / 2, my + 94, 15, C.black, "700", "center")


def draw_simulation(screen):
    from member5_dashboard_camera import draw_left_panel, draw_right_panel, draw_game_over

    base_surface.fill(C.bg)

    draw_road(base_surface)
    for obstacle in state.obstacles:
        draw_obstacle(base_surface, obstacle)
    draw_matatu(base_surface)
    draw_left_panel(base_surface)
    draw_right_panel(base_surface)
    if state.game_over:
        draw_game_over(base_surface)

    win_w, win_h = screen.get_size()
    scale = min(win_w / BASE_W, win_h / BASE_H)
    scaled_w, scaled_h = int(BASE_W * scale), int(BASE_H * scale)
    ox, oy = (win_w - scaled_w) // 2, (win_h - scaled_h) // 2

    screen.fill(C.bg)
    scaled = pygame.transform.smoothscale(base_surface, (scaled_w, scaled_h))
    screen.blit(scaled, (ox, oy))
