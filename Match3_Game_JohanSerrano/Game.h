#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Board.h"
#include "LevelManager.h"
#include "UIManager.h"
#include "UXManager.h"
#include "Objective.h"

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

class Game {

private:
    Board board;
    UIManager* uiManager = nullptr;
    UXManager* ux = nullptr;
    Font font;
    Texture uiTexture;

    bool running = true;
    GameState state = GameState::MainMenu;
    int score = 0;
    int moves = 20;

    LevelManager levelManager;     
    Level* activeLevel = nullptr;

    

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
};
