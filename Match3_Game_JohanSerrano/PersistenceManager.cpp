#include "PersistenceManager.h"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <cctype>

using namespace std;
namespace fs = std::filesystem;

const std::string PersistenceManager::SAVE_DIR = "saves/";


string PersistenceManager::toUpper(const string& s) {
    string result = s;
    transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return static_cast<char>(toupper(c)); });
    return result;
}

void PersistenceManager::saveProgress(const Player& player) {
    try {
        fs::create_directories(SAVE_DIR);

        std::string safeName = toUpper(player.getName());
        std::string filename = SAVE_DIR + safeName + ".txt";

        // Cargar progreso previo (si existe)
        int oldLevel = 0, oldScore = 0;
        {
            std::ifstream in(filename);
            if (in.is_open()) {
                std::string n;
                in >> n >> oldLevel >> oldScore;
                in.close();
            }
        }

        int bestLevel = std::max(oldLevel, player.getLevel());
        int bestScore = std::max(oldScore, player.getScore());

        // Guardar nuevo progreso
        std::ofstream out(filename, std::ios::trunc);
        if (!out.is_open()) {
            throw std::runtime_error("[ERROR] No se pudo abrir " + filename + " para guardar progreso.");
        }

        out << safeName << " " << bestLevel << " " << bestScore << "\n";
        out.close();

        // Actualizar highscores
        PersistenceManager::updateHighScores(safeName, bestScore);

        std::cout << "[SAVE] Progreso guardado para " << safeName
            << ": Nivel=" << bestLevel << ", Score=" << bestScore << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

bool PersistenceManager::loadProgress(Player& player) {
    std::string safeName = toUpper(player.getName());
    std::string filename = SAVE_DIR + safeName + ".txt";

    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "[INFO] No existe progreso previo para " << safeName << std::endl;
            return false;
        }

        std::string name;
        int level = 0, score = 0;
        file >> name >> level >> score;
        file.close();

        player.setLevel(level);
        player.setScore(score);

        std::cout << "[LOAD] Progreso cargado: " << name
            << " Nivel=" << level << " Score=" << score << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "[ERROR] Error al cargar progreso: " << e.what() << std::endl;
        return false;
    }
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

void PersistenceManager::loadHighScores(std::vector<std::pair<std::string, int>>& outScores) {
    outScores.clear();

    // Si hay highscores.txt úsalo
    std::ifstream file("saves/highscores.txt");
    if (file.is_open()) {
        std::string name; int score;
        while (file >> name >> score) {
            outScores.emplace_back(toUpper(name), score);
        }
        file.close();

        std::sort(outScores.begin(), outScores.end(),
            [](const auto& a, const auto& b) { return a.second > b.second; });

        if (outScores.size() > 5) outScores.resize(5);
        return;
    }

    // Si NO existe highscores.txt, derivarlo del progreso por-jugador (sin “Player1 0”)
    std::cerr << "[WARN] No se encontró highscores.txt. Construyendo desde saves/*.txt...\n";
    auto ranking = PersistenceManager::loadRanking();   // lee todos los jugadores y sus scores
    if (ranking.size() > 5) ranking.resize(5);
    outScores = ranking;

    if (!outScores.empty()) {
        std::ofstream out("saves/highscores.txt", std::ios::trunc);
        if (out.is_open()) {
            for (auto& e : outScores) out << e.first << " " << e.second << "\n";
            out.close();
        }
    }
}

void PersistenceManager::updateHighScores(const std::string& playerName, int newScore) {
    Player tmp(playerName);
    tmp.setScore(newScore);
    // tmp.setLevel(bestLevel);
    PersistenceManager::updateHighScores(tmp);
}

void PersistenceManager::updateHighScores(const Player& player) {
    try {
        std::vector<std::pair<std::string, int>> highscores;

        // Leer archivo actual (si existe)
        std::ifstream file("saves/highscores.txt");
        if (file.is_open()) {
            std::string name; int score;
            while (file >> name >> score) {
                highscores.emplace_back(toUpper(name), score);
            }
            file.close();
        }

        const std::string upName = toUpper(player.getName());
        const int newScore = player.getScore();

        bool found = false;
        for (auto& e : highscores) {
            if (e.first == upName) {
                if (newScore > e.second) e.second = newScore;
                found = true;
                break;
            }
        }
        if (!found) highscores.emplace_back(upName, newScore);

        std::sort(highscores.begin(), highscores.end(),
            [](const auto& a, const auto& b) { return a.second > b.second; });

        if (highscores.size() > 5) highscores.resize(5);

        std::ofstream out("saves/highscores.txt", std::ios::trunc);
        if (!out.is_open()) {
            throw std::runtime_error("[ERROR] No se pudo abrir saves/highscores.txt para guardar.");
        }
        for (const auto& e : highscores) {
            out << e.first << " " << e.second << "\n";
        }
        out.close();

        std::cout << "[INFO] High Scores actualizados correctamente.\n";
        for (const auto& e : highscores) {
            std::cout << "   > " << e.first << ": " << e.second << "\n";
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

std::vector<Player> PersistenceManager::loadRankingPlayers() {
    std::vector<Player> ranking;
	int totalLevels = 14;
    try {
        if (!fs::exists(SAVE_DIR)) {
            std::cerr << "[WARN] Carpeta 'saves/' no existe, no hay ranking disponible.\n";
            return ranking;
        }

        for (const auto& entry : fs::directory_iterator(SAVE_DIR)) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                std::ifstream file(entry.path());
                std::string name;
                int level = 0, score = 0;

                if (file >> name >> level >> score) {
                    Player p(name);
                    p.setLevel(level);
                    p.setScore(score);
                    p.updateState(totalLevels); // A futuro levelManager.getTotalLevels()
                    ranking.push_back(p);
                }
            }
        }

        std::sort(ranking.begin(), ranking.end(),
            [](const Player& a, const Player& b) { return a.getScore() > b.getScore(); });

        if (ranking.size() > 5) ranking.resize(5);

        std::cout << "[INFO] Ranking de jugadores cargado correctamente ("
            << ranking.size() << " jugadores).\n";
    }
    catch (const std::exception& e) {
        std::cerr << "[ERROR] loadRankingPlayers(): " << e.what() << std::endl;
    }

    return ranking;
}