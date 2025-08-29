#include <SFML/Graphics.hpp>
#include <ctime>
using namespace sf;
using namespace std;
#include "Gem.h"

float ts = 58;


int main()
{
    srand(time(0));

    RenderWindow app(VideoMode(800, 600), "Match-3 Game!");
    app.setFramerateLimit(60);

    Texture t1, t2;
    t1.loadFromFile("assets/background.png");
    t2.loadFromFile("assets/spritesheet.png");

    Sprite background(t1), gems(t2);


    int click = 0;
    //bool isSwap = false, isMoving = false;

    Vector2i position1, position2;

    while (app.isOpen()) {
        Event e;
        while (app.pollEvent(e)) {
            if (e.type == Event::Closed)
                app.close();

            if (e.type == Event::MouseButtonPressed && e.mouseButton.button == Mouse::Right) {
                //int col = e.mousebutton.x / ts; // convertir píxeles a índice
                //int row = e.mousebutton.y / ts;

                if (click == 0) {
                    click = 1;
                    position1 = Mouse::getPosition(app);
                }
                else if (click == 1) {
                    position2 = Mouse::getPosition(app);

                    //Gem& selectedGem = grid.getGem(firstRow, firstCol);

                    //selectedGem1.setTarget(secondRow, secondCol); // mover suavemente
                    //selectedGem2.setTarget(firstRow, firstCol);

                    click = 0; // reiniciar
                }
            }


        }

        app.clear();
        app.draw(background);
        app.display();
    }
    return 0;
}