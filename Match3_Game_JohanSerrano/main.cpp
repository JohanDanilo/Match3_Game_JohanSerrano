#include <SFML/Graphics.hpp>
#include <iostream>
using namespace sf;
using namespace std;
#include "Gem.h"
#include "Board.h"

int main()
{

    RenderWindow app(VideoMode(800, 600), "Match-3 Game!");
    app.setFramerateLimit(144);

    Texture t1;
    t1.loadFromFile("assets/background.png");

    Sprite background(t1);

    static Board grid;
    grid.initialize();

    bool isStarted = true;

    Clock clock;

    Font font;
    if (!font.loadFromFile("assets/fuente.ttf")) {
        return -1; // asegúrate de tener una fuente en assets
    }

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

    while (app.isOpen()) {
        float dt = clock.restart().asSeconds();
        
        Event e;

        while (app.pollEvent(e)) {
            if (e.type == Event::Closed) app.close();

            if( (e.type == Event::MouseButtonPressed && e.mouseButton.button == Mouse::Right) && grid.isInBounds(app) ){
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
    }


    return 0;
}