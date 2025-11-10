#include "Game.h"
#include "ResourceManager.h"
#include "UIManager.h"

using namespace sf;
using namespace std;

Game::Game() : score(0), moves(20), state(GameState::MainMenu), running(true) { }

void Game::init() {
    cout << "[DEBUG] Iniciando Game::init()" << endl;

    try {
        loadResources();
    }
    catch (const exception& e) {
        cerr << "[ERROR] loadResources(): " << e.what() << endl;
        throw;
    }

    try {
        ResourceManager& rm = ResourceManager::instance();

        cout << "[DEBUG] Cargando texturas principales..." << endl;
        mainMenuTexture = &rm.getTexture("assets/mainMenu.png");
        backgroundTexture = &rm.getTexture("assets/background.png");
        gameOverTexture = &rm.getTexture("assets/gameOver.png");

        mainMenuSprite.setTexture(*mainMenuTexture);
        backgroundSprite.setTexture(*backgroundTexture);
        gameOverSprite.setTexture(*gameOverTexture);

        cout << "[DEBUG] Cargando fuente y spritesheet..." << endl;
        font = &rm.getFont("assets/gameFont.ttf");
        const Texture& gemTex = rm.getTexture("assets/spritesheet.png");

        uiManager = new UIManager(font, gemTex);
        cout << "[DEBUG] UIManager creado correctamente." << endl;
    }
    catch (const exception& e) {
        cerr << "[ERROR] Asignando texturas o creando UIManager: " << e.what() << endl;
        throw;
    }

    Level* currentLevel = levelManager.getCurrentLevel();
    if (currentLevel) {
        cout << "[DEBUG] Inicializando primer nivel..." << endl;
        board.initialize();
        board.setCurrentLevel(currentLevel);
        board.placeObstacles(currentLevel->getObstacleCount());
        moves = currentLevel->getMaxMoves();
        clearScore();
    }

    cout << "[DEBUG] Game::init() completado exitosamente." << endl;
}

void Game::loadResources() {
    cout << "[DEBUG] Precargando recursos..." << endl;
    ResourceManager& rm = ResourceManager::instance();

    rm.getTexture("assets/background.png");
    rm.getTexture("assets/spritesheet.png");
    rm.getTexture("assets/gameOver.png");
    rm.getTexture("assets/mainMenu.png");
    rm.getFont("assets/gameFont.ttf");

    cout << "[DEBUG] Recursos precargados en memoria." << endl;
}

void Game::run() {
    cout << "[DEBUG] Entrando en bucle principal Game::run()" << endl;

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

    if (uiManager) {
        delete uiManager;
        uiManager = nullptr;
        cout << "[DEBUG] UIManager eliminado correctamente." << endl;
    }

    cout << "[DEBUG] Saliendo de Game::run()" << endl;
}

/* ===================== MAIN MENU ===================== */

