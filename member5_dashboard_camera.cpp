// MEMBER 5: Conductor dashboard, camera feed, metrics, and game-over screen.
#include "member5_dashboard_camera.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

void metricBox(sf::RenderTarget& target, float x, float y, const std::string& label,
               const std::string& value, sf::Color color) {
    roundedRectFill(target, x, y, LEFT_PANEL - 48.f, 54.f, 8.f, C::panel2);
    roundedRectOutline(target, x, y, LEFT_PANEL - 48.f, 54.f, 8.f, C::line, 1.f);
    std::string upperLabel = label;
    std::transform(upperLabel.begin(), upperLabel.end(), upperLabel.begin(), ::toupper);
    drawText(target, upperLabel, x + 12.f, y + 8.f, 15, C::muted);
    drawText(target, value, x + 12.f, y + 27.f, 21, color);
}

void drawLeftPanel(sf::RenderTarget& target) {
    sf::RectangleShape panelBg({ LEFT_PANEL, BASE_H });
    panelBg.setFillColor(C::panel);
    target.draw(panelBg);

    sf::RectangleShape divider({ 2.f, BASE_H });
    divider.setPosition(LEFT_PANEL - 2.f, 0.f);
    divider.setFillColor(C::line);
    target.draw(divider);

    drawText(target, "CONTROL PANEL", 22.f, 24.f, 28, C::white, true);
    drawText(target, "CONDUCTOR MODE", 22.f, 58.f, 18, C::muted);

    std::ostringstream speedStr;
    speedStr << std::fixed << std::setprecision(1) << state.speed;
    metricBox(target, 24.f, 112.f, "Speed", padStart(speedStr.str(), 5) + " px/s",
              state.braking ? C::red : C::green);

    std::ostringstream distStr;
    distStr << std::fixed << std::setprecision(1) << state.distance;
    metricBox(target, 24.f, 186.f, "Distance", padStart(distStr.str(), 6) + " m", C::cyan);

    metricBox(target, 24.f, 260.f, "Time", formatTime(state.elapsed), C::amber);
    metricBox(target, 24.f, 334.f, "Lane", lanes[state.lane], C::purple);

    drawText(target, "Controls", 24.f, 438.f, 21, C::white);
    struct KeyRow { const char* key; const char* action; };
    KeyRow rows[] = {
        { "SPACE", "Brake / stop" },
        { "A / LEFT", "Change left" },
        { "D / RIGHT", "Change right" },
        { "Q / ESC", "End session" },
    };
    for (int i = 0; i < 4; ++i) {
        float y = 480.f + i * 42.f;
        roundedRectFill(target, 24.f, y, 92.f, 28.f, 5.f, C::keyBg);
        roundedRectOutline(target, 24.f, y, 92.f, 28.f, 5.f, C::line, 1.f);
        drawText(target, rows[i].key, 34.f, y + 7.f, 15, C::white);
        drawText(target, rows[i].action, 130.f, y + 5.f, 18, C::muted);
    }

    int detected = static_cast<int>(detectedObstacles().size());
    roundedRectFill(target, 24.f, 690.f, LEFT_PANEL - 48.f, 40.f, 8.f, detected ? C::red : C::green);
    drawText(target, detected ? "ALERT: OBSTACLE" : "SYSTEM NORMAL", 38.f, 701.f, 18, C::black, true);
}

void drawRightPanel(sf::RenderTarget& target) {
    sf::RectangleShape panelBg({ RIGHT_PANEL, BASE_H });
    panelBg.setPosition(ROAD_RIGHT, 0.f);
    panelBg.setFillColor(C::panel);
    target.draw(panelBg);

    sf::RectangleShape divider({ 2.f, BASE_H });
    divider.setPosition(ROAD_RIGHT, 0.f);
    divider.setFillColor(C::line);
    target.draw(divider);

    float x = ROAD_RIGHT + 24.f;
    drawText(target, "CAMERA FEED", x, 24.f, 28, C::white, true);
    drawText(target, "FORWARD OBSTACLE VIEW", x, 58.f, 18, C::muted);
    drawCameraPreview(target, x, 102.f);
    drawDetectionList(target, x, 382.f);
    drawStats(target, x, 642.f);
}

