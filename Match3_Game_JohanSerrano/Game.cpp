#include "Game.h"
#include "ResourceManager.h"

Game::Game() {
    init();
}

Game::~Game() {
    if (uiManager) delete uiManager;
    if (ux) delete ux;
}

/* ===================== INIT ===================== */
void Game::init() {
    cout << "[INIT] Cargando recursos iniciales..." << endl;

    //levelManager.initializeLevels();
    activeLevel = levelManager.getCurrentLevel();
    board.setCurrentLevel(activeLevel);

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

/* ===================== MAIN LOOP ===================== */
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

                // START
                if (IntRect(321, 547, 160, 48).contains(mousePos)) {
                    ux->startFade(Color::Black, 0.3f, false);
                    state = GameState::LevelMap;
                }

                // EXIT
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

    cout << "[DEBUG] Entrando al LevelsMapFinal..." << endl;

    while (ux->isOpen() && state == GameState::LevelMap) {
        Event event;
        while (ux->pollGlobalEvent(event)) {
            if (event.type == Event::Closed) {
                ux->close();
                running = false;
            }

            if (event.type == Event::MouseButtonPressed) {
                Vector2i mousePos = Mouse::getPosition(window);
                cout << "[DEBUG] Click en LevelMap: (" << mousePos.x << ", " << mousePos.y << ")\n";

                // PLAY
                if (IntRect(336, 543, 112, 44).contains(mousePos)) {
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
    activeLevel = levelManager.getCurrentLevel();
    if (!activeLevel) {
        cerr << "[ERROR] No se pudo obtener el nivel actual.\n";
        state = GameState::Exit;
        return;
    }

    // Primero se asigna el nivel actual al tablero
    board.setCurrentLevel(activeLevel);

    // Luego se inicializa el tablero usando ese nivel
    board.initialize();

    // Resto igual
    moves = activeLevel->getMaxMoves();
    score = 0;

    cout << "[INFO] Nivel " << activeLevel->getLevelNumber()
        << " con objetivo: " << activeLevel->getObjective()->getDescription() << endl;
}


void Game::runGameLoop() {
    ux->setScene(SceneType::Gameplay);
    auto& window = ux->getWindow();
    Clock clock;

    cout << "[DEBUG] Iniciando runGameLoop()..." << endl;

    while (ux->isOpen() && state == GameState::Playing) {
        Event e;
        while (ux->pollGlobalEvent(e)) {
            if (e.type == Event::Closed) {
                ux->close();
                running = false;
                return;
            }
            ux->handleBoardEvents(board, e);
        }

        float dt = clock.restart().asSeconds();
        int scoreGained = 0;
        bool moveConsumed = false;

        board.update(dt, scoreGained, moveConsumed);

        if (moveConsumed) {
            moves--;
            cout << "[DEBUG] Movimiento consumido. Restantes: " << moves << endl;
        }

        if (scoreGained > 0) {
            score += scoreGained;
            cout << "[DEBUG] +" << scoreGained << " puntos. Total: " << score << endl;

            board.updateScoreObjective(scoreGained);
        }

        // Protección contra nullptr 
        if (activeLevel) {
            uiManager->update(score, moves, activeLevel->getLevelNumber(), activeLevel->getObjective());
        }

        ux->clear();
        ux->drawBackground("assets/background.png");
        ux->drawBoard(board);
        ux->drawGameUI(*uiManager);
        ux->display();

        if (activeLevel && activeLevel->getObjective()->isCompleted()) {
            cout << "[INFO] Nivel completado correctamente." << endl;
            state = GameState::LevelComplete;
            break;
        }

        if (moves <= 0) {
            cout << "[INFO] Movimientos agotados. Fin del juego." << endl;
            state = GameState::GameOver;
            break;
        }
    }

    cout << "[DEBUG] runGameLoop() finalizado. Estado siguiente: ";
    switch (state) {
    case GameState::LevelComplete: cout << "LevelComplete"; break;
    case GameState::GameOver: cout << "GameOver"; break;
    default: cout << "Otro"; break;
    }
    cout << endl;
}

/* ===================== LEVEL COMPLETE ===================== */
void Game::runLevelComplete() {
    ux->setScene(SceneType::LevelComplete);
    auto& window = ux->getWindow();

    cout << "[DEBUG] Entrando a LevelComplete..." << endl;

    while (ux->isOpen() && state == GameState::LevelComplete) {
        Event event;
        while (ux->pollGlobalEvent(event)) {
            if (event.type == Event::Closed) {
                ux->close();
                running = false;
                return;
            }

            if (event.type == Event::MouseButtonPressed) {
                Vector2i mousePos = Mouse::getPosition(window);
                cout << "[DEBUG] Click detectado en LevelComplete (" << mousePos.x << ", " << mousePos.y << ")\n";

                // Avanzar al siguiente nivel antes de regresar al mapa
                if (levelManager.hasNextLevel()) {
                    levelManager.nextLevel();
                    activeLevel = levelManager.getCurrentLevel();
                    cout << "[INFO] Avanzando al nivel " << activeLevel->getLevelNumber() << endl;
                }
                else {
                    cout << "[INFO] Último nivel completado. Reiniciando niveles..." << endl;
                    levelManager.reset();
                    activeLevel = levelManager.getCurrentLevel();
                }

                state = GameState::LevelMap;
                return;
            }
        }

        ux->clear();
        ux->drawBackground("assets/background.png");

        RectangleShape overlay(Vector2f(800.f, 600.f));
        overlay.setFillColor(Color(0, 255, 0, 150));
        window.draw(overlay);

        Text msg("LEVEL COMPLETE!", font, 60);
        msg.setFillColor(Color::White);
        msg.setOutlineColor(Color::Black);
        msg.setOutlineThickness(3);
        msg.setPosition(180, 200);
        window.draw(msg);

        Text cont("Click anywhere to continue...", font, 36);
        cont.setFillColor(Color::Yellow);
        cont.setPosition(180, 320);
        window.draw(cont);

        ux->display();
    }
}

/* ===================== GAME OVER ===================== */
void Game::runGameOver() {
    ux->setScene(SceneType::GameOver);
    auto& window = ux->getWindow();

    cout << "[DEBUG] Entrando a GameOver..." << endl;

    while (ux->isOpen() && state == GameState::GameOver) {
        Event event;
        while (ux->pollGlobalEvent(event)) {
            if (event.type == Event::Closed) {
                ux->close();
                running = false;
                return;
            }

            if (event.type == Event::MouseButtonPressed) {
                Vector2i mousePos = Mouse::getPosition(window);
                cout << "[DEBUG] Click en GameOver: (" << mousePos.x << ", " << mousePos.y << ")\n";

                // Botón RESTART
                if (IntRect(190, 466, 186, 47).contains(mousePos)) {
                    cout << "[INFO] Reiniciando nivel actual..." << endl;
                    levelManager.resetCurrentLevel();
                    activeLevel = levelManager.getCurrentLevel();

                    if (activeLevel) {
                        activeLevel->getObjective()->reset();
                        board.initialize();
                        board.setCurrentLevel(activeLevel);
                        moves = activeLevel->getMaxMoves();
                        score = 0;
                    }

                    state = GameState::Playing;
                    return;
                }

                // Botón EXIT regresar al mapa
                if (IntRect(459, 466, 158, 43).contains(mousePos)) {
                    cout << "[INFO] Saliendo al mapa de niveles." << endl;
                    state = GameState::LevelMap;
                    return;
                }
            }
        }

        ux->clear();
        ux->drawBackground("assets/gameover.png");

        // Caja visual del score: (212–600, 256–402)
        FloatRect scoreBox(212.f, 256.f, 388.f, 146.f);
        Text scoreText("Score: " + to_string(score), font, 60);
        FloatRect textBounds = scoreText.getLocalBounds();
        scoreText.setOrigin(textBounds.width / 2, textBounds.height / 2);
        scoreText.setPosition(scoreBox.left + scoreBox.width / 2, scoreBox.top + scoreBox.height / 2);
        scoreText.setFillColor(Color::White);
        scoreText.setOutlineColor(Color::Black);
        scoreText.setOutlineThickness(3);
        window.draw(scoreText);

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
