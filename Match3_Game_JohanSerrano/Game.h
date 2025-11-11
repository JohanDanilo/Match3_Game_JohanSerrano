#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>        // <-- asegúrate de incluir
#include "Board.h"
#include "LevelManager.h"
#include "UIManager.h"
#include "UXManager.h"
#include "Objective.h"
#include "PersistenceManager.h"

using namespace sf;
using namespace std;

enum class GameState {
    MainMenu,
    LevelMap,
    Playing,
    LevelComplete,
    GameOver,
    Exit
};

struct LevelIcon {
    Vector2f position;      // centro de la “isla” del nivel
    Sprite lockSprite;      // mostrado si bloqueado
    Sprite checkSprite;     // mostrado si completado
    bool unlocked = false;  // true solo para el nivel actual
    bool completed = false; // true para niveles < actual
};

class Game {
private:
    Board board;
    UIManager* uiManager = nullptr;
    UXManager* ux = nullptr;
    Font font;
    Texture uiTexture;

    string playerName;
    bool running = true;
    GameState state = GameState::MainMenu;

    int score = 0;           // puntaje del nivel actual
    int totalScore = 0;      // puntaje acumulado total
    int moves = 20;
    int currentLevelIndex = 0;

    LevelManager levelManager;
    Level* activeLevel = nullptr;

    // ---- LevelMap UI state ----
    vector<LevelIcon> levelIcons;
    Sprite cursor;
    Sprite highScoresButton;
    Sprite exitButton;
    bool levelMapUIInitialized = false;

public:
    Game();
    ~Game();

    void run();

private:
    void init();
    void runMainMenu();
    void runLevelMap();
    void runGameLoop();
    void runLevelComplete();
    void runGameOver();

    void startLevel();
    void drawCurrentScene();

    // ---- helpers LevelMap ----
    void initLevelMapUI();                 // crea sprites y coordenadas
    void syncLevelIconsByProgress();       // marca completed/unlocked según progreso
};
