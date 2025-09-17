#pragma once

#include "Board.h"
#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;
using namespace std;

enum class GameState { MainMenu, Playing, GameOver };

class Game {
private:
    Board board;
    int score;
    int moves;
    GameState state;
    bool running;

    Texture mainMenuTexture, backgroundTexture, gameOverTexture;
    Sprite mainMenuSprite, backgroundSprite, gameOverSprite;
    Font font;

    int firstSelectedRow = -1, firstSelectedCol = -1;

    Vector2f getClickPosition(RenderWindow& window);
    bool isClickInsideBoard(RenderWindow& window);

    void runMainMenu();
    void runGameLoop();
    void runGameOver();
    void selectGem(RenderWindow& window);
    void refillMoves();
    void clearScore();

public:
    Game();
    void init();
    void loadResources();
    void run();
};

