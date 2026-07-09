
#include "member4_road_rendering.h"
#include "member5_dashboard_camera.h"
#include <algorithm>

void drawRoad(sf::RenderTarget& target) {
    sf::RectangleShape shadow({ ROAD_W + 20.f, BASE_H });
    shadow.setPosition(ROAD_X - 10.f, 0.f);
    shadow.setFillColor(C::roadShadow);
    target.draw(shadow);

    sf::RectangleShape road({ ROAD_W, BASE_H });
    road.setPosition(ROAD_X, 0.f);
    road.setFillColor(C::road);
    target.draw(road);

    sf::RectangleShape leftEdge({ 5.f, BASE_H });
    leftEdge.setPosition(ROAD_X + 12.f, 0.f);
    leftEdge.setFillColor(C::white);
    target.draw(leftEdge);

    sf::RectangleShape rightEdge({ 5.f, BASE_H });
    rightEdge.setPosition(ROAD_RIGHT - 16.f, 0.f);
    rightEdge.setFillColor(C::white);
    target.draw(rightEdge);

    for (int laneLine = 1; laneLine < LANE_COUNT; ++laneLine) {
        float x = ROAD_X + laneLine * LANE_W - 3.f;
        for (float y = -80.f + state.roadOffset; y < BASE_H; y += 80.f) {
            roundedRectFill(target, x, y, 6.f, 44.f, 3.f, C::yellow);
        }
    }

    roundedRectFill(target, ROAD_X + 30.f, BASE_H - 34.f, ROAD_W - 60.f, 26.f, 13.f, C::panelDark);
    drawText(target, "AUTONOMOUS LANE CAMERA SIMULATION", ROAD_X + ROAD_W / 2.f, BASE_H - 27.f,
              15, C::muted, false, Align::Center);
}

void drawObstacle(sf::RenderTarget& target, const Obstacle& obstacle) {
    float x = obstacle.x, y = obstacle.y, w = obstacle.w, h = obstacle.h;

    if (obstacle.kind == "VEHICLE") {
        roundedRect(target, x, y, w, h, 7.f, &obstacle.color, &C::vehicleTrim, 2.f);
        roundedRectFill(target, x + 12.f, y + 9.f, w - 24.f, 17.f, 3.f, C::glassBlue);
        roundedRectFill(target, x + 15.f, y + 36.f, 18.f, 12.f, 2.f, C::headlight);
        roundedRectFill(target, x + w - 33.f, y + 36.f, 18.f, 12.f, 2.f, C::headlight);
        drawCircle(target, x + 13.f, y + h - 10.f, 8.f, C::black);
        drawCircle(target, x + w - 13.f, y + h - 10.f, 8.f, C::black);
        if (obstacle.detected) {
            roundedRectOutline(target, x - 5.f, y - 5.f, w + 10.f, h + 10.f, 8.f, C::red, 3.f);
        }
        return;
    }

    roundedRectFill(target, x, y, w, h, 2.f, C::asphaltDark);
    for (float stripeX = x + 12.f; stripeX < x + w - 12.f; stripeX += 44.f) {
        roundedRectFill(target, stripeX, y + 9.f, 28.f, h - 18.f, 3.f, C::white);
    }
    roundedRectFill(target, x + w / 2.f - 82.f, y + h / 2.f - 14.f, 164.f, 28.f, 4.f, C::white);
    drawText(target, obstacle.cleared ? "CLEARED" : "ZEBRA CROSSING", x + w / 2.f, y + h / 2.f - 8.f,
              15, C::black, true, Align::Center);

    if (obstacle.cleared) {
        roundedRectOutline(target, x - 1.f, y - 3.f, w + 2.f, h + 6.f, 4.f, C::green, 3.f);
    } else if (obstacle.detected) {
        roundedRectOutline(target, x - 1.f, y - 3.f, w + 2.f, h + 6.f, 4.f, C::red, 3.f);
    }
}

void drawMatatu(sf::RenderTarget& target) {
    sf::FloatRect matatu = matatuRect();
    float mx = matatu.left, my = matatu.top;

    roundedRectFill(target, mx + 8.f, my + 10.f, MATATU_W, MATATU_H, 12.f, sf::Color(0, 0, 0, 153));
    roundedRect(target, mx, my, MATATU_W, MATATU_H, 12.f, &C::matatuYellow, &C::matatuTrim, 3.f);
    roundedRectFill(target, mx + 10.f, my + 14.f, MATATU_W - 20.f, 38.f, 8.f, C::matatuBlue);
    roundedRectFill(target, mx + 17.f, my + 20.f, MATATU_W - 34.f, 26.f, 5.f, C::cyan);

    for (int i = 0; i < 3; ++i) {
        roundedRectFill(target, mx + 12.f + i * 22.f, my + 62.f, 16.f, 18.f, 3.f, C::windowTint);
    }

    roundedRectFill(target, mx + 16.f, my + MATATU_H - 28.f, 18.f, 10.f, 3.f, C::lampYellow);
    roundedRectFill(target, mx + MATATU_W - 34.f, my + MATATU_H - 28.f, 18.f, 10.f, 3.f, C::lampYellow);
    drawCircle(target, mx + 14.f, my + 34.f, 8.f, C::black);
    drawCircle(target, mx + MATATU_W - 14.f, my + 34.f, 8.f, C::black);
    drawCircle(target, mx + 14.f, my + MATATU_H - 24.f, 8.f, C::black);
    drawCircle(target, mx + MATATU_W - 14.f, my + MATATU_H - 24.f, 8.f, C::black);

    roundedRectFill(target, mx, my - 16.f, MATATU_W, 8.f, 4.f, state.braking ? C::red : C::green);
    drawText(target, "ISUZU PSV", mx + MATATU_W / 2.f, my + 94.f, 15, C::black, true, Align::Center);
}

void drawSimulation() {
    sf::RenderTexture& canvas = baseCanvas();
    canvas.clear(C::bg);

    drawRoad(canvas);
    for (const auto& obstacle : state.obstacles) drawObstacle(canvas, obstacle);
    drawMatatu(canvas);
    drawLeftPanel(canvas);
    drawRightPanel(canvas);
    if (state.gameOver) drawGameOver(canvas);

    canvas.display();

    sf::RenderWindow& window = gameWindow();
    sf::Vector2u winSize = window.getSize();
    float scale = std::min(winSize.x / BASE_W, winSize.y / BASE_H);
    float scaledW = BASE_W * scale, scaledH = BASE_H * scale;
    float ox = (winSize.x - scaledW) / 2.f, oy = (winSize.y - scaledH) / 2.f;

    window.clear(C::bg);

    sf::Sprite sprite(canvas.getTexture());
    sprite.setScale(scale, scale);
    sprite.setPosition(ox, oy);
    window.draw(sprite);
}
