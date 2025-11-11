#include "LevelManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

/* ===================== CONSTRUCTOR / DESTRUCTOR ===================== */

LevelManager::LevelManager() : currentLevelIndex(0) {
    // Carga automática desde archivo de texto
    loadLevelsFromFile("assets/data/levels.txt");
}

LevelManager::~LevelManager() {
    for (Level* level : levels) {
        delete level;
    }
    levels.clear();
}

/* ===================== CARGA DE NIVELES DESDE ARCHIVO ===================== */

void LevelManager::loadLevelsFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "[ERROR] No se pudo abrir el archivo de niveles: " << filename << endl;
        return;
    }

    levels.clear();
    string line;
    int lineNumber = 0;

    while (getline(file, line)) {
        lineNumber++;
        if (line.empty() || line[0] == '#') continue; // Ignora comentarios o líneas vacías

        stringstream ss(line);
        int number, typeInt, target, gemKind, moves, obstacles;

        ss >> number >> typeInt >> target >> gemKind >> moves >> obstacles;
        if (ss.fail()) {
            cerr << "[WARN] Línea inválida en " << lineNumber << ": " << line << endl;
            continue;
        }

        ObjectiveType type = static_cast<ObjectiveType>(typeInt);
        createLevelGeneric(number, type, target, gemKind, moves, obstacles);
    }

    file.close();

    cout << "[INFO] Se cargaron " << levels.size() << " niveles desde archivo '" << filename << "'.\n";
}

/* ===================== CREACIÓN DE NIVEL ===================== */

void LevelManager::createLevelGeneric(int number, ObjectiveType type, int target, int gemKind, int moves, int obstacles) {
    Objective* obj = new Objective(type, target, gemKind);
    Level* lvl = new Level(number, moves, obj);
    lvl->setObstacleCount(obstacles);
    levels.push_back(lvl);
}

/* ===================== CONTROL DE NIVELES ===================== */

Level* LevelManager::getCurrentLevel() {
    if (currentLevelIndex < levels.size()) {
        return levels[currentLevelIndex];
    }
    cerr << "[WARN] Solicitud de nivel inválido. Índice actual: " << currentLevelIndex << endl;
    return nullptr;
}

bool LevelManager::hasNextLevel() const {
    return currentLevelIndex < static_cast<int>(levels.size()) - 1;
}

void LevelManager::nextLevel() {
    if (hasNextLevel()) {
        currentLevelIndex++;
    }
    else {
        cout << "[INFO] No hay más niveles disponibles.\n";
    }
}

void LevelManager::resetCurrentLevel() {
    if (currentLevelIndex < levels.size()) {
        levels[currentLevelIndex]->resetLevel();
    }
}

void LevelManager::reset() {
    currentLevelIndex = 0;
    for (Level* level : levels) {
        if (level) {
            level->resetLevel();
        }
    }
}

int LevelManager::getCurrentLevelNumber() const {
    return currentLevelIndex + 1;
}
