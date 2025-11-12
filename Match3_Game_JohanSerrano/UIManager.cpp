#include "UIManager.h"

using namespace sf;
using std::string;
using std::pair;
using std::vector;
using namespace std;

UIManager::UIManager(const Font* f, const Texture& t) : font(f), gemTexture(t) {}

void UIManager::update(int currentScore, int remainingMoves, int currentLevel, Objective* obj) {
    score = currentScore;
    moves = remainingMoves;
    level = currentLevel;
    objective = obj;
}

void UIManager::draw(RenderWindow& window) {
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

void UIManager::drawHighScores(RenderWindow& window, const Font& font,
    const vector<pair<string, int>>& highscores) {
    const Texture& bgTex = ResourceManager::instance().getTexture("../assets/highScoresBackGround.png");
    Sprite background(bgTex);
    window.draw(background);

    RectangleShape panel(Vector2f(600.f, 350.f));
    panel.setFillColor(Color(255, 255, 255, 140));
    panel.setPosition(100.f, 130.f);
    window.draw(panel);

    Text title("HALL OF FAME", font, 38);
    title.setFillColor(Color::Black);
    title.setStyle(Text::Bold);
    FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.width / 2.f, tb.height / 2.f);
    title.setPosition(400.f, 165.f);
    window.draw(title);

    Text hPlayer("PLAYER", font, 26);
    Text hScore("SCORE", font, 26);
    hPlayer.setFillColor(Color::Black);
    hScore.setFillColor(Color::Black);
    hPlayer.setPosition(160.f, 230.f);
    hScore.setPosition(580.f, 230.f);
    window.draw(hPlayer);
    window.draw(hScore);

    float startY = 275.f;
    int count = std::min(5, (int)highscores.size());
    for (int i = 0; i < count; ++i) {
        const string& name = highscores[i].first;
        int score = highscores[i].second;

        Text nameText(name, font, 24);
        Text scoreText(std::to_string(score), font, 24);
        nameText.setFillColor(Color::Black);
        scoreText.setFillColor(Color::Black);
        nameText.setPosition(160.f, startY + i * 45.f);
        scoreText.setPosition(580.f, startY + i * 45.f);

        window.draw(nameText);
        window.draw(scoreText);
    }
}

void UIManager::drawGameWon(RenderWindow& window, const Font& font, const Player& player) {
    static float alpha = 0.f;
    if (alpha < 255.f) alpha += 3.f;

    RectangleShape stripe(Vector2f(800.f, 100.f));
    stripe.setFillColor(Color(0, 0, 128, (Uint8)alpha));
    stripe.setPosition(0.f, 0.f);
    window.draw(stripe);

    stripe.setFillColor(Color(255, 255, 255, (Uint8)alpha));
    stripe.setPosition(0.f, 100.f);
    window.draw(stripe);

    stripe.setFillColor(Color(200, 0, 0, (Uint8)alpha));
    stripe.setPosition(0.f, 200.f);
    window.draw(stripe);
    stripe.setPosition(0.f, 300.f);
    window.draw(stripe);

    stripe.setFillColor(Color(255, 255, 255, (Uint8)alpha));
    stripe.setPosition(0.f, 400.f);
    window.draw(stripe);

    stripe.setFillColor(Color(0, 0, 128, (Uint8)alpha));
    stripe.setPosition(0.f, 500.f);
    window.draw(stripe);

    std::string name = player.getName();
    for (auto& c : name)
        c = static_cast<char>(toupper(static_cast<unsigned char>(c)));

    Text title("FELICIDADES, " + name + "!", font, 50);
    title.setFillColor(Color::White);
    title.setOutlineColor(Color::Black);
    title.setOutlineThickness(4);
    FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.width / 2.f, tb.height / 2.f);
    title.setPosition(400.f, 160.f);
    window.draw(title);

    Text subtitle("Has completado todos los niveles!", font, 40);
    subtitle.setFillColor(Color::White);
    subtitle.setOutlineColor(Color::Black);
    subtitle.setOutlineThickness(2);
    FloatRect sb = subtitle.getLocalBounds();
    subtitle.setOrigin(sb.width / 2.f, sb.height / 2.f);
    subtitle.setPosition(400.f, 250.f);
    window.draw(subtitle);

    Text scoreText("Puntaje total: " + std::to_string(player.getScore()), font, 40);
    scoreText.setFillColor(Color::White);
    scoreText.setOutlineColor(Color::Black);
    scoreText.setOutlineThickness(2);
    FloatRect scb = scoreText.getLocalBounds();
    scoreText.setOrigin(scb.width / 2.f, scb.height / 2.f);
    scoreText.setPosition(400.f, 320.f);
    window.draw(scoreText);

    Text instr("Presiona R para reiniciar o ESC salir", font, 30);
    instr.setFillColor(Color::White);
    instr.setOutlineColor(Color::Black);
    instr.setOutlineThickness(2);
    FloatRect ib = instr.getLocalBounds();
    instr.setOrigin(ib.width / 2.f, ib.height / 2.f);
    instr.setPosition(400.f, 520.f);
    window.draw(instr);

    Text credits("Desarrollado por Johan Serrano Víctor", font, 30);
    credits.setFillColor(Color::White);
    credits.setOutlineColor(Color::Black);
    credits.setOutlineThickness(2);
    FloatRect cb = credits.getLocalBounds();
    credits.setOrigin(cb.width / 2.f, cb.height / 2.f);
    credits.setPosition(400.f, 570.f);
    window.draw(credits);
}
