#pragma once
#include <vector>
#include <string>
#include "Level.h"
#include "Objective.h"

using namespace std;

class LevelManager {
private:
    vector<Level*> levels;
    int currentLevelIndex;

    void createLevelGeneric(int number, ObjectiveType type, int target, int gemKind, int moves, int obstacles);

public:
    LevelManager();
    ~LevelManager();

    // Carga los niveles desde un archivo de texto
    void loadLevelsFromFile(const string& filename);

    // Control general
    Level* getCurrentLevel();
    bool hasNextLevel() const;
    void nextLevel();
    void reset();
    void resetCurrentLevel();
    int getCurrentLevelNumber() const;
    void setCurrentLevelIndex(int index);


};
