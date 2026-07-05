// MEMBER 1: Project setup, window, constants, colors, and shared drawing helpers.
#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <array>

// ---- Base "canvas" size (everything is drawn at this resolution, then
// scaled/letterboxed onto the real window, same idea as the JS canvas). ----
constexpr float BASE_W = 1200.f;
constexpr float BASE_H = 800.f;
constexpr float LEFT_PANEL = 260.f;
constexpr float RIGHT_PANEL = 300.f;
constexpr float ROAD_X = LEFT_PANEL;
constexpr float ROAD_W = BASE_W - LEFT_PANEL - RIGHT_PANEL;
constexpr float ROAD_RIGHT = ROAD_X + ROAD_W;
constexpr int   LANE_COUNT = 3;
constexpr float LANE_W = ROAD_W / LANE_COUNT;

constexpr float MATATU_W = 86.f;
constexpr float MATATU_H = 122.f;
constexpr float MATATU_Y = BASE_H - 180.f;

constexpr float BASE_SPEED = 200.f;
constexpr float BRAKE_DECEL = 300.f;
constexpr float RECOVERY_ACCEL = 70.f;
constexpr float SPAWN_INTERVAL = 2.0f;
constexpr float DETECTION_DISTANCE = 300.f;
constexpr float LANE_CHANGE_COOLDOWN = 120.f; // milliseconds
constexpr float CROSSING_CLEAR_SPEED = 12.f;
constexpr float CROSSING_CLEAR_DISTANCE = 90.f;

// ---- Colors ----
namespace C {
    inline sf::Color fromHex(unsigned int hex) {
        return sf::Color((hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF);
    }

    inline const sf::Color bg          = fromHex(0x080a0d);
    inline const sf::Color road        = fromHex(0x1c1f23);
    inline const sf::Color roadShadow  = fromHex(0x121417);
    inline const sf::Color panel       = fromHex(0x161c24);
    inline const sf::Color panel2      = fromHex(0x1c242e);
    inline const sf::Color line        = fromHex(0x3e4c5c);
    inline const sf::Color white       = fromHex(0xf0f4f8);
    inline const sf::Color muted       = fromHex(0x9cabb8);
    inline const sf::Color yellow      = fromHex(0xffc72c);
    inline const sf::Color matatuYellow= fromHex(0xfad63b);
    inline const sf::Color matatuBlue  = fromHex(0x2d7ddd);
    inline const sf::Color cyan        = fromHex(0x58dceb);
    inline const sf::Color green       = fromHex(0x36d377);
    inline const sf::Color red         = fromHex(0xef4444);
    inline const sf::Color amber       = fromHex(0xffab2e);
    inline const sf::Color purple      = fromHex(0x9c76ff);
    inline const sf::Color black       = fromHex(0x080a0d);
    inline const sf::Color panelDark   = fromHex(0x20252b);
    inline const sf::Color vehicleTrim = fromHex(0x821e1e);
    inline const sf::Color glassBlue   = fromHex(0x96dceb);
    inline const sf::Color headlight   = fromHex(0xebf5ff);
    inline const sf::Color asphaltDark = fromHex(0x2a2d31);
    inline const sf::Color cameraBg    = fromHex(0x0b0f14);
    inline const sf::Color cameraInner = fromHex(0x1e2227);
    inline const sf::Color cameraLine  = fromHex(0xe1be38);
    inline const sf::Color keyBg       = fromHex(0x0e1218);
    inline const sf::Color gameOverBg  = fromHex(0x181e26);
    inline const sf::Color matatuTrim  = fromHex(0x8c680a);
    inline const sf::Color lampYellow  = fromHex(0xffee75);
    inline const sf::Color windowTint  = fromHex(0xa6e5ee);
}

inline const std::array<std::string, LANE_COUNT> lanes = {"LEFT", "CENTER", "RIGHT"};

// ---- Font (shared, loaded once) ----
sf::Font& gameFont();

// ---- Window / render target shared by all members ----
sf::RenderWindow& gameWindow();
sf::RenderTexture& baseCanvas(); // fixed BASE_W x BASE_H drawing surface

void initWindow();

// ---- Shared drawing helpers (equivalent of roundedRect/circle/text in JS) ----
void roundedRect(sf::RenderTarget& target, float x, float y, float w, float h, float r,
                  const sf::Color* fill, const sf::Color* stroke = nullptr, float lineWidth = 1.f);

inline void roundedRectFill(sf::RenderTarget& target, float x, float y, float w, float h, float r, sf::Color fill) {
    roundedRect(target, x, y, w, h, r, &fill, nullptr);
}

inline void roundedRectOutline(sf::RenderTarget& target, float x, float y, float w, float h, float r,
                                sf::Color stroke, float lineWidth) {
    roundedRect(target, x, y, w, h, r, nullptr, &stroke, lineWidth);
}

void drawCircle(sf::RenderTarget& target, float x, float y, float r, sf::Color fill);

enum class Align { Left, Center, Right };

void drawText(sf::RenderTarget& target, const std::string& value, float x, float y,
              unsigned int size = 18, sf::Color color = C::white, bool bold = false,
              Align align = Align::Left);

std::string formatTime(float seconds);
std::string padStart(const std::string& s, size_t width, char fillChar = '0');
std::string padEnd(const std::string& s, size_t width, char fillChar = ' ');
std::string fixedNumber(float value, int decimals);
