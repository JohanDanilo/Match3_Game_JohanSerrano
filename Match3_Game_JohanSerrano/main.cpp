#include <SFML/Graphics.hpp>
#include <iostream>
using namespace sf;
using namespace std;
#include "Gem.h"
#include "Board.h"

// Función que ejecuta el juego
// Retorna true si se terminó por falta de movimientos
bool playGame(Board& grid, Font& font, Sprite& background) {
    RenderWindow app(VideoMode(800, 600), "Match-3 Game!");
    app.setFramerateLimit(144);

    // Texto Score
    Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(40);
    scoreText.setFillColor(Color::White);
    scoreText.setOutlineColor(Color::Black);
    scoreText.setOutlineThickness(2);
    scoreText.setPosition(70, 8);

    Text movesText;
    movesText.setFont(font);
    movesText.setCharacterSize(40);
    movesText.setFillColor(Color::White);
    movesText.setOutlineColor(Color::Black);
    movesText.setOutlineThickness(2);
    movesText.setPosition(550, 8);

    grid.initialize(); // reinicia tablero
    grid.refillMoves();
    grid.clearScore();
    Clock clock;

    while (app.isOpen()) {
        float dt = clock.restart().asSeconds();

        Event e;
        while (app.pollEvent(e)) {
            if (e.type == Event::Closed) app.close();
            if ((e.type == Event::MouseButtonPressed && e.mouseButton.button == Mouse::Right) && grid.isInBounds(app)) {
                grid.prepareSwap(app);
            }

        }

        grid.update(dt);

        
        scoreText.setString("Score: " + to_string(grid.getScore()));
        movesText.setString("Moves: " + to_string(grid.getMoves()));

        app.clear();
        app.draw(background);
        grid.draw(app);
        app.draw(scoreText);
        app.draw(movesText);
        app.display();

        
        

        if (grid.getMoves() <= 0 && !grid.isResolving()) {
            return true; // game over
        }
    }

    return true; // ventana cerrada manualmente
}

int main() {
    RenderWindow window(VideoMode(800, 600), "Main Menu");

    static Board grid;

    Font font;
    if (!font.loadFromFile("assets/fuente.ttf")) {
        return -1;
    }

    // Fondo principal
    Texture t1;
    t1.loadFromFile("assets/background.png");
    Sprite background(t1);

    // --- Pantalla de menú ---
    Texture menuTexture;
    if (!menuTexture.loadFromFile("assets/mainMenu.png")) {
        cout << "Error cargando mainMenu.png\n";
        return -1;
    }
    Sprite menuSprite(menuTexture);

    IntRect exitButtonArea(706, 45, 70, 32);
    IntRect startButtonArea(321, 547, 160, 48);

    bool startGame = false;

    while (window.isOpen() && !startGame) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) window.close();

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Right) {
                Vector2i mousePos = Mouse::getPosition(window);
                if (startButtonArea.contains(mousePos)) {
                    startGame = true;
                    window.close();
                }
                else if (exitButtonArea.contains(mousePos)) {
                    window.close();
                }
            }
        }

        window.clear();
        window.draw(menuSprite);
        window.display();
    }

    // --- Bucle principal: juego y pantalla de Game Over ---
    bool playing = startGame;
    while (playing) {
        bool gameOver = playGame(grid, font, background);

        if (gameOver) {
            RenderWindow overWindow(VideoMode(800, 600), "Game Over");

            // Cargar pantalla Game Over
            Texture gameOverTexture;
            if (!gameOverTexture.loadFromFile("assets/gameOver.png")) {
                cout << "Error cargando gameOver.png\n";
            }
            Sprite gameOverSprite(gameOverTexture);

            // Texto final de puntaje
            Text finalScore;
            finalScore.setFont(font);
            finalScore.setCharacterSize(60);
            finalScore.setFillColor(Color::White);
            finalScore.setOutlineColor(Color::Black);
            finalScore.setOutlineThickness(2);
            finalScore.setPosition(224, 297);
            finalScore.setString("Final Score: " + to_string(grid.getScore()));

            IntRect restartButtonArea(190, 467, 193, 50);
            IntRect quitButtonArea(454, 467, 193, 50);

            bool restart = false;

            while (overWindow.isOpen()) {
                Event e;
                while (overWindow.pollEvent(e)) {
                    if (e.type == Event::Closed) overWindow.close();

                    if (e.type == Event::MouseButtonPressed && e.mouseButton.button == Mouse::Right) {
                        Vector2i mousePos = Mouse::getPosition(overWindow);
                        if (restartButtonArea.contains(mousePos)) {
                            restart = true;
                            overWindow.close();
                        }
                        else if (quitButtonArea.contains(mousePos)) {
                            playing = false;
                            overWindow.close();
                        }
                    }
                }

                overWindow.clear();
                overWindow.draw(gameOverSprite);
                overWindow.draw(finalScore);
                overWindow.display();
            }

            if (!restart) playing = false;
        }
    }

    return 0;
}
