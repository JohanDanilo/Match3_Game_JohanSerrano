#pragma once
#include <SFML/Graphics.hpp>
#include "Gem.h"
#include "NormalGem.h"
#include "BombGem.h"
#include "IceGem.h"
#include "Level.h"
#include "Objective.h"
#include "Obstacle.h"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

using namespace sf;
using namespace std;


static const int ROWS = 8;
static const int COLS = 8;

class Board {
private:
    

    Gem* grid[ROWS][COLS]{};
    bool matches[ROWS][COLS]{};
    vector<Obstacle*> obstacles;

    Level* currentLevel = nullptr;

    enum State { Idle, Swapping, Reverting, Scoring, Moving };
    State state = Idle;

    bool inputLocked = false;

    // Selección / swap
    Gem* firstGem = nullptr;
    Gem* secondGem = nullptr;
    int firstRow = -1, firstCol = -1;
    int secondRow = -1, secondCol = -1;
    Vector2f swapOrigPos1, swapOrigPos2;
    bool playerInitiatedMove = false;

    // Métodos privados
    void revertSwap();
    void findMatches();
    void checkLineMatches(bool horizontal);
    void markMatches(bool horizontal, int outer, int lastIndex, int count);
    int clearMatches();
    void applyGravity();
    void refill();
    void spawnGem(int r, int c);
    void spawnSpecialGem(int row, int col, bool horizontal);
    void updateObjectivesOnMatch(int row, int col);
    void damageAdjacentObstacles(int row, int col);
    bool hasObstacleAt(int row, int col) const;

public:
    Board();
    ~Board();

    // Ciclo principal
    void initialize();
    void draw(RenderWindow& window);
    void update(float deltaTime, int& scoreGained, bool& moveConsumed);

    // Estados del juego
    void handleIdleState();
    void handleSwappingState(float deltaTime, bool& moveConsumed);
    void handleRevertingState(float deltaTime);
    void handleScoringState(float deltaTime, int& scoreGained, bool& moveConsumed);
    void handleMovingState(float deltaTime);

    // Lógica de matches
    bool trySwapIndices(int row1, int col1, int row2, int col2);
    bool areAdjacent(int row1, int col1, int row2, int col2) const;
    bool createsMatch(int row, int col, int kind);
    bool checkAnyMatch();
    void triggerDisappearance();

    // Efectos especiales
    void activateSpecialGemsInMatches();
    void activateBombEffect(int row, int col);
    void activateIceEffect(int row);

    // Nivel y objetivos
    void setCurrentLevel(Level* level);
    void updateScoreObjective(int scoreGained);
    void clearCurrentLevel();

    // Obstáculos
    void placeObstacles(int count);
    void clearObstacles();

    // Utilidades
    int getState() const;
    Gem* getGem(int row, int col);

    // Input del jugador
    void handleGemClick(int row, int col);

};