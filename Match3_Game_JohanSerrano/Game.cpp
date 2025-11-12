#include "Game.h"
#include "ResourceManager.h"
#include "Player.h"
#include "PersistenceManager.h"

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
        font = ResourceManager::instance().getFont("../assets/gameFont.ttf");
        const Texture& tex = ResourceManager::instance().getTexture("../assets/spritesheet.png");

        uiManager = new UIManager(&font, tex);
        ux = new UXManager(&font, uiManager);
        ux->setScene(SceneType::MainMenu);

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
        case GameState::HighScores:    runHighScores(); break;
		case GameState::GameWon:       runGameWon(); break;
        case GameState::Exit:          running = false; break;
        }
    }
}

/* ===================== MAIN MENU ===================== */
void Game::runMainMenu() {
    ux->setScene(SceneType::MainMenu);
    auto& window = ux->getWindow();

    // Siempre inicia mostrando solo el menú
    bool enteringName = false;
    std::string tempName = "";

    // --- Overlay oscuro cuando se ingresa el nombre ---
    RectangleShape overlay(Vector2f(800.f, 600.f));
    overlay.setFillColor(Color(0, 0, 0, 150)); // Negro translúcido

    // --- Texto “Enter your name” ---
    Text namePrompt("ENTER YOUR NAME:", font, 50);
    namePrompt.setFillColor(Color::White);
    namePrompt.setOutlineColor(Color::Black);
    namePrompt.setOutlineThickness(4);
    FloatRect npb = namePrompt.getLocalBounds();
    namePrompt.setOrigin(npb.width / 2.f, npb.height / 2.f);
    namePrompt.setPosition(400.f, 220.f);

    // --- Campo donde se escribe el nombre ---
    Text nameInput("", font, 50);
    nameInput.setFillColor(Color::White);
    nameInput.setOutlineColor(Color::Black);
    nameInput.setOutlineThickness(4);
    nameInput.setPosition(260.f, 290.f);

    // --- Texto inferior de ayuda ---
    Text tip("Press ENTER to continue", font, 35);
    tip.setFillColor(Color::White);
    tip.setOutlineColor(Color::Black);
    tip.setOutlineThickness(2);
    FloatRect tp = tip.getLocalBounds();
    tip.setOrigin(tp.width / 2.f, tp.height / 2.f);
    tip.setPosition(400.f, 500.f);

    while (ux->isOpen() && state == GameState::MainMenu) {
        Event event;
        while (ux->pollGlobalEvent(event)) {
            if (event.type == Event::Closed) {
                ux->close();
                running = false;
                return;
            }

            // --- Solo si está ingresando nombre ---
            if (enteringName && event.type == Event::TextEntered) {
                if (event.text.unicode == '\b' && !tempName.empty()) {
                    tempName.pop_back();
                }
                else if (event.text.unicode == '\r' && !tempName.empty()) {
                    currentPlayer = Player(tempName);
                    cout << "[INFO] Nombre ingresado: " << currentPlayer.getName() << endl;

                    // Cargar o crear progreso
                    if (!PersistenceManager::loadProgress(currentPlayer)) {
                        PersistenceManager::saveProgress(currentPlayer);
                    }

                    currentPlayer.updateState(levelManager.getTotalLevels());
                    levelManager.setCurrentLevelIndex(currentPlayer.getLevel() - 1);
                    syncLevelIconsByProgress();

                    enteringName = false;
                    state = GameState::LevelMap;
                    return;
                }
                else if (event.text.unicode < 128 && event.text.unicode >= 32) {
                    tempName += static_cast<char>(event.text.unicode);
                }
            }

            // --- Botones del menú principal ---
            if (!enteringName && event.type == Event::MouseButtonPressed) {
                Vector2i mousePos = Mouse::getPosition(window);

                // Botón PLAY (posición de tu imagen)
                if (IntRect(321, 547, 160, 48).contains(mousePos)) {
                    enteringName = true; // recién aquí se activa la captura de nombre
                    tempName.clear();
                }

                // Botón EXIT
                if (IntRect(706, 45, 70, 32).contains(mousePos)) {
                    ux->close();
                    running = false;
                    return;
                }
            }
        }

        // --- Renderizado ---
        ux->clear();
        ux->drawBackground("../assets/mainMenu.png");

        if (enteringName) {
            window.draw(overlay);
            window.draw(namePrompt);
            nameInput.setString(tempName + "_");
            window.draw(nameInput);
            window.draw(tip);
        }

        ux->display();
    }
}


