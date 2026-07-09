

#include "member1_setup_config.h"
#include "member2_matatu_controls.h"
#include "member3_obstacles_physics.h"
#include "member4_road_rendering.h"
#include "member5_dashboard_camera.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    initWindow();
    resetSimulation();

    sf::Clock frameClock;
    sf::RenderWindow& window = gameWindow();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::KeyPressed) {
                handleKeyPressed(event.key.code);
            } else if (event.type == sf::Event::KeyReleased) {
                handleKeyReleased(event.key.code);
            }
        }

        float dt = std::min(0.05f, frameClock.restart().asSeconds());
        updateSimulation(dt);
        drawSimulation();
        window.display();
    }

    return 0;
}
