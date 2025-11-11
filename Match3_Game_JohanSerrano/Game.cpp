#include "Game.h"
#include "ResourceManager.h"

Game::Game() {
    init();
}

Game::~Game() {
    if (uiManager) delete uiManager;
    if (ux) delete ux;
}

void Game::init() {
    cout << "[INIT] Cargando recursos iniciales..." << endl;

    try {
        font = ResourceManager::instance().getFont("assets/gameFont.ttf");
        const Texture& tex = ResourceManager::instance().getTexture("assets/spritesheet.png");

        uiManager = new UIManager(&font, tex);
        ux = new UXManager(&font, uiManager);

        ux->setScene(SceneType::MainMenu);
        cout << "[INIT] UXManager inicializado correctamente." << endl;
    }
    catch (const exception& e) {
        cerr << "[ERROR] Game::init() - No se pudieron cargar los recursos: " << e.what() << endl;
        running = false;
    }
}

void Game::run() {
    while (ux->isOpen() && running) {
        switch (state) {
        case GameState::MainMenu:      runMainMenu(); break;
        case GameState::LevelMap:      runLevelMap(); break;
        case GameState::Playing:       runGameLoop(); break;
        case GameState::LevelComplete: runLevelComplete(); break;
        case GameState::GameOver:      runGameOver(); break;
        case GameState::Exit:          running = false; break;
        }
    }
}

/* ===================== MAIN MENU ===================== */
void Game::runMainMenu() {
    ux->setScene(SceneType::MainMenu);
    auto& window = ux->getWindow();

    while (ux->isOpen() && state == GameState::MainMenu) {
        Event event;
        while (ux->pollGlobalEvent(event)) {
            if (event.type == Event::Closed) {
                ux->close();
                running = false;
            }

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2i mousePos = Mouse::getPosition(window);

                // START button (posiciones de ejemplo)
                if (IntRect(321, 547, 160, 48).contains(mousePos)) {
                    ux->startFade(Color::Black, 0.3f, false);
                    state = GameState::LevelMap;
                }

                // EXIT button
                if (IntRect(706, 45, 70, 32).contains(mousePos)) {
                    ux->close();
                    running = false;
                }
            }
        }

        ux->clear();
        ux->drawBackground("assets/mainMenu.png");
        ux->display();
    }
}

/* ===================== LEVEL MAP ===================== */
void Game::runLevelMap() {
    ux->setScene(SceneType::LevelMap);
    auto& window = ux->getWindow();

    cout << "[DEBUG] Entrando al LevelMapFinal..." << endl;

    while (ux->isOpen() && state == GameState::LevelMap) {
        Event event;
        while (ux->pollGlobalEvent(event)) {
            if (event.type == Event::Closed) {
                ux->close();
                running = false;
            }

            if (event.type == Event::MouseButtonPressed ) {
                Vector2i mousePos = Mouse::getPosition(window);
                cout << "[DEBUG] Click en LevelMap: (" << mousePos.x << ", " << mousePos.y << ")\n";

                // --- BOTÓN PLAY (coordenadas reales) ---
                if (IntRect(336, 543, 112, 44).contains(mousePos)) { // ancho = 448-336, alto = 587-543
                    cout << "[DEBUG] PLAY del LevelMap presionado. Iniciando nivel...\n";
                    startLevel();
                    state = GameState::Playing;
                }
            }

        }

        ux->clear();
        ux->drawBackground("assets/LevelsMapFinal.png");
        ux->display();
    }
}

/* ===================== GAMEPLAY ===================== */
void Game::startLevel() {
    cout << "[INFO] Iniciando nivel..." << endl;
    board.initialize();
    objective = Objective(ObjectiveType::CollectGems, 4, 2);
    moves = 20;
    score = 0;
    currentLevel = 1;
}

void Game::runGameLoop() {
    ux->setScene(SceneType::Gameplay);
    auto& window = ux->getWindow();

    Clock clock;

    while (ux->isOpen() && state == GameState::Playing) {
        Event e;
        while (ux->pollGlobalEvent(e)) {
            ux->handleBoardEvents(board, e); // aquí se integrará en el futuro
            if (e.type == Event::Closed) { ux->close(); running = false; }
        }

        float dt = clock.restart().asSeconds();

        int scoreGained = 0;
        bool moveConsumed = false;
        board.update(dt, scoreGained, moveConsumed);

        if (moveConsumed) moves--;
        score += scoreGained;

        uiManager->update(score, moves, currentLevel, &objective);

        ux->clear();
        drawCurrentScene();
        ux->display();

        if (moves <= 0) {
            state = GameState::GameOver;
        }
        else if (objective.isCompleted()) {
            state = GameState::LevelComplete;
        }
    }
}

/* ===================== LEVEL COMPLETE ===================== */
void Game::runLevelComplete() {
    ux->setScene(SceneType::LevelComplete);
    auto& window = ux->getWindow();

    while (ux->isOpen() && state == GameState::LevelComplete) {
        Event event;
        while (ux->pollGlobalEvent(event)) {
            if (event.type == Event::Closed) { ux->close(); running = false; }
            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2i mousePos = Mouse::getPosition(window);
                // Back to LevelMap
                if (IntRect(300, 500, 200, 80).contains(mousePos)) {
                    state = GameState::LevelMap;
                }
            }
        }

        ux->clear();
        ux->drawBackground("assets/levelcomplete.png");
        ux->display();
    }
}

/* ===================== GAME OVER ===================== */
void Game::runGameOver() {
    ux->setScene(SceneType::GameOver);
    auto& window = ux->getWindow();

    while (ux->isOpen() && state == GameState::GameOver) {
        Event event;
        while (ux->pollGlobalEvent(event)) {
            if (event.type == Event::Closed) { ux->close(); running = false; }

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2i mousePos = Mouse::getPosition(window);

                // Restart
                if (IntRect(320, 480, 200, 80).contains(mousePos)) {
                    startLevel();
                    state = GameState::Playing;
                }

                // Exit to map
                if (IntRect(560, 480, 200, 80).contains(mousePos)) {
                    state = GameState::LevelMap;
                }
            }
        }

        ux->clear();
        ux->drawBackground("assets/gameover.png");
        ux->display();
    }
}

/* ===================== DRAW ===================== */
void Game::drawCurrentScene() {
    switch (ux->getScene()) {
    case SceneType::Gameplay:
        ux->drawBackground("assets/background.png");
        ux->drawBoard(board);
        ux->drawGameUI(*uiManager);
        break;

    default:
        break;
    }
}
