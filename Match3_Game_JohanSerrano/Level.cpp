#include "Level.h"

Level::Level(int number, int moves, Objective* anObjective) {
    levelNumber = number;
    maxMoves = moves;
    objective = anObjective;
}

Level::~Level() {
    if (objective) {
        delete objective;
        objective = nullptr;
    }
}

Objective* Level::getObjective() {
    return objective;
}

int Level::getLevelNumber() const {
    return levelNumber;
}

int Level::getMaxMoves() const {
    return maxMoves;
}

int Level::getObstacleCount() const {
    return obstacleCount;
}

void Level::setObstacleCount(int count) {
    obstacleCount = count;
}

bool Level::isLevelComplete() const {
    if (objective) {
        return objective->isCompleted();
    }
    return false;
}

void Level::resetLevel() {
    if (objective) {
        objective->reset();
    }
}