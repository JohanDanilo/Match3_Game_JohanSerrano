#pragma once
#include <string>
#include <vector>
#include "Player.h"

class PersistenceManager {
public:
    
    static const std::string SAVE_DIR;

    static std::string toUpper(const std::string& s);

    static void saveProgress(const Player& player);
    static bool loadProgress(Player& player);

    static std::vector<std::pair<std::string, int>> loadRanking();
    static void loadHighScores(std::vector<std::pair<std::string, int>>& outScores);

    static void updateHighScores(const std::string& playerName, int newScore);
    static void updateHighScores(const Player& player);
    static std::vector<Player> loadRankingPlayers();
};
