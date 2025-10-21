#pragma once
#include <string>
using namespace std;

enum class ObjectiveType {
    CollectGems,
    ReachScore,
    ClearObstacles
};

class Objective {
private:
    ObjectiveType type;
    int targetAmount;
    int currentAmount = 0;
    int gemKind;
    string description;
    bool completed = false;

public:
    Objective(ObjectiveType aType, int aTarget, int aGemKind);

    void setDescription();

    void addProgress(int amount = 1);
    bool isCompleted() const;
    float getProgress() const;  // Retorna 0.0 a 1.0

    string getDescription() const;
    int getCurrentAmount() const;
    int getTargetAmount() const;
    int getGemKind() const;
    ObjectiveType getType() const;

    void reset();
};

