#include "UXManager.h"
#include "Board.h"

UXManager::UXManager(const Font* f, UIManager* ui)
    : font(f), uiManager(ui), currentScene(SceneType::MainMenu)
{
    cout << "[DEBUG] UXManager inicializado." << endl;

    if (!window.isOpen()) {
        window.create(VideoMode(800, 600), "Costa Rican Enchanted Gems", Style::Default);
        window.setFramerateLimit(144);
    }

    fadeRect.setSize(Vector2f(800, 600));
    fadeRect.setFillColor(Color::Transparent);
}


UXManager::~UXManager() {
    if (window.isOpen()) {
        window.close();
        cout << "[DEBUG] Ventana cerrada correctamente en UXManager." << endl;
    }
}


RenderWindow& UXManager::getWindow() { return window; }
bool UXManager::isOpen() const { return window.isOpen(); }
void UXManager::close() { window.close(); }
void UXManager::clear(const Color& color) { window.clear(color); }

void UXManager::display() {
    if (fadeActive) updateFade();
    window.display();
}


void UXManager::setScene(SceneType scene) {
    currentScene = scene;
    cout << "[DEBUG] Escena cambiada a: ";
    switch (scene) {
    case SceneType::MainMenu:      cout << "MainMenu"; break;
    case SceneType::LevelMap:      cout << "LevelMap"; break;
    case SceneType::Gameplay:      cout << "Gameplay"; break;
    case SceneType::LevelComplete: cout << "LevelComplete"; break;
    case SceneType::GameOver:      cout << "GameOver"; break;
    case SceneType::HighScores:    cout << "HighScores"; break;
    case SceneType::GameWon:       cout << "GameWon"; break;
    }
    cout << endl;
}

SceneType UXManager::getScene() const {
    return currentScene;
}

void UXManager::drawBackground(const string& textureName) {
    try {
        const Texture& bgTex = ResourceManager::instance().getTexture(textureName);
        Sprite bgSprite;
        bgSprite.setTexture(bgTex);
        window.draw(bgSprite);
    }
    catch (const exception& e) {
        cerr << "[ERROR] UXManager::drawBackground() - No se pudo cargar "
            << textureName << ". Motivo: " << e.what() << endl;
    }
}


void UXManager::startFade(Color color, float duration, bool fadeInEffect) {
    fadeColor = color;
    fadeDuration = duration;
    fadeIn = fadeInEffect;
    fadeActive = true;
    fadeClock.restart();

    if (fadeIn) fadeRect.setFillColor(Color(color.r, color.g, color.b, 255));
    else fadeRect.setFillColor(Color(color.r, color.g, color.b, 0));
}

void UXManager::updateFade() {
    float elapsed = fadeClock.getElapsedTime().asSeconds();
    float alpha = 0.f;

    if (fadeIn) {
        alpha = 255.f - (elapsed / fadeDuration) * 255.f;
        if (alpha < 0.f) { alpha = 0.f; fadeActive = false; }
    }
    else {
        alpha = (elapsed / fadeDuration) * 255.f;
        if (alpha > 255.f) { alpha = 255.f; fadeActive = false; }
    }

    fadeRect.setFillColor(Color(fadeColor.r, fadeColor.g, fadeColor.b, static_cast<Uint8>(alpha)));
    drawFade();
}

void UXManager::drawFade() {
    if (fadeActive) window.draw(fadeRect);
}

bool UXManager::pollGlobalEvent(Event& e) {
    return window.pollEvent(e);
}


void UXManager::handleBoardEvents(Board& board, Event& e) {
    if (e.type != Event::MouseButtonPressed) return;

    Vector2i mousePos = Mouse::getPosition(window);
    float x = mousePos.x - offset.x;
    float y = mousePos.y - offset.y;

    if (x < 0 || y < 0 || x >= TILE_SIZE * COLS || y >= TILE_SIZE * ROWS) {
        return;
    }

    int col = static_cast<int>(x / TILE_SIZE);
    int row = static_cast<int>(y / TILE_SIZE);

    if (board.getState() != 0) return;

    

    board.handleGemClick(row, col);
}


void UXManager::drawBoard(Board& board) {
    board.draw(window);
}

void UXManager::drawGameUI(UIManager& ui) {
    ui.draw(window);
}
