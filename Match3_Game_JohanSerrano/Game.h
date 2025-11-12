#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "Board.h"
#include "LevelManager.h"
#include "UIManager.h"
#include "UXManager.h"
#include "Objective.h"
#include "PersistenceManager.h"
#include "Player.h"

using namespace sf;
using namespace std;

enum class GameState {
    MainMenu,
    LevelMap,
    Playing,
    LevelComplete,
    GameOver,
    HighScores,
    Exit
};

struct LevelIcon {
    Vector2f position;
    Sprite lockSprite;
    Sprite checkSprite;
    bool unlocked = false;
    bool completed = false;
};

class Game {
private:
    Board board;
    UIManager* uiManager = nullptr;
    UXManager* ux = nullptr;
    Font font;
    Texture uiTexture;

    Player currentPlayer;        // reemplaza playerName, score, totalScore
    bool running = true;
    GameState state = GameState::MainMenu;

    int score = 0;               // puntaje del nivel actual
    int moves = 20;
    int currentLevelIndex = 0;

    LevelManager levelManager;
    Level* activeLevel = nullptr;

    // ---- LevelMap UI ----
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
    void runHighScores();
    void runGameLoop();
    void runLevelComplete();
    void runGameOver();

    void startLevel();
    void drawCurrentScene();

    void initLevelMapUI();
    void syncLevelIconsByProgress();
};
