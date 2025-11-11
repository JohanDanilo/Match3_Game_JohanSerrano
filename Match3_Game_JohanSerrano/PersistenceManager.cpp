#include "PersistenceManager.h"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <cctype>

using namespace std;
namespace fs = filesystem;

const string SAVE_DIR = "saves/";

string PersistenceManager::toUpper(const string& s) {
    string result = s;
    transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return static_cast<char>(toupper(c)); });
    return result;
}

void PersistenceManager::saveProgress(const string& playerName, int level, int score) {
    try {
        if (playerName.empty()) throw runtime_error("Nombre de jugador vacío.");

        fs::create_directory(SAVE_DIR);
        string safeName = toUpper(playerName);
        string filename = SAVE_DIR + safeName + ".txt";

        ofstream file(filename, ios::trunc);
        if (!file.is_open()) throw runtime_error("No se pudo abrir el archivo del jugador.");

        file << safeName << " " << level << " " << score << endl;
        file.close();

        cout << "[SAVE] Progreso guardado para " << safeName
            << ": Nivel " << level << ", Score " << score << endl;
    }
    catch (const exception& e) {
        cerr << "[ERROR] PersistenceManager::saveProgress - " << e.what() << endl;
    }
}

bool PersistenceManager::loadProgress(const string& playerName, int& level, int& score) {
    string safeName = toUpper(playerName);
    string filename = SAVE_DIR + safeName + ".txt";

    ifstream file(filename);
    if (!file.is_open()) {
        cout << "[INFO] No existe progreso previo para " << safeName << endl;
        return false;
    }

    string name;
    file >> name >> level >> score;
    file.close();

    cout << "[LOAD] Progreso cargado: " << name
        << " Nivel=" << level << " Score=" << score << endl;
    return true;
}

vector<pair<string, int>> PersistenceManager::loadRanking() {
    vector<pair<string, int>> ranking;
    if (!fs::exists(SAVE_DIR)) return ranking;

    for (const auto& entry : fs::directory_iterator(SAVE_DIR)) {
        ifstream file(entry.path());
        string name;
        int lvl, score;
        if (file >> name >> lvl >> score) {
            ranking.push_back({ toUpper(name), score });
        }
    }

    sort(ranking.begin(), ranking.end(),
        [](auto& a, auto& b) { return a.second > b.second; });

    return ranking;
}
