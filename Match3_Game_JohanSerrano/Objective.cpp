#include "Objective.h"

Objective::Objective(ObjectiveType aType, int aTarget, int aGemKind){

    type = aType;
    targetAmount = aTarget;
    gemKind = aGemKind;
    setDescription();
    
}

void Objective :: setDescription() {

    switch (type) {
    case ObjectiveType::CollectGems:
        description = "Collect " + to_string(targetAmount) + " gems";
        break;
    case ObjectiveType::ReachScore:
        description = "Reach " + to_string(targetAmount) + " points";
        break;
    case ObjectiveType::ClearObstacles:
        description = "Clear " + to_string(targetAmount) + " obstacles";
        break;
    }

}

void Objective::addProgress(int amount) {
    if (completed) return;

    currentAmount += amount;
    if (currentAmount >= targetAmount) {
        currentAmount = targetAmount;
        completed = true;
    }
}

bool Objective::isCompleted() const {
    return completed;
}

float Objective::getProgress() const {
    if (targetAmount == 0) return 1.0f;
    return static_cast<float>(currentAmount) / static_cast<float>(targetAmount);
}

string Objective::getDescription() const {
    return description;
}

int Objective::getCurrentAmount() const {
    return currentAmount;
}

int Objective::getTargetAmount() const {
    return targetAmount;
}

int Objective::getGemKind() const {
    return gemKind;
}

ObjectiveType Objective::getType() const {
    return type;
}

void Objective::reset() {
    currentAmount = 0;
    completed = false;
}
