#pragma once
#include "Objective.h"

class Level {
private:
    int levelNumber;
    int maxMoves;
    Objective* objective;
    int obstacleCount = 0;

public:
    Level(int number, int moves, Objective* anObjective);
    ~Level();

    Objective* getObjective();
    int getLevelNumber() const;
    int getMaxMoves() const;
    int getObstacleCount() const;
    void setObstacleCount(int count);

    bool isLevelComplete() const;
    void resetLevel();
};