#include "Game.h"
#include "ResourceManager.h"

/* ===================== CONSTRUCTOR / DESTRUCTOR ===================== */
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

    try {
        font = ResourceManager::instance().getFont("assets/gameFont.ttf");
        const Texture& tex = ResourceManager::instance().getTexture("assets/spritesheet.png");

        uiManager = new UIManager(&font, tex);
        ux = new UXManager(&font, uiManager);
        ux->setScene(SceneType::MainMenu);

        // Intentar cargar progreso guardado
        if (PersistenceManager::loadProgress(playerName, currentLevelIndex, totalScore)) {
            cout << "[LOAD] Progreso cargado correctamente:\n";
            cout << "Jugador: " << playerName << " | Nivel: " << currentLevelIndex << " | Total Score: " << totalScore << endl;
            levelManager.setCurrentLevelIndex(currentLevelIndex);
        }
        else {
            cout << "[LOAD] No se encontró progreso previo. Iniciando nuevo jugador." << endl;
            currentLevelIndex = 0;
        }

        activeLevel = levelManager.getCurrentLevel();
        board.setCurrentLevel(activeLevel);

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

    bool enteringName = playerName.empty(); // Solo se pide si no hay jugador
    std::string tempName = "";

    Text namePrompt("Enter your name:", font, 40);
    namePrompt.setFillColor(Color::White);
    namePrompt.setPosition(200, 200);

    Text nameInput("", font, 40);
    nameInput.setFillColor(Color::White);
    nameInput.setPosition(200, 260);

    RectangleShape overlay(Vector2f(800.f, 600.f));
    overlay.setFillColor(Color(0, 255, 255, 100)); // layout cian translúcido

    while (ux->isOpen() && state == GameState::MainMenu) {
        Event event;
        while (ux->pollGlobalEvent(event)) {
            if (event.type == Event::Closed) {
                ux->close();
                running = false;
                return;
            }

            // Ingreso de nombre
            if (enteringName && event.type == Event::TextEntered) {
                if (event.text.unicode == '\b' && !tempName.empty()) {
                    tempName.pop_back(); // retroceso
                }
                else if (event.text.unicode == '\r') { // ENTER
                    if (!tempName.empty()) {
                        playerName = tempName;
                        cout << "[INFO] Nombre ingresado: " << playerName << endl;

                        int savedLevel = 1, savedScore = 0;

                        // Si ya existe archivo cargar progreso
                        if (!PersistenceManager::loadProgress(playerName, savedLevel, savedScore)) {
                            PersistenceManager::saveProgress(playerName, 1, 0);
                        }

                        // Sincronizar progreso con LevelManager
                        levelManager.setCurrentLevelIndex(savedLevel - 1);

                        enteringName = false;

                        // Avanzar directo al mapa de niveles
                        state = GameState::LevelMap;
                        return;
                    }
                }
                else if (event.text.unicode < 128 && event.text.unicode >= 32) {
                    tempName += static_cast<char>(event.text.unicode);
                }
            }

            // Botones del menú (solo si no estamos ingresando nombre)
            if (!enteringName && event.type == Event::MouseButtonPressed) {
                Vector2i mousePos = Mouse::getPosition(window);

                // START
                if (IntRect(321, 547, 160, 48).contains(mousePos)) {
                    ux->startFade(Color::Black, 0.3f, false);
                    state = GameState::LevelMap;
                    return;
                }

                // EXIT
                if (IntRect(706, 45, 70, 32).contains(mousePos)) {
                    ux->close();
                    running = false;
                    return;
                }
            }
        }

        // Dibujo de escena
        ux->clear();
        ux->drawBackground("assets/mainMenu.png");

        if (enteringName) {
            window.draw(overlay);
            window.draw(namePrompt);
            nameInput.setString(tempName + "_");
            window.draw(nameInput);
        }

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

                if (IntRect(336, 543, 112, 44).contains(mousePos)) {
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

    board.setCurrentLevel(activeLevel);
    board.initialize();
    moves = activeLevel->getMaxMoves();
    score = 0;

    cout << "[INFO] Nivel " << activeLevel->getLevelNumber()
        << " con objetivo: " << activeLevel->getObjective()->getDescription() << endl;
}

void Game::runGameLoop() {
    ux->setScene(SceneType::Gameplay);
    auto& window = ux->getWindow();
    Clock clock;

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

        if (moveConsumed) moves--;
        if (scoreGained > 0) {
            score += scoreGained;
            board.updateScoreObjective(scoreGained);
        }

        if (activeLevel) {
            uiManager->update(score, moves, activeLevel->getLevelNumber(), activeLevel->getObjective());
        }

        ux->clear();
        ux->drawBackground("assets/background.png");
        ux->drawBoard(board);
        ux->drawGameUI(*uiManager);
        ux->display();

        // Cuando se completa el nivel
        if (activeLevel && activeLevel->getObjective()->isCompleted()) {
            totalScore += score;

            // Guarda progreso y puntuación acumulada
            PersistenceManager::saveProgress(
                playerName,
                levelManager.getCurrentLevelNumber(),
                totalScore
            );

            state = GameState::LevelComplete;
            break;
        }

        if (moves <= 0) {
            state = GameState::GameOver;
            break;
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
            if (event.type == Event::Closed) {
                ux->close();
                running = false;
                return;
            }

            if (event.type == Event::MouseButtonPressed) {
                if (levelManager.hasNextLevel()) {
                    levelManager.nextLevel();
                }
                else {
                    levelManager.reset();
                }
                activeLevel = levelManager.getCurrentLevel();
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

                if (IntRect(190, 466, 186, 47).contains(mousePos)) {
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

                if (IntRect(459, 466, 158, 43).contains(mousePos)) {
                    state = GameState::LevelMap;
                    return;
                }
            }
        }

        ux->clear();
        ux->drawBackground("assets/gameover.png");

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
    if (ux->getScene() == SceneType::Gameplay) {
        ux->drawBackground("assets/background.png");
        ux->drawBoard(board);
        ux->drawGameUI(*uiManager);
    }
}
