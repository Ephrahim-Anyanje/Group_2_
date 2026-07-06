// MEMBER 1: Project setup, window, constants, colors, and shared drawing helpers.
#include "member1_setup_config.h"
#include <cmath>
#include <sstream>
#include <iomanip>

sf::Font& gameFont() {
    static sf::Font regular;
    static sf::Font bold;
    static bool loaded = false;
    static sf::Font* active = &regular;
    if (!loaded) {
        // Try a few common monospace font locations; fall back gracefully.
        const char* candidates[] = {
            "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf",
            "/usr/share/fonts/truetype/liberation/LiberationMono-Bold.ttf",
        };
        for (auto path : candidates) {
            if (bold.loadFromFile(path)) break;
        }
        const char* regularCandidates[] = {
            "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
            "/usr/share/fonts/truetype/liberation/LiberationMono-Regular.ttf",
        };
        for (auto path : regularCandidates) {
            if (regular.loadFromFile(path)) break;
        }
        loaded = true;
    }
    return regular;
}

sf::Font& gameFontBold() {
    static sf::Font bold;
    static bool loaded = false;
    if (!loaded) {
        const char* candidates[] = {
            "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf",
            "/usr/share/fonts/truetype/liberation/LiberationMono-Bold.ttf",
        };
        for (auto path : candidates) {
            if (bold.loadFromFile(path)) break;
        }
        loaded = true;
    }
    return bold;
}

sf::RenderWindow& gameWindow() {
    static sf::RenderWindow window;
    return window;
}

sf::RenderTexture& baseCanvas() {
    static sf::RenderTexture texture;
    return texture;
}

void initWindow() {
    gameWindow().create(sf::VideoMode(1000, 700),
                         "Autonomous Matatu Conductor Monitoring System",
                         sf::Style::Default);
    gameWindow().setFramerateLimit(60);
    baseCanvas().create(static_cast<unsigned int>(BASE_W), static_cast<unsigned int>(BASE_H));
    gameFont();      // force-load regular
    gameFontBold();  // force-load bold
}

// Builds a rounded-rectangle polygon, matching the arcTo-based path used
// in the JS roundedRect() helper (one shared shape, filled and/or outlined).
static sf::ConvexShape buildRoundedRect(float x, float y, float w, float h, float r) {
    r = std::min(r, std::min(w, h) / 2.f);
    if (r < 0.f) r = 0.f;

    const int segments = 8; // per corner
    std::vector<sf::Vector2f> points;

    struct Corner { float cx, cy, startAngle; };
    Corner corners[4] = {
        { x + w - r, y + r,     -90.f }, // top-right
        { x + w - r, y + h - r,   0.f }, // bottom-right
        { x + r,     y + h - r,  90.f }, // bottom-left
        { x + r,     y + r,     180.f }, // top-left
    };

    for (const auto& c : corners) {
        for (int i = 0; i <= segments; ++i) {
            float angle = (c.startAngle + (90.f * i / segments)) * 3.14159265f / 180.f;
            points.push_back({ c.cx + r * std::cos(angle), c.cy + r * std::sin(angle) });
        }
    }

    sf::ConvexShape shape;
    shape.setPointCount(points.size());
    for (size_t i = 0; i < points.size(); ++i) shape.setPoint(i, points[i]);
    return shape;
}

void roundedRect(sf::RenderTarget& target, float x, float y, float w, float h, float r,
                  const sf::Color* fill, const sf::Color* stroke, float lineWidth) {
    sf::ConvexShape shape = buildRoundedRect(x, y, w, h, r);

    if (fill) {
        shape.setFillColor(*fill);
    } else {
        shape.setFillColor(sf::Color::Transparent);
    }

    if (stroke) {
        shape.setOutlineColor(*stroke);
        shape.setOutlineThickness(lineWidth);
    }

    target.draw(shape);
}

void drawCircle(sf::RenderTarget& target, float x, float y, float r, sf::Color fill) {
    sf::CircleShape circle(r);
    circle.setOrigin(r, r);
    circle.setPosition(x, y);
    circle.setFillColor(fill);
    target.draw(circle);
}

void drawText(sf::RenderTarget& target, const std::string& value, float x, float y,
              unsigned int size, sf::Color color, bool bold, Align align) {
    sf::Text text;
    text.setFont(bold ? gameFontBold() : gameFont());
    text.setString(value);
    text.setCharacterSize(size);
    text.setFillColor(color);

    sf::FloatRect bounds = text.getLocalBounds();
    float drawX = x;
    if (align == Align::Center) {
        drawX = x - bounds.width / 2.f - bounds.left;
    } else if (align == Align::Right) {
        drawX = x - bounds.width - bounds.left;
    } else {
        drawX = x - bounds.left;
    }
    // SFML text glyphs have vertical padding baked into local bounds;
    // nudge up slightly so size/position feel consistent with the JS canvas version.
    text.setPosition(drawX, y - bounds.top * 0.15f);

    target.draw(text);
}

std::string formatTime(float seconds) {
    int minutes = static_cast<int>(seconds / 60.f);
    int secs = static_cast<int>(std::fmod(seconds, 60.f));
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << minutes << ":"
        << std::setw(2) << std::setfill('0') << secs;
    return oss.str();
}

std::string padStart(const std::string& s, size_t width, char fillChar) {
    if (s.size() >= width) return s;
    return std::string(width - s.size(), fillChar) + s;
}

std::string padEnd(const std::string& s, size_t width, char fillChar) {
    if (s.size() >= width) return s;
    return s + std::string(width - s.size(), fillChar);
}

std::string fixedNumber(float value, int decimals) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(decimals) << value;
    return oss.str();
}
