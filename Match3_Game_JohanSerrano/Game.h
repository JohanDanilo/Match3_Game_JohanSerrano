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
    GameWon,
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

    Player currentPlayer;
    bool running = true;
    GameState state = GameState::MainMenu;

    int score = 0;
    int moves = 20;
    int currentLevelIndex = 0;

    LevelManager levelManager;
    Level* activeLevel = nullptr;

    vector<LevelIcon> levelIcons;
    Sprite cursor;
    Sprite highScoresButton;
    Sprite exitButton;
    bool levelMapUIInitialized = false;

    std::vector<std::pair<std::string, int>> highscoreCache;

    void init();
    void runMainMenu();
    void runLevelMap();
    void runHighScores();
    void runGameLoop();
    void runLevelComplete();
    void runGameOver();
    void runGameWon();

    void startLevel();

    void initLevelMapUI();
    void syncLevelIconsByProgress();
    void drawCurrentScene();

public:
    Game();
    ~Game();

    void run();
};
