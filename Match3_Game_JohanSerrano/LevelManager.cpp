#include "LevelManager.h"

LevelManager::LevelManager() : currentLevelIndex(0) {
    initializeLevels();
}

LevelManager::~LevelManager() {
    for (Level* level : levels) {
        delete level;
    }
    levels.clear();
}

void LevelManager::initializeLevels() {
    createLevel1();
    createLevel2();
    createLevel3();
}

void LevelManager::createLevel1() {
    Objective* obj1 = new Objective(ObjectiveType::CollectGems, 20, 0);
    Level* level1 = new Level(1, 20, obj1);
    level1->setObstacleCount(0);
    levels.push_back(level1);
}

void LevelManager::createLevel2() {
    Objective* obj2 = new Objective(ObjectiveType::CollectGems, 20, 2);
    Level* level2 = new Level(2, 25, obj2);
    level2->setObstacleCount(3);
    levels.push_back(level2);
}

void LevelManager::createLevel3() {
    Objective* obj3 = new Objective(ObjectiveType::ClearObstacles, 20, 5);
    Level* level3 = new Level(3, 30, obj3);
    level3->setObstacleCount(5);
    levels.push_back(level3);
}

Level* LevelManager::getCurrentLevel() {
    if (currentLevelIndex < levels.size()) {
        return levels[currentLevelIndex];
    }
    return nullptr;
}

bool LevelManager::hasNextLevel() const {
    return currentLevelIndex < levels.size() - 1;
}

void LevelManager::nextLevel() {
    if (hasNextLevel()) {
        currentLevelIndex++;
    }
}

void LevelManager::resetCurrentLevel() {
    if (currentLevelIndex < levels.size()) {
        levels[currentLevelIndex]->resetLevel();
    }
}

int LevelManager::getCurrentLevelNumber() const {
    return currentLevelIndex + 1;
}