/* ===================== LEVEL MAP ===================== */
void Game::runLevelMap() {
    ux->setScene(SceneType::LevelMap);
    auto& window = ux->getWindow();

    if (!levelMapUIInitialized)
        initLevelMapUI();

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

                if (IntRect(336, 543, 112, 44).contains(mousePos)) {
                    startLevel();
                    state = GameState::Playing;
                    return;
                }

                if (highScoresButton.getGlobalBounds().contains((Vector2f)mousePos)) {
                    cout << "[UI] Mostrando High Scores..." << endl;
                    state = GameState::HighScores;
                    return;
                }

                if (exitButton.getGlobalBounds().contains((Vector2f)mousePos)) {
                    ux->close();
                    running = false;
                    return;
                }
            }
        }

        ux->clear();
        ux->drawBackground("../assets/LevelsMapFinal.png");

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

        int current = levelManager.getCurrentLevelNumber() - 1;
        current = std::clamp(current, 0, (int)levelIcons.size() - 1);

        Vector2f pos = levelIcons[current].position;
        cursor.setPosition(pos.x - 75.f, pos.y - 20.f);
        window.draw(cursor);

        window.draw(highScoresButton);
        window.draw(exitButton);
        ux->display();
    }
}

/* ===================== HIGH SCORES ===================== */
void Game::runHighScores() {
    ux->setScene(SceneType::HighScores);
    auto& window = ux->getWindow();

    const Texture& backTex = ResourceManager::instance().getTexture("../assets/back.png");
    const Texture& bgTex = ResourceManager::instance().getTexture("../assets/highScoresBackGround.png");

    Sprite background(bgTex);
    Sprite backButton(backTex);
    backButton.setPosition(50.f, 40.f);

    RectangleShape panel(Vector2f(600.f, 350.f));
    panel.setFillColor(Color(255, 255, 255, 140));
    panel.setPosition(100.f, 130.f);

    Text title("COSTA RICAN ENCHANTED GEMS", font, 42);
    title.setFillColor(Color::Black);
    title.setStyle(Text::Bold);
    FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.width / 2.f, tb.height / 2.f);
    title.setPosition(400.f, 168.f);

    Text subtitle("HALL OF FAME", font, 34);
    subtitle.setFillColor(Color::Black);
    subtitle.setStyle(Text::Bold);
    FloatRect sb = subtitle.getLocalBounds();
    subtitle.setOrigin(sb.width / 2.f, sb.height / 2.f);
    subtitle.setPosition(400.f, 205.f);

    Text playerHeader("PLAYER", font, 26);
    playerHeader.setFillColor(Color::Black);
    playerHeader.setStyle(Text::Bold);
    playerHeader.setPosition(140.f, 240.f);

    Text scoreHeader("SCORE", font, 26);
    scoreHeader.setFillColor(Color::Black);
    scoreHeader.setStyle(Text::Bold);
    scoreHeader.setPosition(370.f, 240.f);

    Text stateHeader("STATE", font, 26);
    stateHeader.setFillColor(Color::Black);
    stateHeader.setStyle(Text::Bold);
    stateHeader.setPosition(580.f, 240.f);

    std::vector<Player> highscores = PersistenceManager::loadRankingPlayers();

    std::vector<Text> scoreTexts;
    float startY = 285.f;
    int displayCount = std::min(5, (int)highscores.size());

    for (int i = 0; i < displayCount; ++i) {
        const Player& p = highscores[i];

        Text nameText(p.getName(), font, 24);
        nameText.setFillColor(Color::Black);
        nameText.setPosition(140.f, startY + i * 50.f);

        Text scoreText(std::to_string(p.getScore()), font, 24);
        scoreText.setFillColor(Color::Black);
        scoreText.setPosition(370.f, startY + i * 50.f);

        Text stateText(p.getState(), font, 24);
        stateText.setFillColor(Color::Black);
        stateText.setPosition(580.f, startY + i * 50.f);

        scoreTexts.push_back(nameText);
        scoreTexts.push_back(scoreText);
        scoreTexts.push_back(stateText);
    }

    while (ux->isOpen() && state == GameState::HighScores) {
        Event event;
        while (ux->pollGlobalEvent(event)) {
            if (event.type == Event::Closed) {
                ux->close();
                running = false;
                return;
            }

            if (event.type == Event::MouseButtonPressed) {
                Vector2i mousePos = Mouse::getPosition(window);
                if (backButton.getGlobalBounds().contains((Vector2f)mousePos)) {
                    state = GameState::LevelMap;
                    return;
                }
            }
        }

        ux->clear();
        window.draw(background);
        window.draw(panel);
        window.draw(title);
        window.draw(subtitle);
        window.draw(playerHeader);
        window.draw(scoreHeader);
        window.draw(stateHeader);

        for (auto& t : scoreTexts)
            window.draw(t);

        window.draw(backButton);
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

        if (activeLevel)
            uiManager->update(score, moves, activeLevel->getLevelNumber(), activeLevel->getObjective());

        ux->clear();
        ux->drawBackground("../assets/background.png");
        ux->drawBoard(board);
        ux->drawGameUI(*uiManager);
        ux->display();

        if (activeLevel && activeLevel->getObjective()->isCompleted()) {
            currentPlayer.setScore(currentPlayer.getScore() + score);
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

                // Si hay más niveles, pasa al siguiente
                if (levelManager.hasNextLevel()) {
                    levelManager.nextLevel();

                    currentPlayer.setLevel(levelManager.getCurrentLevelNumber());
                    currentPlayer.setScore(currentPlayer.getScore() + score);
                    currentPlayer.updateState(levelManager.getTotalLevels());

                    PersistenceManager::saveProgress(currentPlayer);
                    syncLevelIconsByProgress();
                    activeLevel = levelManager.getCurrentLevel();
                    state = GameState::LevelMap;
                }
                else {
                    // Si ya era el último nivel, mostrar pantalla de victoria
                    currentPlayer.setScore(currentPlayer.getScore() + score);
                    currentPlayer.updateState(levelManager.getTotalLevels());
                    PersistenceManager::saveProgress(currentPlayer);

                    cout << "[INFO] ¡Todos los niveles completados! Mostrando pantalla de victoria..." << endl;
                    state = GameState::GameWon;
                }

                return;
            }
        }

        ux->clear();
        ux->drawBackground("../assets/background.png");

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
                    PersistenceManager::saveProgress(currentPlayer); // ? guarda progreso
                    state = GameState::LevelMap;
                    return;
                }
            }
        }

        ux->clear();
        ux->drawBackground("../assets/gameover.png");

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

