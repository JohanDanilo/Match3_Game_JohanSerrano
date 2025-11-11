#pragma once
#include <string>
#include <vector>
#include "Level.h"

using namespace std;

class PersistenceManager {
public:
    static void saveProgress(const string& playerName, int level, int score);
    static bool loadProgress(const string& playerName, int& level, int& score);
    static vector<pair<string, int>> loadRanking();
};
