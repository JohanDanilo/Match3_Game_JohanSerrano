#include "Game.h"

Game::Game() : score(0), moves(20), state(GameState::MainMenu), running(true) {
    loadResources();
}

void Game::init() {
    loadResources();
    run();
}

void Game::loadResources() {
    if (!font.loadFromFile("assets/gameFont.ttf")) {
        cerr << "Error loading font\n";
    }

    if (!mainMenuTexture.loadFromFile("assets/mainMenu.png")) {
        cerr << "Error loading mainMenu.png\n";
    }
    else mainMenuSprite.setTexture(mainMenuTexture);

    if (!backgroundTexture.loadFromFile("assets/background.png")) {
        cerr << "Error loading background.png\n";
    }
    else backgroundSprite.setTexture(backgroundTexture);

    if (!gameOverTexture.loadFromFile("assets/gameOver.png")) {
        cerr << "Error loading gameOver.png\n";
    }
    else gameOverSprite.setTexture(gameOverTexture);
}

void Game::run() {
    while (running) {
        switch (state) {
        case GameState::MainMenu:
            runMainMenu();
            break;
        case GameState::Playing:
            runGameLoop();
            break;
        case GameState::GameOver:
            runGameOver();
            break;
        case GameState::LevelComplete:
            runLevelComplete();
            break;
        }
    }
}

void Game::runMainMenu() {
    RenderWindow window(VideoMode(800, 600), "Main Menu");
    window.setFramerateLimit(144);

    IntRect startButton(321, 547, 160, 48);
    IntRect exitButton(706, 45, 70, 32);

    while (window.isOpen() && state == GameState::MainMenu) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
                running = false;
            }

            if (event.type == Event::MouseButtonPressed) {
                Vector2i mousePos = Mouse::getPosition(window);
                if (startButton.contains(mousePos)) {
                    state = GameState::Playing;
                    startLevel();
                    window.close();
                }
                else if (exitButton.contains(mousePos)) {
                    window.close();
                    running = false;
                }
            }
        }
        window.clear();
        window.draw(mainMenuSprite);
        window.display();
    }
}

void Game::startLevel() {
    Level* currentLevel = levelManager.getCurrentLevel();
    if (currentLevel) {
        board.initialize();
        board.setCurrentLevel(currentLevel);
        board.placeObstacles(currentLevel->getObstacleCount());
        moves = currentLevel->getMaxMoves();
        clearScore();
    }
}

void Game::runGameLoop() {
    RenderWindow window(VideoMode(800, 600), "Match-3 Game!");
    window.setFramerateLimit(144);

    Text scoreText, movesText, levelText;

    scoreText.setFont(font);
    scoreText.setCharacterSize(40);
    scoreText.setFillColor(Color::White);
    scoreText.setOutlineColor(Color::Black);
    scoreText.setOutlineThickness(2);
    scoreText.setPosition(70, 8);

    movesText.setFont(font);
    movesText.setCharacterSize(40);
    movesText.setFillColor(Color::White);
    movesText.setOutlineColor(Color::Black);
    movesText.setOutlineThickness(2);
    movesText.setPosition(550, 8);

    levelText.setFont(font);
    levelText.setCharacterSize(40);
    levelText.setFillColor(Color::White);
    levelText.setOutlineColor(Color::Black);
    levelText.setOutlineThickness(2);
    levelText.setPosition(340, 8);

    Clock clock;

    while (window.isOpen() && state == GameState::Playing) {
        float deltaTime = clock.restart().asSeconds();

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
                running = false;
            }

            if (event.type == Event::MouseButtonPressed && isClickInsideBoard(window)) {
                selectGem(window);
            }
        }

        int scoreGained = 0;
        bool moveConsumed = false;
        board.update(deltaTime, scoreGained, moveConsumed);

        if (scoreGained > 0) {
            score += scoreGained;
            board.updateScoreObjective(scoreGained);
        }
        if (moveConsumed) { moves--; }

        // Actualizar textos
        scoreText.setString("Score: " + to_string(score));
        movesText.setString("Moves: " + to_string(moves));
        levelText.setString("Level " + to_string(levelManager.getCurrentLevelNumber()));

        window.clear();
        window.draw(backgroundSprite);
        board.draw(window);
        window.draw(scoreText);
        window.draw(movesText);
        window.draw(levelText);

        // Dibujar panel de objetivos
        drawObjectivesPanel(window);

        window.display();

        // Verificar si se completó el nivel
        if (checkLevelCompletion()) {
            state = GameState::LevelComplete;
            window.close();
        }
        // Verificar si se acabaron los movimientos
        else if (moves <= 0 && board.getState() == 0) {
            state = GameState::GameOver;
            window.close();
        }
    }
}

