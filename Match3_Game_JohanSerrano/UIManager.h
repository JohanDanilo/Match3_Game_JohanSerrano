#pragma once
#include <SFML/Graphics.hpp>
#include "Objective.h"
#include "ResourceManager.h"

using namespace sf;
using namespace std;

class UIManager {
private:
    const Font* font;
    const Texture& gemTexture;

    int score = 0;
    int moves = 0;
    int level = 1;
    Objective* objective = nullptr;

    // === HUD SUPERIOR ===
    Vector2f headerPosScore = { 70.f, 8.f };
    Vector2f headerPosMoves = { 550.f, 8.f };
    Vector2f headerPosLevel = { 340.f, 8.f };

    // === PANEL INFERIOR ===
    Vector2f panelPos = { 8.f, 535.f };
    Vector2f titlePos = { 10.f, 520.f };
    Vector2f objTextPos = { 10.f, 540.f };
    Vector2f progressBarPos = { 12.f, 580.f };

    // === GEM TARGET ===
    Vector2f targetLabelPos = { 350.f, 535.f };
    Vector2f gemFramePos = { 465.f, 535.f };
    Vector2f gemSpritePos = { 472.f, 538.f };

public:
    UIManager(const Font* f, const Texture& t);
    void update(int currentScore, int remainingMoves, int currentLevel, Objective* obj);
    void draw(RenderWindow& window);
};
