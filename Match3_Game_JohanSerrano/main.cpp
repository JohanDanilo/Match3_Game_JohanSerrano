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

            if (e.type == Event::MouseButtonPressed && e.mouseButton.button == Mouse::Right) {
                //Just some tests

                if (click == 0) {
                    click = 1;
                    position1 = Mouse::getPosition(app);
                    int x = position1.x, y = position1.y;

                    Vector2f worldPos1 = app.mapPixelToCoords(position1);

                    //cout << position1.x << " " << position1.y << endl;

                    // convertir a índices de grilla
                    int col = (x - offset.x) / TILE_SIZE;
                    int row = (y - offset.y) / TILE_SIZE;

                    //cout << row << " " << col << endl;

                    float x1 = grid.getGem(row, col).getX();
                    float y1 = grid.getGem(row, col).getY();

                    if( (x1 + offset.x <= worldPos1.x && worldPos1.x <= x1 + offset.x + TILE_SIZE)
                        && (y1 + offset.y <= worldPos1.y && worldPos1.y <= y1 + offset.y + TILE_SIZE)) {
                        cout << "The first selected gem with the right click in the window is: " << grid.getGem(row, col).getRow() << ", " << grid.getGem(row, col).getColum();
                        cout << " and the clicked position is: " << position1.x << " " << position1.y << endl;
                    }
                }


                else if (click == 1) {
                    
                    position2 = Mouse::getPosition(app);
                    int x = position2.x, y = position2.y;
                    Vector2f worldPos2 = app.mapPixelToCoords(position2);

                    //cout << position1.x << " " << position1.y << endl;

                    // convertir a índices de grilla
                    int col = (x - offset.x) / TILE_SIZE;
                    int row = (y - offset.y) / TILE_SIZE;

                    //cout << row << " " << col << endl;

                    float x2 = grid.getGem(row, col).getX();
                    float y2 = grid.getGem(row, col).getY();

                    if ((x2 + offset.x <= worldPos2.x && worldPos2.x <= x2 + offset.x + TILE_SIZE)
                        && (y2 + offset.y <= worldPos2.y && worldPos2.y <= y2 + offset.y + TILE_SIZE)) {
                        cout << "\nThe second selected gem with the right click in the window is: " << row << ", " << col ;
                        cout << " and the clicked position is: " << position2.x << " " << position2.y << endl;

                    }
                    click = 0;
                }
            }


        }
        
        app.clear();
        app.draw(background);

        gema.initialDraw(app, gems);

        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                grid.getGem(i, j).initialDraw(app, gems);
            }
        }

        app.display();
    }

    return 0;
}