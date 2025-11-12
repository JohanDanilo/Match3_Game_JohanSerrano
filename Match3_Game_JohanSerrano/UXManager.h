#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include "ResourceManager.h"
#include "UIManager.h"
#include "Board.h"
#include "Player.h"

using namespace sf;
using namespace std;

enum class SceneType {
    MainMenu,
    LevelMap,
    Gameplay,
    LevelComplete,
    GameOver,
    HighScores,
	GameWon,
};

class UXManager {
private:
    RenderWindow window;
    SceneType currentScene;

    const Font* font;
    UIManager* uiManager;

    bool fadeActive = false;
    RectangleShape fadeRect;
    Clock fadeClock;
    float fadeDuration = 0.f;
    Color fadeColor;
    bool fadeIn = true;

public:

    UXManager(const Font* f, UIManager* ui);
    ~UXManager();

    RenderWindow& getWindow();
    bool isOpen() const;
    void close();
    void clear(const Color& color = Color::Black);
    void display();

    void setScene(SceneType scene);
    SceneType getScene() const;
    void drawBackground(const string& textureName);

    void startFade(Color color, float duration, bool fadeInEffect);
    void updateFade();
    void drawFade();

    bool pollGlobalEvent(Event& e);

    void handleBoardEvents(Board& board, Event& e);
    void drawBoard(Board& board);
    void drawGameUI(UIManager& ui);
};