void drawCameraPreview(sf::RenderTarget& target, float x, float y) {
    roundedRectFill(target, x, y, RIGHT_PANEL - 48.f, 230.f, 8.f, C::cameraBg);
    roundedRectOutline(target, x, y, RIGHT_PANEL - 48.f, 230.f, 8.f, C::line, 1.f);

    float innerX = x + 14.f, innerY = y + 12.f;
    float innerW = RIGHT_PANEL - 76.f, innerH = 206.f;
    roundedRectFill(target, innerX, innerY, innerW, innerH, 4.f, C::cameraInner);

    sf::RectangleShape centerLine({ 2.f, innerH });
    centerLine.setPosition(innerX + innerW / 2.f - 1.f, innerY);
    centerLine.setFillColor(C::cameraLine);
    target.draw(centerLine);

    sf::RectangleShape leftEdge({ 2.f, innerH });
    leftEdge.setPosition(innerX + 8.f, innerY);
    leftEdge.setFillColor(C::white);
    target.draw(leftEdge);

    sf::RectangleShape rightEdge({ 2.f, innerH });
    rightEdge.setPosition(innerX + innerW - 10.f, innerY);
    rightEdge.setFillColor(C::white);
    target.draw(rightEdge);

    for (const auto* obstacle : detectedObstacles()) {
        float distance = std::max(1.f, MATATU_Y - (obstacle->y + obstacle->h));
        float normalized = std::max(0.f, std::min(1.f, distance / DETECTION_DISTANCE));
        float markerY = innerY + innerH - normalized * innerH;

        if (obstacle->kind == "VEHICLE") {
            float laneX = innerX + (obstacle->lane + 0.5f) * innerW / LANE_COUNT;
            roundedRectFill(target, laneX - 19.f, markerY - 13.f, 38.f, 26.f, 5.f, C::red);
        } else {
            roundedRectFill(target, innerX + 14.f, markerY - 10.f, innerW - 28.f, 20.f, 3.f,
                             obstacle->cleared ? C::green : C::white);
        }
    }

    bool live = state.alertFlash > 0.f;
    drawCircle(target, x + RIGHT_PANEL - 66.f, y + 18.f, 6.f, live ? C::red : C::green);
    drawText(target, "LIVE", x + RIGHT_PANEL - 114.f, y + 11.f, 15, live ? C::red : C::green);
}

void drawDetectionList(sf::RenderTarget& target, float x, float y) {
    auto detected = detectedObstacles();
    drawText(target, "Detected Obstacles", x, y, 21, C::white);
    drawText(target, std::to_string(detected.size()) + " in camera range", x, y + 30.f, 18, C::muted);

    if (detected.empty()) {
        roundedRectFill(target, x, y + 70.f, RIGHT_PANEL - 48.f, 58.f, 8.f, C::panel2);
        drawText(target, "Road clear", x + 16.f, y + 89.f, 18, C::green);
        return;
    }

    for (size_t i = 0; i < detected.size(); ++i) {
        const Obstacle* obstacle = detected[i];
        float rowY = y + 66.f + i * 48.f;
        roundedRectFill(target, x, rowY, RIGHT_PANEL - 48.f, 38.f, 7.f, C::panel2);
        roundedRectOutline(target, x, rowY, RIGHT_PANEL - 48.f, 38.f, 7.f, i == 0 ? C::red : C::line, 1.f);

        float distance = std::max(0.f, MATATU_Y - (obstacle->y + obstacle->h));
        std::string lane = obstacle->lane < 0 ? "ALL" : lanes[obstacle->lane];
        std::string status = obstacle->cleared ? "  OK" : padStart(fixedNumber(distance, 0), 4) + "px";
        std::string line = padEnd(obstacle->kind, 8) + " " + padEnd(lane, 6) + " " + status;
        drawText(target, line, x + 10.f, rowY + 11.f, 15, C::white);
    }
}

void drawStats(sf::RenderTarget& target, float x, float y) {
    drawText(target, "Session Metrics", x, y, 21, C::white);

    std::ostringstream speedStr;
    speedStr << std::fixed << std::setprecision(1) << state.speed;

    struct Row { std::string label, value; };
    Row rows[] = {
        { "Avoided", std::to_string(state.avoided) },
        { "Final Speed", speedStr.str() },
        { "Mode", state.braking ? "BRAKING" : "AUTO" },
    };
    for (int i = 0; i < 3; ++i) {
        float yy = y + 42.f + i * 34.f;
        drawText(target, rows[i].label, x, yy, 18, C::muted);
        drawText(target, rows[i].value, x + 170.f, yy, 18, C::white);
    }
}

void drawGameOver(sf::RenderTarget& target) {
    sf::RectangleShape overlay({ BASE_W, BASE_H });
    overlay.setFillColor(sf::Color(0, 0, 0, 178));
    target.draw(overlay);

    float boxX = 320.f, boxY = 235.f, boxW = 560.f, boxH = 330.f;
    roundedRect(target, boxX, boxY, boxW, boxH, 12.f, &C::gameOverBg, &C::red, 3.f);
    drawText(target, "COLLISION DETECTED", boxX + boxW / 2.f, boxY + 36.f, 44, C::red, true, Align::Center);

    std::string object = state.collision ? state.collision->kind : "UNKNOWN";
    std::ostringstream distStr, speedStr;
    distStr << std::fixed << std::setprecision(1) << state.distance;
    speedStr << std::fixed << std::setprecision(1) << state.speed;

    std::string lines[] = {
        "Impact object: " + object,
        "Distance traveled: " + distStr.str() + " m",
        "Time survived: " + formatTime(state.elapsed),
        "Obstacles avoided: " + std::to_string(state.avoided),
        "Final speed: " + speedStr.str() + " px/s",
    };
    for (int i = 0; i < 5; ++i) {
        drawText(target, lines[i], boxX + 74.f, boxY + 112.f + i * 34.f, 21, C::white);
    }

    drawText(target, "Press SPACE to restart", boxX + boxW / 2.f, boxY + boxH - 52.f, 21, C::amber, false, Align::Center);
}
