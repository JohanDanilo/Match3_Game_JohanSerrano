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

        // No se carga progreso aquí, se hace correctamente en runMainMenu()

        activeLevel = levelManager.getCurrentLevel();
        board.setCurrentLevel(activeLevel);

        cout << "[INIT] UXManager inicializado correctamente." << endl;
    }
    catch (const exception& e) {
        cerr << "[ERROR] Game::init() - No se pudieron cargar los recursos: " << e.what() << endl;
        running = false;
    }

    initLevelMapUI();
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

    bool enteringName = playerName.empty();
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
                    tempName.pop_back();
                }
                else if (event.text.unicode == '\r') { // ENTER
                    if (!tempName.empty()) {
                        playerName = tempName;
                        cout << "[INFO] Nombre ingresado: " << playerName << endl;

                        int savedLevel = 1, savedScore = 0;

                        // Si ya existe archivo, cargar progreso; si no, crearlo nuevo
                        if (!PersistenceManager::loadProgress(playerName, savedLevel, savedScore)) {
                            PersistenceManager::saveProgress(playerName, 1, 0);
                        }

                        // Sincronizar progreso con LevelManager
                        levelManager.setCurrentLevelIndex(savedLevel-1);
                        totalScore = savedScore;

                        syncLevelIconsByProgress();

                        enteringName = false;
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

    if (!levelMapUIInitialized) {
        initLevelMapUI();
    }

    syncLevelIconsByProgress();

    cout << "[DEBUG] Entrando a LevelsMapFinal..." << endl;

    while (ux->isOpen() && state == GameState::LevelMap) {
        Event event;
        while (ux->pollGlobalEvent(event)) {
            if (event.type == Event::Closed) {
                ux->close();
                running = false;
                return;
            }

            if (event.type == Event::MouseButtonPressed) {
                Vector2i mousePos = Mouse::getPosition(window);

                // --- Botón PLAY (zona exacta del fondo) ---
                if (IntRect(336, 543, 112, 44).contains(mousePos)) {
                    startLevel();
                    state = GameState::Playing;
                    return;
                }

                // --- Botón HIGH SCORES ---
                if (highScoresButton.getGlobalBounds().contains((Vector2f)mousePos)) {
                    cout << "[UI] High Scores presionado." << endl;
                }

                // --- Botón EXIT ---
                if (exitButton.getGlobalBounds().contains((Vector2f)mousePos)) {
                    ux->close();
                    running = false;
                    return;
                }
            }
        }

        // --- Renderizado ---
        ux->clear();
        ux->drawBackground("assets/LevelsMapFinal.png");

        // Dibuja candados y checks
        for (size_t i = 0; i < levelIcons.size(); ++i) {
            if (levelIcons[i].completed) {
                levelIcons[i].checkSprite.setPosition(levelIcons[i].position);
                window.draw(levelIcons[i].checkSprite);
            }
            else if (!levelIcons[i].unlocked) {
                levelIcons[i].lockSprite.setPosition(levelIcons[i].position);
                window.draw(levelIcons[i].lockSprite);
            }
        }

        // Cursor del nivel actual (a la izquierda del nivel desbloqueado)
        int current = levelManager.getCurrentLevelNumber() - 1;
        if (current < 0) current = 0;
        if (current >= (int)levelIcons.size()) current = (int)levelIcons.size() - 1;

        Vector2f pos = levelIcons[current].position;
        cursor.setPosition(pos.x - 75.f, pos.y - 20.f);
        window.draw(cursor);

        // Botones fijos
        window.draw(highScoresButton);
        window.draw(exitButton);

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
                // Avanzar al siguiente nivel o reiniciar al primero si terminó todos
                if (levelManager.hasNextLevel()) {
                    levelManager.nextLevel();
                }
                else {
                    levelManager.reset(); // reinicia a nivel 1 si ya completó todos
                }

                // Obtener el siguiente nivel a jugar (ya actualizado tras nextLevel)
                int nextToPlay = levelManager.getCurrentLevelNumber();

                // Guardar progreso actualizado (próximo nivel a jugar)
                PersistenceManager::saveProgress(
                    playerName,
                    nextToPlay,
                    totalScore
                );

                // Actualizar iconos visuales (candados, checks y cursor)
                syncLevelIconsByProgress();

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

/* ===================== LEVEL MAP UI INIT ===================== */
void Game::initLevelMapUI() {
    try {
        const Texture& padlockTex = ResourceManager::instance().getTexture("assets/padlock.png");
        const Texture& checkTex = ResourceManager::instance().getTexture("assets/completedLevel.png");
        const Texture& cursorTex = ResourceManager::instance().getTexture("assets/cursor.png");
        const Texture& hsTex = ResourceManager::instance().getTexture("assets/highScores.png");
        const Texture& exitTex = ResourceManager::instance().getTexture("assets/exit.png");

        highScoresButton.setTexture(hsTex);
        highScoresButton.setPosition(680.f, 542.f);

        exitButton.setTexture(exitTex);
        exitButton.setPosition(30.f, 542.f);

        cursor.setTexture(cursorTex);

        vector<Vector2f> centers = {
            {441.f, 511.f}, {385.f, 485.f}, {336.f, 458.f}, {296.f, 431.f},
            {258.f, 404.f}, {226.f, 374.f}, {205.f, 345.f}, {232.f, 318.f},
            {278.f, 295.f}, {322.f, 281.f}, {359.f, 267.f}, {383.f, 249.f},
            {353.f, 235.f}, {371.f, 208.f}
        };

        levelIcons.clear();
        levelIcons.resize(centers.size());

        for (size_t i = 0; i < centers.size(); ++i) {
            levelIcons[i].position = centers[i];
            levelIcons[i].lockSprite.setTexture(padlockTex);
            levelIcons[i].checkSprite.setTexture(checkTex);

            FloatRect lb = levelIcons[i].lockSprite.getLocalBounds();
            levelIcons[i].lockSprite.setOrigin(lb.width / 2.f, lb.height / 2.f);

            FloatRect cb = levelIcons[i].checkSprite.getLocalBounds();
            levelIcons[i].checkSprite.setOrigin(cb.width / 2.f, cb.height / 2.f);
        }

        syncLevelIconsByProgress();
        levelMapUIInitialized = true;
    }
    catch (const exception& e) {
        cerr << "[ERROR] Game::initLevelMapUI - " << e.what() << endl;
    }
}

void Game::syncLevelIconsByProgress() {
    // nextToPlay es 1-based (nivel visible que debe jugarse)
    int nextToPlay = levelManager.getCurrentLevelNumber();
    if (nextToPlay < 1) { nextToPlay = 1; }
    if (nextToPlay > static_cast<int>(levelIcons.size())) {
        nextToPlay = static_cast<int>(levelIcons.size());
    }

    for (size_t i = 0; i < levelIcons.size(); ++i) {
        // Reinicio explícito de banderas en cada iteración
        levelIcons[i].completed = false;
        levelIcons[i].unlocked = false;

        int levelNumber = static_cast<int>(i) + 1; // 1-based para comparar

        if (levelNumber < nextToPlay) {
            // Niveles estrictamente anteriores al próximo a jugar -> completados
            levelIcons[i].completed = true;
        }
        else if (levelNumber == nextToPlay) {
            // Este es el próximo a jugar -> desbloqueado (cursor)
            levelIcons[i].unlocked = true;
        }
        // else: niveles posteriores -> bloqueados (sin flags)
    }
}