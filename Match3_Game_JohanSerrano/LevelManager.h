#pragma once
#include "Level.h"
#include <vector>
using namespace std;

class LevelManager {
private:
    vector<Level*> levels;
    int currentLevelIndex = 0;

    void createLevel1();
    void createLevel2();
    void createLevel3();

public:
    LevelManager();
    ~LevelManager();

    void initializeLevels();
    Level* getCurrentLevel();
    bool hasNextLevel() const;
    void nextLevel();
    void resetCurrentLevel();
    int getCurrentLevelNumber() const;
};