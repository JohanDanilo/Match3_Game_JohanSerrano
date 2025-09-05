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

    Board grid;
    grid.initialize();

    int click = 0;

    Vector2i position1, position2;

    //Gem gema(1, 8, 0);

    bool isStarted = true;

    Clock clock;


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

        app.clear();
        app.draw(background);

        grid.draw(app);

        app.display();
    }


    return 0;
}