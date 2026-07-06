"""
MEMBER 5: Conductor dashboard, camera feed, metrics, and game-over screen.
"""

from member1_setup_config import (
    BASE_H, ROAD_X, ROAD_RIGHT, RIGHT_PANEL, LEFT_PANEL, LANE_COUNT, C, lanes,
    MATATU_Y, DETECTION_DISTANCE, format_time,
    rounded_rect, circle, draw_text,
)
from member2_matatu_controls import state
from member3_obstacles_physics import detected_obstacles


def metric(surface, x, y, label, value, color):
    rounded_rect(surface, x, y, LEFT_PANEL - 48, 54, 8, C.panel2, C.line)
    draw_text(surface, label.upper(), x + 12, y + 8, 15, C.muted)
    draw_text(surface, value, x + 12, y + 27, 21, color)


def draw_left_panel(surface):
    import pygame
    pygame.draw.rect(surface, C.panel, (0, 0, LEFT_PANEL, BASE_H))
    pygame.draw.rect(surface, C.line, (LEFT_PANEL - 2, 0, 2, BASE_H))

    draw_text(surface, "CONTROL PANEL", 22, 24, 28, C.white, "700")
    draw_text(surface, "CONDUCTOR MODE", 22, 58, 18, C.muted)
    metric(surface, 24, 112, "Speed", f"{state.speed:05.1f} px/s", C.red if state.braking else C.green)
    metric(surface, 24, 186, "Distance", f"{state.distance:06.1f} m", C.cyan)
    metric(surface, 24, 260, "Time", format_time(state.elapsed), C.amber)
    metric(surface, 24, 334, "Lane", lanes[state.lane], C.purple)

    draw_text(surface, "Controls", 24, 438, 21, C.white)
    controls = [
        ("SPACE", "Brake / stop"),
        ("A / LEFT", "Change left"),
        ("D / RIGHT", "Change right"),
        ("Q / ESC", "End session"),
    ]
    for i, (key, action) in enumerate(controls):
        y = 480 + i * 42
        rounded_rect(surface, 24, y, 92, 28, 5, (0x0e, 0x12, 0x18), C.line)
        draw_text(surface, key, 34, y + 7, 15, C.white)
        draw_text(surface, action, 130, y + 5, 18, C.muted)

    detected = len(detected_obstacles())
    rounded_rect(surface, 24, 690, LEFT_PANEL - 48, 40, 8, C.red if detected else C.green)
    draw_text(surface, "ALERT: OBSTACLE" if detected else "SYSTEM NORMAL", 38, 701, 18, C.black, "700")


def draw_right_panel(surface):
    import pygame
    pygame.draw.rect(surface, C.panel, (ROAD_RIGHT, 0, RIGHT_PANEL, BASE_H))
    pygame.draw.rect(surface, C.line, (ROAD_RIGHT, 0, 2, BASE_H))

    x = ROAD_RIGHT + 24
    draw_text(surface, "CAMERA FEED", x, 24, 28, C.white, "700")
    draw_text(surface, "FORWARD OBSTACLE VIEW", x, 58, 18, C.muted)
    draw_camera_preview(surface, x, 102)
    draw_detection_list(surface, x, 382)
    draw_stats(surface, x, 642)