void Game::runMainMenu() {
    cout << "[DEBUG] Entrando a runMainMenu()" << endl;

    if (!mainMenuTexture) {
        cerr << "[ERROR] mainMenuTexture es null!" << endl;
        running = false;
        return;
    }

    cout << "[DEBUG] Textura del menu cargada: "
        << mainMenuTexture->getSize().x << "x" << mainMenuTexture->getSize().y << endl;

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
                cout << "[DEBUG] Click en: " << mousePos.x << ", " << mousePos.y << endl;

                if (startButton.contains(mousePos)) {
                    cout << "[DEBUG] Botón START presionado" << endl;
                    state = GameState::Playing;
                    startLevel();
                    window.close();
                }
                else if (exitButton.contains(mousePos)) {
                    cout << "[DEBUG] Botón EXIT presionado" << endl;
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

/* ===================== START LEVEL ===================== */

void Game::startLevel() {
    cout << "[DEBUG] Iniciando nuevo nivel..." << endl;

    Level* currentLevel = levelManager.getCurrentLevel();
    if (currentLevel) {
        board.initialize();
        board.setCurrentLevel(currentLevel);
        board.placeObstacles(currentLevel->getObstacleCount());
        moves = currentLevel->getMaxMoves();
        clearScore();

        cout << "[DEBUG] Nivel iniciado con " << moves << " movimientos." << endl;
    }
}

/* ===================== GAME LOOP ===================== */

void Game::runGameLoop() {
    cout << "[DEBUG] Entrando a runGameLoop()" << endl;

    RenderWindow window(VideoMode(800, 600), "Match-3 Game!");
    window.setFramerateLimit(144);

    Clock clock;

    while (window.isOpen() && state == GameState::Playing) {
        float deltaTime = clock.restart().asSeconds();

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) { window.close(); running = false; }
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

        // === Dibujo ===
        window.clear();
        window.draw(backgroundSprite);
        board.draw(window);

        uiManager->update(score, moves, levelManager.getCurrentLevelNumber(),
            levelManager.getCurrentLevel() ? levelManager.getCurrentLevel()->getObjective() : nullptr);
        uiManager->draw(window);

        window.display();

        // === Cambios de estado ===
        if (moves <= 0 && board.getState() == 0) {
            cout << "[DEBUG] Fin de movimientos, verificando nivel..." << endl;
            if (checkLevelCompletion()) { state = GameState::LevelComplete; }
            else { state = GameState::GameOver; }
            window.close();
        }
        else if (checkLevelCompletion()) {
            cout << "[DEBUG] Nivel completado, cambiando estado." << endl;
            state = GameState::LevelComplete;
            window.close();
        }
    }

    cout << "[DEBUG] Saliendo de runGameLoop()" << endl;
}

/* ===================== CHECK LEVEL COMPLETION ===================== */

bool Game::checkLevelCompletion() {
    Level* currentLevel = levelManager.getCurrentLevel();
    if (!currentLevel) return false;
    return currentLevel->isLevelComplete();
}

/* ===================== LEVEL COMPLETE ===================== */

void Game::runLevelComplete() {
    cout << "[DEBUG] Ejecutando runLevelComplete()" << endl;

    RenderWindow window(VideoMode(800, 600), "Level Complete!");
    window.setFramerateLimit(144);

    RectangleShape bg(Vector2f(800, 600));
    bg.setFillColor(Color(0, 100, 0, 200));

    Text congratsText, scoreDisplayText;
    congratsText.setFont(*font);
    congratsText.setString("LEVEL COMPLETE!");
    congratsText.setCharacterSize(50);
    congratsText.setFillColor(Color::Yellow);
    congratsText.setOutlineColor(Color::Black);
    congratsText.setOutlineThickness(3);
    congratsText.setPosition(180, 150);

    scoreDisplayText.setFont(*font);
    scoreDisplayText.setString("Final Score: " + to_string(score));
    scoreDisplayText.setCharacterSize(35);
    scoreDisplayText.setFillColor(Color::White);
    scoreDisplayText.setOutlineColor(Color::Black);
    scoreDisplayText.setOutlineThickness(2);
    scoreDisplayText.setPosition(250, 250);

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
    nextButtonText.setFont(*font);
    quitButtonText.setFont(*font);

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
            if (e.type == Event::Closed) { window.close(); running = false; waiting = false; }

            if (e.type == Event::MouseButtonPressed) {
                Vector2i mousePos = Mouse::getPosition(window);
                if (nextButton.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y)) {
                    if (hasNext) {
                        levelManager.nextLevel();
                        state = GameState::Playing;
                        startLevel();
                    }
                    else {
                        board.clearCurrentLevel();
                        levelManager.reset();
                        state = GameState::Playing;
                        startLevel();
                    }
                    window.close(); waiting = false;
                }
                else if (quitButton.getGlobalBounds().contains((float)mousePos.x, (float)mousePos.y)) {
                    board.clearCurrentLevel();
                    levelManager.reset();
                    state = GameState::MainMenu;
                    window.close(); waiting = false;
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

    cout << "[DEBUG] Saliendo de runLevelComplete()" << endl;
}

/* ===================== GAME OVER ===================== */

void Game::runGameOver() {
    cout << "[DEBUG] Ejecutando runGameOver()" << endl;

    RenderWindow window(VideoMode(800, 600), "Game Over");
    Text finalScore;
    finalScore.setFont(*font);
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
            if (e.type == Event::Closed) { window.close(); running = false; }
            if (e.type == Event::MouseButtonPressed) {
                Vector2i mousePos = Mouse::getPosition(window);
                if (restartButton.contains(mousePos)) { restart = true; window.close(); }
                else if (quitButton.contains(mousePos)) { window.close(); }
            }
        }
        window.clear();
        window.draw(gameOverSprite);
        window.draw(finalScore);
        window.display();
    }

    if (restart) {
        cout << "[DEBUG] Reiniciando nivel..." << endl;
        levelManager.resetCurrentLevel();
        startLevel();
        state = GameState::Playing;
    }
    else {
        running = false;
    }

    cout << "[DEBUG] Saliendo de runGameOver()" << endl;
}

/* ===================== INPUT HELPERS ===================== */

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
        firstSelectedRow = row; firstSelectedCol = col;
    }
    else {
        if (firstSelectedRow == row && firstSelectedCol == col) {
            firstSelectedRow = firstSelectedCol = -1;
            return;
        }
        bool started = board.trySwapIndices(firstSelectedRow, firstSelectedCol, row, col);
        firstSelectedRow = firstSelectedCol = -1;
        (void)started;
    }
}

void Game::refillMoves() {
    Level* currentLevel = levelManager.getCurrentLevel();
    if (currentLevel) { moves = currentLevel->getMaxMoves(); }
    else { moves = 20; }
}

void Game::clearScore() { score = 0; }
