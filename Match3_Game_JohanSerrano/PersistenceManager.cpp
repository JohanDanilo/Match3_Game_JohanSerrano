#include "PersistenceManager.h"
#include <fstream>
#include <filesystem>
#include <iostream>

using namespace std;
namespace fs = filesystem;

const string SAVE_DIR = "saves/";

void PersistenceManager::saveProgress(const string& playerName, int level, int score) {
    try {
        fs::create_directory(SAVE_DIR);
        string filename = SAVE_DIR + playerName + ".txt";
        ofstream file(filename);
        if (!file.is_open()) throw runtime_error("No se pudo abrir el archivo del jugador.");

        file << playerName << " " << level << " " << score << endl;
        file.close();
        cout << "[SAVE] Progreso guardado para " << playerName << ": nivel " << level << ", score " << score << endl;
    }
    catch (const exception& e) {
        cerr << "[ERROR] PersistenceManager::saveProgress - " << e.what() << endl;
    }
}

bool PersistenceManager::loadProgress(const string& playerName, int& level, int& score) {
    string filename = SAVE_DIR + playerName + ".txt";
    ifstream file(filename);

    if (!file.is_open()) {
        cout << "[INFO] No existe progreso previo para " << playerName << endl;
        return false;
    }

    string name;
    file >> name >> level >> score;
    file.close();
    cout << "[LOAD] Progreso cargado: " << name << " Nivel=" << level << " Score=" << score << endl;
    return true;
}

// Ranking general: lee todos los archivos y ordena por score
vector<pair<string, int>> PersistenceManager::loadRanking() {
    vector<pair<string, int>> ranking;

    if (!fs::exists(SAVE_DIR)) return ranking;

    for (const auto& entry : fs::directory_iterator(SAVE_DIR)) {
        ifstream file(entry.path());
        string name;
        int lvl, score;
        if (file >> name >> lvl >> score) {
            ranking.push_back({ name, score });
        }
    }

    // Orden descendente por score
    sort(ranking.begin(), ranking.end(), [](auto& a, auto& b) {
        return a.second > b.second;
        });

    return ranking;
}
