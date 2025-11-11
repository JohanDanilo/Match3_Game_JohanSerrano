#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include "ResourceManager.h"
#include "UIManager.h"
#include "Board.h"

using namespace sf;
using namespace std;

enum class SceneType {
    MainMenu,
    LevelMap,
    Gameplay,
    LevelComplete,
    GameOver
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

    // --- Gestión de ventana ---
    RenderWindow& getWindow();
    bool isOpen() const;
    void close();
    void clear(const Color& color = Color::Black);
    void display();

    // --- Escenas ---
    void setScene(SceneType scene);
    SceneType getScene() const;
    void drawBackground(const string& textureName);

    // --- Efectos visuales ---
    void startFade(Color color, float duration, bool fadeInEffect);
    void updateFade();
    void drawFade();

    // --- Eventos globales ---
    bool pollGlobalEvent(Event& e);

    // --- NUEVO: Experiencia de juego ---
    void handleBoardEvents(Board& board, Event& e);
    void drawBoard(Board& board);
    void drawGameUI(UIManager& ui);
};
