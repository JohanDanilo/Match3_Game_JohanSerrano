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
    } else mainMenuSprite.setTexture(mainMenuTexture);

    if (!backgroundTexture.loadFromFile("assets/background.png")) {
        cerr << "Error loading background.png\n";
    } else backgroundSprite.setTexture(backgroundTexture);

    if (!gameOverTexture.loadFromFile("assets/gameOver.png")) {
        cerr << "Error loading gameOver.png\n";
    } else gameOverSprite.setTexture(gameOverTexture);
}

void Game::run() {
    while (running) {
        switch (state) {
        case GameState::MainMenu: runMainMenu(); break;
        case GameState::Playing: runGameLoop(); break;
        case GameState::GameOver: runGameOver(); break;
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
            if (event.type == Event::Closed) { window.close(); running = false; }

            if (event.type == Event::MouseButtonPressed) {
                Vector2i mousePos = Mouse::getPosition(window);
                if (startButton.contains(mousePos)) {
                    state = GameState::Playing;
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

void Game::runGameLoop() {
    RenderWindow window(VideoMode(800, 600), "Match-3 Game!");
    window.setFramerateLimit(144);

    Text scoreText, movesText;
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

    refillMoves();
    clearScore();

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

        if (scoreGained > 0) { score += scoreGained; }
        if (moveConsumed) { moves--; }

        scoreText.setString("Score: " + to_string(score));
        movesText.setString("Moves: " + to_string(moves));

        window.clear();
        window.draw(backgroundSprite);
        board.draw(window);
        window.draw(scoreText);
        window.draw(movesText);
        window.display();

        if (moves <= 0 && board.getState() == 0) {
            state = GameState::GameOver;
            window.close();
        }
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
            if (e.type == Event::Closed) { window.close(); running = false; }

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
        board.initialize();
        refillMoves();
        clearScore();
        state = GameState::Playing;
    }
    else { running = false; }
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
    } else {
        if (firstSelectedRow == row && firstSelectedCol == col) {
            firstSelectedRow = firstSelectedCol = -1;
            return;
        }
        bool started = board.trySwapIndices(firstSelectedRow, firstSelectedCol, row, col);
        if (started) {
            firstSelectedRow = firstSelectedCol = -1;
        } else {
            firstSelectedRow = firstSelectedCol = -1;
        }
    }
}

void Game::refillMoves() { moves = 20; }

void Game::clearScore() { score = 0; }
