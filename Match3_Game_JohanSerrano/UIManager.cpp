#include "UIManager.h"
using namespace sf;
using namespace std;

UIManager::UIManager(const Font* f, const Texture& t) : font(f), gemTexture(t) {}

void UIManager::update(int currentScore, int remainingMoves, int currentLevel, Objective* obj) {
    score = currentScore;
    moves = remainingMoves;
    level = currentLevel;
    objective = obj;
}

void UIManager::draw(RenderWindow& window) {
    // === HUD SUPERIOR ===
    Text scoreText("Score: " + to_string(score), *font, 40);
    scoreText.setPosition(headerPosScore);
    scoreText.setFillColor(Color::White);
    scoreText.setOutlineColor(Color::Black);
    scoreText.setOutlineThickness(2);
    window.draw(scoreText);

    Text movesText("Moves: " + to_string(moves), *font, 40);
    movesText.setPosition(headerPosMoves);
    movesText.setFillColor(Color::White);
    movesText.setOutlineColor(Color::Black);
    movesText.setOutlineThickness(2);
    window.draw(movesText);

    Text levelText("Level " + to_string(level), *font, 40);
    levelText.setPosition(headerPosLevel);
    levelText.setFillColor(Color::White);
    levelText.setOutlineColor(Color::Black);
    levelText.setOutlineThickness(2);
    window.draw(levelText);

    // === PANEL DE OBJETIVO ===
    if (!objective) return;

    RectangleShape panel(Vector2f(310, 60));
    panel.setPosition(panelPos);
    panel.setFillColor(Color(19, 14, 59, 160));
    panel.setOutlineColor(Color::Black);
    panel.setOutlineThickness(2);
    window.draw(panel);

    Text title("Objective", *font, 40);
    title.setPosition(titlePos);
    title.setFillColor(Color::Cyan);
    title.setOutlineColor(Color::Black);
    title.setOutlineThickness(2);
    window.draw(title);

    string objStr;
    Color textColor = Color::White;

    switch (objective->getType()) {
    case ObjectiveType::CollectGems: objStr = "Collect Gems: "; break;
    case ObjectiveType::ReachScore:  objStr = "Score: "; break;
    case ObjectiveType::ClearObstacles: objStr = "Clear Iron: "; break;
    }

    objStr += to_string(objective->getCurrentAmount()) + "/" + to_string(objective->getTargetAmount());
    if (objective->isCompleted()) {
        objStr += " OK";
        textColor = Color::Green;
    }

    Text objText(objStr, *font, 40);
    objText.setPosition(objTextPos);
    objText.setFillColor(textColor);
    objText.setOutlineColor(Color::Black);
    objText.setOutlineThickness(2);
    window.draw(objText);

    // === GEM TARGET ===
    if (objective->getType() == ObjectiveType::CollectGems) {
        int gemKind = objective->getGemKind();

        Text targetLabel("Target:", *font, 40);
        targetLabel.setPosition(targetLabelPos);
        targetLabel.setFillColor(Color::White);
        targetLabel.setOutlineColor(Color::Black);
        targetLabel.setOutlineThickness(2);
        window.draw(targetLabel);

        RectangleShape gemFrame(Vector2f(60, 60));
        gemFrame.setPosition(gemFramePos);
        gemFrame.setFillColor(Color(50, 50, 50, 180));
        gemFrame.setOutlineColor(Color::Cyan);
        gemFrame.setOutlineThickness(2);
        window.draw(gemFrame);

        Sprite gemSprite;
        gemSprite.setTexture(gemTexture);
        gemSprite.setTextureRect(IntRect(gemKind * 47, 0, 47, 54));
        gemSprite.setPosition(gemSpritePos);
        window.draw(gemSprite);
    }

    // === PROGRESS BAR ===
    RectangleShape progressBg(Vector2f(150, 10));
    progressBg.setPosition(progressBarPos);
    progressBg.setFillColor(Color(50, 50, 50));
    window.draw(progressBg);

    float progress = objective->getProgress();
    RectangleShape progressBar(Vector2f(150 * progress, 10));
    progressBar.setPosition(progressBarPos);
    progressBar.setFillColor(objective->isCompleted() ? Color::Green : Color::Cyan);
    window.draw(progressBar);
}
