#include <SFML/Graphics.hpp>
#include <iostream>
using namespace sf;
using namespace std;
#include "Gem.h"
#include "Board.h"

int main()
{

    RenderWindow app(VideoMode(800, 600), "Match-3 Game!");
    app.setFramerateLimit(60);

    Texture t1;
    t1.loadFromFile("assets/background.png");

    Sprite background(t1);

    Board grid;
    grid.initialize();

    int click = 0;
    //bool isSwap = false, isMoving = false;

    Vector2i position1, position2;

    Texture gems;
    gems.loadFromFile("assets/spritesheet.png");

    Gem gema(1, 8, 0);

    while (app.isOpen()) {
        Event e;


        while (app.pollEvent(e)) {
            if (e.type == Event::Closed)
                app.close();

            if( (e.type == Event::MouseButtonPressed && e.mouseButton.button == Mouse::Right) && (grid.isInBounds(app)) ) {
                //Just some tests

                if (click == 0) {
                    click = 1;

                    if (grid.isSelectedGem(app) ){
                        cout << "\nThe first gem was selected! " << endl;
                    }

                }
                else if (click == 1) {
                    if (grid.isSelectedGem(app)) {
                        cout << "\nThe second gem was selected! " << endl;
                    }
                    click = 0;
                }
            }


        }
        
        app.clear();
        app.draw(background);

        //gema.initialDraw(app, gems);

        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                grid.getGem(i, j).initialDraw(app, gems);
            }
        }

        app.display();
    }

    return 0;
}