void Game::drawObjectivesPanel(RenderWindow& window) {
    Level* currentLevel = levelManager.getCurrentLevel();
    if (!currentLevel) return;

    Objective* objective = currentLevel->getObjective();
    if (!objective) return;

    // Panel de fondo
    RectangleShape panel(Vector2f(300, 60));
    panel.setPosition(8, 535);
    panel.setFillColor(Color(19, 14, 59, 160));
    panel.setOutlineColor(Color(0, 0, 0));
    panel.setOutlineThickness(2);
    window.draw(panel);

    // Título
    Text title;
    title.setFont(font);
    title.setString("Objective");
    title.setCharacterSize(40);
    title.setFillColor(Color::Cyan);
    title.setOutlineColor(Color::Black);
    title.setOutlineThickness(2);
    title.setPosition(10, 520);
    window.draw(title);

    // Texto del objetivo
    Text objText;
    objText.setFont(font);
    objText.setCharacterSize(40);
    objText.setOutlineColor(Color::Black);
    objText.setOutlineThickness(2);
    objText.setPosition(10, 540);

    int kind = objective->getGemKind();
    Texture target;

    string objStr = "";
    Color textColor = Color::White;

    switch (objective->getType()) {
    case ObjectiveType::CollectGems:
        objStr = "Collect Gems: ";
        break;
    case ObjectiveType::ReachScore:
        objStr = "Score: ";
        break;
    case ObjectiveType::ClearObstacles:
        objStr = "Clear Iron: ";
        break;
    }

    objStr += to_string(objective->getCurrentAmount()) + "/" + to_string(objective->getTargetAmount());

    if (objective->isCompleted()) {
        textColor = Color::Green;
        objStr += " OK";
    }

    objText.setString(objStr);
    objText.setFillColor(textColor);
    window.draw(objText);

    // Barra de progreso
    RectangleShape progressBg(Vector2f(150, 10));
    progressBg.setPosition(12, 580);
    progressBg.setFillColor(Color(50, 50, 50));
    window.draw(progressBg);

    float progress = objective->getProgress();
    RectangleShape progressBar(Vector2f(150 * progress, 10));
    progressBar.setPosition(12, 580);
    progressBar.setFillColor(objective->isCompleted() ? Color::Green : Color::Cyan);
    window.draw(progressBar);
}

bool Game::checkLevelCompletion() {
    Level* currentLevel = levelManager.getCurrentLevel();
    if (!currentLevel) return false;

    return currentLevel->isLevelComplete();
}

void Game::runLevelComplete() {
    RenderWindow window(VideoMode(800, 600), "Level Complete!");
    window.setFramerateLimit(144);

    // Fondo semi-transparente
    RectangleShape bg(Vector2f(800, 600));
    bg.setFillColor(Color(0, 100, 0, 200));

    Text congratsText, scoreDisplayText;

    congratsText.setFont(font);
    congratsText.setString("LEVEL COMPLETE!");
    congratsText.setCharacterSize(50);
    congratsText.setFillColor(Color::Yellow);
    congratsText.setOutlineColor(Color::Black);
    congratsText.setOutlineThickness(3);
    congratsText.setPosition(180, 150);

    scoreDisplayText.setFont(font);
    scoreDisplayText.setString("Final Score: " + to_string(score));
    scoreDisplayText.setCharacterSize(35);
    scoreDisplayText.setFillColor(Color::White);
    scoreDisplayText.setOutlineColor(Color::Black);
    scoreDisplayText.setOutlineThickness(2);
    scoreDisplayText.setPosition(250, 250);

    // Botones
    RectangleShape nextButton(Vector2f(200, 50));
    nextButton.setPosition(300, 400);
    nextButton.setFillColor(Color(0, 200, 0));
    nextButton.setOutlineColor(Color::White);
    nextButton.setOutlineThickness(3);

    RectangleShape quitButton(Vector2f(200, 50));
    quitButton.setPosition(300, 470);
    quitButton.setFillColor(Color(200, 0, 0));
    quitButton.setOutlineColor(Color::White);
    quitButton.setOutlineThickness(3);

    Text nextButtonText, quitButtonText;
    nextButtonText.setFont(font);
    quitButtonText.setFont(font);

    bool hasNext = levelManager.hasNextLevel();

    if (hasNext) {
        nextButtonText.setString("Next Level");
        nextButtonText.setCharacterSize(30);
        nextButtonText.setFillColor(Color::White);
        nextButtonText.setPosition(330, 410);
    }
    else {
        nextButtonText.setString("Play Again");
        nextButtonText.setCharacterSize(30);
        nextButtonText.setFillColor(Color::White);
        nextButtonText.setPosition(330, 410);
    }

    quitButtonText.setString("Main Menu");
    quitButtonText.setCharacterSize(30);
    quitButtonText.setFillColor(Color::White);
    quitButtonText.setPosition(335, 480);

    bool waiting = true;
    while (window.isOpen() && waiting) {
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) {
                window.close();
                running = false;
                waiting = false;
            }

            if (e.type == Event::MouseButtonPressed) {
                Vector2i mousePos = Mouse::getPosition(window);

                // Click en botón Next/Play Again
                if (nextButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    if (hasNext) {
                        levelManager.nextLevel();
                        state = GameState::Playing;
                        startLevel();
                    }
                    else {
                        // Reiniciar desde nivel 1
                        levelManager = LevelManager();
                        state = GameState::Playing;
                        startLevel();
                    }
                    window.close();
                    waiting = false;
                }
                // Click en botón Quit
                else if (quitButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    state = GameState::MainMenu;
                    levelManager = LevelManager();
                    window.close();
                    waiting = false;
                }
            }
        }

        window.clear();
        window.draw(backgroundSprite);
        window.draw(bg);
        window.draw(congratsText);
        window.draw(scoreDisplayText);
        window.draw(nextButton);
        window.draw(quitButton);
        window.draw(nextButtonText);
        window.draw(quitButtonText);
        window.display();
    }
}

