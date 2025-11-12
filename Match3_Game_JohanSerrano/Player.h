#pragma once
#include <string>
#include <fstream>
#include <iostream>

class Player {
private:
    std::string name;
    int level;
    int score;
    std::string state;

public:
    Player(const std::string& playerName = "", int lvl = 1, int scr = 0)
        : name(playerName), level(lvl), score(scr), state("In Progress") {
    }

    std::string getName() const { return name; }
    int getLevel() const { return level; }
    int getScore() const { return score; }
    std::string getState() const { return state; }

    void setLevel(int lvl) { level = lvl; }
    void setScore(int scr) { score = scr; }
    void setState(const std::string& st) { state = st; }

    void updateState(int totalLevels) {
        state = (level >= totalLevels) ? "Completed" : "In Progress";
    }

    bool loadFromFile() {
        std::ifstream in("saves/" + name + ".txt");
        if (!in.is_open()) {
            std::cerr << "[WARN] No se pudo abrir archivo de jugador: " << name << "\n";
            return false;
        }
        in >> name >> level >> score;
        in.close();
        return true;
    }

    void saveToFile() const {
        std::ofstream out("saves/" + name + ".txt", std::ios::trunc);
        if (!out.is_open()) {
            std::cerr << "[ERROR] No se pudo guardar progreso de: " << name << "\n";
            return;
        }
        out << name << " " << level << " " << score << "\n";
        out.close();
    }

    void printInfo() const {
        std::cout << "Player: " << name << " | Level: " << level
            << " | Score: " << score << " | State: " << state << std::endl;
    }
};