void Game::runGameWon() {
    ux->setScene(SceneType::GameWon);
    auto& window = ux->getWindow();

    while (ux->isOpen() && state == GameState::GameWon) {
        Event e;
        while (ux->pollGlobalEvent(e)) {
            if (e.type == Event::Closed) {
                ux->close();
                running = false;
                return;
            }
            if (e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::R) {
                    levelManager.reset();
                    currentPlayer.setLevel(1);
                    PersistenceManager::saveProgress(currentPlayer);
                    syncLevelIconsByProgress();
                    state = GameState::LevelMap;
                    return;
                }
                if (e.key.code == Keyboard::Escape) {
                    ux->close();
                    running = false;
                    return;
                }
            }
        }

        // Render centralizado (UIManager dibuja la escena)
        drawCurrentScene();
    }
}


/* ===================== LEVEL MAP UI ===================== */
void Game::initLevelMapUI() {
    try {
        const Texture& padlockTex = ResourceManager::instance().getTexture("../assets/padlock.png");
        const Texture& checkTex = ResourceManager::instance().getTexture("../assets/completedLevel.png");
        const Texture& cursorTex = ResourceManager::instance().getTexture("../assets/cursor.png");
        const Texture& hsTex = ResourceManager::instance().getTexture("../assets/highScores.png");
        const Texture& exitTex = ResourceManager::instance().getTexture("../assets/exit.png");

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
    int nextToPlay = levelManager.getCurrentLevelNumber();
    nextToPlay = std::clamp(nextToPlay, 1, (int)levelIcons.size());

    for (size_t i = 0; i < levelIcons.size(); ++i) {
        levelIcons[i].completed = false;
        levelIcons[i].unlocked = false;

        int levelNumber = (int)i + 1;

        if (levelNumber < nextToPlay)
            levelIcons[i].completed = true;
        else if (levelNumber == nextToPlay)
            levelIcons[i].unlocked = true;
    }
}


void Game::drawCurrentScene() {
    auto& window = ux->getWindow();
    ux->clear();

    switch (state) {
    case GameState::Playing: {
        ux->drawBackground("../assets/background.png");
        ux->drawBoard(board);
        ux->drawGameUI(*uiManager);
        break;
    }
    case GameState::HighScores: {
        // UIManager dibuja TODO el contenido de la escena
        uiManager->drawHighScores(window, font, highscoreCache);
        break;
    }
    case GameState::GameWon: {
        uiManager->drawGameWon(window, font, currentPlayer);
        break;
    }
    default:
        // otras escenas siguen con su propio render existente
        break;
    }

    ux->drawFade();
    ux->display();
}

