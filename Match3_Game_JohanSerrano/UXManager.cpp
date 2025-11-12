#include "UXManager.h"
#include "Board.h"

UXManager::UXManager(const Font* f, UIManager* ui)
    : font(f), uiManager(ui), currentScene(SceneType::MainMenu)
{
    cout << "[DEBUG] UXManager inicializado." << endl;

    // Inicializar correctamente la ventana
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

/* ===================== GESTIÓN DE VENTANA ===================== */

RenderWindow& UXManager::getWindow() { return window; }
bool UXManager::isOpen() const { return window.isOpen(); }
void UXManager::close() { window.close(); }
void UXManager::clear(const Color& color) { window.clear(color); }

void UXManager::display() {
    if (fadeActive) updateFade();
    window.display();
}

/* ===================== ESCENAS ===================== */

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

/* ===================== EFECTOS VISUALES ===================== */

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

/* ===================== EVENTOS GLOBALES ===================== */
bool UXManager::pollGlobalEvent(Event& e) {
    return window.pollEvent(e);
}

/* ===================== EXPERIENCIA DE JUEGO ===================== */

// Traducir eventos del mouse/teclado hacia acciones del Board
// UXManager.cpp
void UXManager::handleBoardEvents(Board& board, Event& e) {
    // Solo procesar clics del tipo presionado (sin importar el botón)
    if (e.type != Event::MouseButtonPressed) return;

    Vector2i mousePos = Mouse::getPosition(window);
    float x = mousePos.x - offset.x;
    float y = mousePos.y - offset.y;

    // 1? Validar límites del área del tablero
    if (x < 0 || y < 0 || x >= TILE_SIZE * COLS || y >= TILE_SIZE * ROWS) {
        return; // clic fuera del tablero ? ignorar
    }

    int col = static_cast<int>(x / TILE_SIZE);
    int row = static_cast<int>(y / TILE_SIZE);

    // 2? Validar estado del tablero
    if (board.getState() != 0) return; // solo cuando está Idle

    

    // 4? Si todo está bien, manejar el clic
    board.handleGemClick(row, col);
}




// Dibuja el tablero (solo visual)
void UXManager::drawBoard(Board& board) {
    board.draw(window);
}

// Dibuja el HUD/UI (marcadores, metas, etc.)
void UXManager::drawGameUI(UIManager& ui) {
    ui.draw(window);
}
