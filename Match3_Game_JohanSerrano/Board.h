#pragma once
#include "Gem.h"
#include <SFML/Graphics.hpp>
#include <ctime>
#include <cstdlib>
#include <cmath>

using namespace sf;
using namespace std;

const int ROWS = 8;
const int COLS = 8;

class Board {
private:
    enum BoardState { Idle, Swapping, Reverting, Scoring, Moving };
    BoardState state;

    Gem grid[ROWS][COLS];
    bool matches[ROWS][COLS] = { false };

    Gem* firstGem = nullptr;
    Gem* secondGem = nullptr;

    int firstRow = -1, firstCol = -1;
    int secondRow = -1, secondCol = -1;

    Texture texture;

    bool playerInitiatedMove = false;
    Vector2f swapOrigPos1;
    Vector2f swapOrigPos2;


    void findMatches();
    int clearMatches();
    void applyGravity();
    void refill();

public:
    Board();

    void initialize();
    void loadTexture();
    void draw(RenderWindow& window);

    bool areAdjacent(int row1, int col1, int row2, int col2) const;
    bool trySwapIndices(int row1, int col1, int row2, int col2);

    void update(float deltaTime, int& scoreGained, bool& moveConsumed);

    int getState() const;
    Gem& getGem(int row, int col);

};
