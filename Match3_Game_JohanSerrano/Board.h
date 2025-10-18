#pragma once
#include "Gem.h"
#include "NormalGem.h"
#include "BombGem.h"
#include "IceGem.h"
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

    Gem* grid[ROWS][COLS];
    bool matches[ROWS][COLS] = { false };

    Gem* firstGem = nullptr;
    Gem* secondGem = nullptr;

    int firstRow = -1, firstCol = -1;
    int secondRow = -1, secondCol = -1;

    Texture texture;

    bool playerInitiatedMove = false;
    Vector2f swapOrigPos1;
    Vector2f swapOrigPos2;


    void revertSwap();

    void findMatches();
    void checkLineMatches(bool horizontal);
    void markMatches(bool horizontal, int outer, int lastIndex, int count);
    int clearMatches();
    void applyGravity();
    void refill();

    void spawnGem(int r, int c);

public:
    Board();
    ~Board();

    void initialize();
    bool createsMatch(int row, int col, int kind);
    void loadTexture();
    void draw(RenderWindow& window);

    bool areAdjacent(int row1, int col1, int row2, int col2) const;
    bool trySwapIndices(int row1, int col1, int row2, int col2);

    void update(float deltaTime, int& scoreGained, bool& moveConsumed);

    void handleIdleState();

    void handleSwappingState(float deltaTime);

    void handleRevertingState(float deltaTime);

    void handleScoringState(float deltaTime, int& scoreGained, bool& moveConsumed);

    void handleMovingState(float deltaTime);

    bool checkAnyMatch();

    void triggerDisappearance();

    int getState() const;
    Gem* getGem(int row, int col);

};
