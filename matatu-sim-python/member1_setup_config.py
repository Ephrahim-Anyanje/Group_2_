"""
MEMBER 1: Project setup, window, constants, colors, and shared drawing helpers.
"""

import pygame

pygame.init()
pygame.display.set_caption("Autonomous Matatu Conductor Monitoring System")

BASE_W = 1200
BASE_H = 800
LEFT_PANEL = 260
RIGHT_PANEL = 300
ROAD_X = LEFT_PANEL
ROAD_W = BASE_W - LEFT_PANEL - RIGHT_PANEL
ROAD_RIGHT = ROAD_X + ROAD_W
LANE_COUNT = 3
LANE_W = ROAD_W / LANE_COUNT

MATATU_W = 86
MATATU_H = 122
MATATU_Y = BASE_H - 180

BASE_SPEED = 200
BRAKE_DECEL = 300
RECOVERY_ACCEL = 70
SPAWN_INTERVAL = 2.0
DETECTION_DISTANCE = 300
LANE_CHANGE_COOLDOWN = 120  # milliseconds
CROSSING_CLEAR_SPEED = 12
CROSSING_CLEAR_DISTANCE = 90

# Windowed, resizable, scaled to fit like the canvas version.
screen = pygame.display.set_mode((1000, 700), pygame.RESIZABLE)
clock = pygame.time.Clock()


def _c(hex_str):
    hex_str = hex_str.lstrip("#")
    return tuple(int(hex_str[i:i + 2], 16) for i in (0, 2, 4))


class C:
    bg = _c("#080a0d")
    road = _c("#1c1f23")
    roadShadow = _c("#121417")
    panel = _c("#161c24")
    panel2 = _c("#1c242e")
    line = _c("#3e4c5c")
    white = _c("#f0f4f8")
    muted = _c("#9cabb8")
    yellow = _c("#ffc72c")
    matatuYellow = _c("#fad63b")
    matatuBlue = _c("#2d7ddd")
    cyan = _c("#58dceb")
    green = _c("#36d377")
    red = _c("#ef4444")
    amber = _c("#ffab2e")
    purple = _c("#9c76ff")
    black = _c("#080a0d")


lanes = ["LEFT", "CENTER", "RIGHT"]

# Font cache so we are not rebuilding fonts every frame.
_font_cache = {}


def _font(size, weight):
    key = (size, weight)
    if key not in _font_cache:
        bold = weight in ("700", "bold", "600")
        try:
            _font_cache[key] = pygame.font.SysFont("consolas", size, bold=bold)
        except Exception:
            _font_cache[key] = pygame.font.Font(None, size)
    return _font_cache[key]


def rounded_rect(surface, x, y, w, h, r, fill=None, stroke=None, line_width=1):
    rect = pygame.Rect(int(x), int(y), int(w), int(h))
    r = int(max(0, min(r, min(w, h) / 2)))
    if fill:
        pygame.draw.rect(surface, fill, rect, border_radius=r)
    if stroke:
        pygame.draw.rect(surface, stroke, rect, width=line_width, border_radius=r)


def circle(surface, x, y, r, fill):
    pygame.draw.circle(surface, fill, (int(x), int(y)), int(r))


def draw_text(surface, value, x, y, size=18, color=C.white, weight="400", align="left"):
    font = _font(size, weight)
    img = font.render(str(value), True, color)
    rect = img.get_rect()
    if align == "left":
        rect.topleft = (x, y)
    elif align == "center":
        rect.midtop = (x, y)
    elif align == "right":
        rect.topright = (x, y)
    surface.blit(img, rect)


def format_time(seconds):
    minutes = int(seconds // 60)
    secs = int(seconds % 60)
    return f"{minutes:02d}:{secs:02d}"