def draw_camera_preview(surface, x, y):
    import pygame
    rounded_rect(surface, x, y, RIGHT_PANEL - 48, 230, 8, (0x0b, 0x0f, 0x14), C.line)
    inner_x, inner_y = x + 14, y + 12
    inner_w, inner_h = RIGHT_PANEL - 76, 206
    rounded_rect(surface, inner_x, inner_y, inner_w, inner_h, 4, (0x1e, 0x22, 0x27))

    pygame.draw.rect(surface, (0xe1, 0xbe, 0x38), (int(inner_x + inner_w / 2 - 1), inner_y, 2, inner_h))
    pygame.draw.rect(surface, C.white, (inner_x + 8, inner_y, 2, inner_h))
    pygame.draw.rect(surface, C.white, (int(inner_x + inner_w - 10), inner_y, 2, inner_h))

    for obstacle in detected_obstacles():
        distance = max(1, MATATU_Y - (obstacle["y"] + obstacle["h"]))
        normalized = max(0, min(1, distance / DETECTION_DISTANCE))
        marker_y = inner_y + inner_h - normalized * inner_h

        if obstacle["kind"] == "VEHICLE":
            lane_x = inner_x + (obstacle["lane"] + 0.5) * inner_w / LANE_COUNT
            rounded_rect(surface, lane_x - 19, marker_y - 13, 38, 26, 5, C.red)
        else:
            rounded_rect(surface, inner_x + 14, marker_y - 10, inner_w - 28, 20, 3,
                         C.green if obstacle["cleared"] else C.white)

    circle(surface, x + RIGHT_PANEL - 66, y + 18, 6, C.red if state.alert_flash > 0 else C.green)
    draw_text(surface, "LIVE", x + RIGHT_PANEL - 114, y + 11, 15, C.red if state.alert_flash > 0 else C.green)


def draw_detection_list(surface, x, y):
    detected = detected_obstacles()
    draw_text(surface, "Detected Obstacles", x, y, 21, C.white)
    draw_text(surface, f"{len(detected)} in camera range", x, y + 30, 18, C.muted)

    if not detected:
        rounded_rect(surface, x, y + 70, RIGHT_PANEL - 48, 58, 8, C.panel2)
        draw_text(surface, "Road clear", x + 16, y + 89, 18, C.green)
        return

    for i, obstacle in enumerate(detected):
        row_y = y + 66 + i * 48
        rounded_rect(surface, x, row_y, RIGHT_PANEL - 48, 38, 7, C.panel2, C.red if i == 0 else C.line)
        distance = max(0, MATATU_Y - (obstacle["y"] + obstacle["h"]))
        lane = "ALL" if obstacle["lane"] is None else lanes[obstacle["lane"]]
        status = "  OK" if obstacle["cleared"] else f"{distance:4.0f}px"
        draw_text(surface, f"{obstacle['kind']:<8} {lane:<6} {status}", x + 10, row_y + 11, 15, C.white)


def draw_stats(surface, x, y):
    draw_text(surface, "Session Metrics", x, y, 21, C.white)
    rows = [
        ("Avoided", str(state.avoided)),
        ("Final Speed", f"{state.speed:.1f}"),
        ("Mode", "BRAKING" if state.braking else "AUTO"),
    ]
    for i, (label, value) in enumerate(rows):
        yy = y + 42 + i * 34
        draw_text(surface, label, x, yy, 18, C.muted)
        draw_text(surface, value, x + 170, yy, 18, C.white)


def draw_game_over(surface):
    import pygame
    from member1_setup_config import BASE_W

    overlay = pygame.Surface((BASE_W, BASE_H), pygame.SRCALPHA)
    overlay.fill((0, 0, 0, 178))
    surface.blit(overlay, (0, 0))

    box_x, box_y, box_w, box_h = 320, 235, 560, 330
    rounded_rect(surface, box_x, box_y, box_w, box_h, 12, (0x18, 0x1e, 0x26), C.red, 3)
    draw_text(surface, "COLLISION DETECTED", box_x + box_w / 2, box_y + 36, 44, C.red, "700", "center")

    obj = state.collision["kind"] if state.collision else "UNKNOWN"
    lines = [
        f"Impact object: {obj}",
        f"Distance traveled: {state.distance:.1f} m",
        f"Time survived: {format_time(state.elapsed)}",
        f"Obstacles avoided: {state.avoided}",
        f"Final speed: {state.speed:.1f} px/s",
    ]
    for i, line in enumerate(lines):
        draw_text(surface, line, box_x + 74, box_y + 112 + i * 34, 21, C.white)

    draw_text(surface, "Press SPACE to restart", box_x + box_w / 2, box_y + box_h - 52, 21, C.amber, "400", "center")