void Game::runGameOver() {
    RenderWindow window(VideoMode(800, 600), "Game Over");

    Text finalScore;
    finalScore.setFont(font);
    finalScore.setCharacterSize(60);
    finalScore.setFillColor(Color::White);
    finalScore.setOutlineColor(Color::Black);
    finalScore.setOutlineThickness(2);
    finalScore.setPosition(224, 297);
    finalScore.setString("Final Score: " + to_string(score));

    IntRect restartButton(190, 467, 193, 50);
    IntRect quitButton(454, 467, 193, 50);

    bool restart = false;

    while (window.isOpen()) {
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) {
                window.close();
                running = false;
            }

            if (e.type == Event::MouseButtonPressed) {
                Vector2i mousePos = Mouse::getPosition(window);
                if (restartButton.contains(mousePos)) {
                    restart = true;
                    window.close();
                }
                else if (quitButton.contains(mousePos)) {
                    window.close();
                }
            }
        }
        window.clear();
        window.draw(gameOverSprite);
        window.draw(finalScore);
        window.display();
    }

    if (restart) {
        levelManager.resetCurrentLevel();
        startLevel();
        state = GameState::Playing;
    }
    else {
        state = GameState::MainMenu;
        levelManager = LevelManager();
    }
}

Vector2f Game::getClickPosition(RenderWindow& window) {
    Vector2i position = Mouse::getPosition(window);
    return window.mapPixelToCoords(position);
}

bool Game::isClickInsideBoard(RenderWindow& window) {
    Vector2f pos = getClickPosition(window);
    int col = static_cast<int>((pos.x - offset.x) / TILE_SIZE);
    int row = static_cast<int>((pos.y - offset.y) / TILE_SIZE);
    return (row >= 0 && row < ROWS && col >= 0 && col < COLS);
}

void Game::selectGem(RenderWindow& window) {
    Vector2f pos = getClickPosition(window);
    int col = static_cast<int>((pos.x - offset.x) / TILE_SIZE);
    int row = static_cast<int>((pos.y - offset.y) / TILE_SIZE);
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS) { return; }

    if (board.getState() != 0) { return; }

    if (firstSelectedRow == -1) {
        firstSelectedRow = row;
        firstSelectedCol = col;
    }
    else {
        if (firstSelectedRow == row && firstSelectedCol == col) {
            firstSelectedRow = firstSelectedCol = -1;
            return;
        }
        bool started = board.trySwapIndices(firstSelectedRow, firstSelectedCol, row, col);
        if (started) {
            firstSelectedRow = firstSelectedCol = -1;
        }
        else {
            firstSelectedRow = firstSelectedCol = -1;
        }
    }
}

void Game::refillMoves() {
    Level* currentLevel = levelManager.getCurrentLevel();
    if (currentLevel) {
        moves = currentLevel->getMaxMoves();
    }
    else {
        moves = 20;
    }
}

void Game::clearScore() {
    score = 0;
}