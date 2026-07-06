"""
Entry point for the Python port of the Autonomous Matatu Conductor
Monitoring System. This plays the same role as index.html in the JS
version: it loads each member's module in the required order and
drives the main loop.

Run with:
    python main.py
"""

import sys
import pygame

# Load order matters, same as <script> tags in index.html.
import member1_setup_config as m1
import member2_matatu_controls as m2
import member3_obstacles_physics as m3
import member4_road_rendering as m4
import member5_dashboard_camera as m5  # noqa: F401  (registers panel/game-over drawers)


def main():
    screen = m1.screen
    clock = m1.clock

    running = True
    while running:
        dt = min(0.05, clock.tick(60) / 1000)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.VIDEORESIZE:
                m1.screen = pygame.display.set_mode((event.w, event.h), pygame.RESIZABLE)
                screen = m1.screen
            elif event.type == pygame.KEYDOWN:
                m2.handle_keydown(event)
            elif event.type == pygame.KEYUP:
                m2.handle_keyup(event)

        m3.update_simulation(dt)
        m4.draw_simulation(screen)
        pygame.display.flip()

    pygame.quit()
    sys.exit()


if __name__ == "__main__":
    main()
