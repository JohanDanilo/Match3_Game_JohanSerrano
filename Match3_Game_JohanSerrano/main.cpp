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

    while (app.isOpen()) {
        Event e;


        while (app.pollEvent(e)) {
            if (e.type == Event::Closed)
                app.close();

            if (e.type == Event::MouseButtonPressed && e.mouseButton.button == Mouse::Right) {
                //Just some tests
                ////gem.actualizarPosicionConClick(app, e);

                // Obtener las coordenadas del mouse en la ventana
                Vector2i posicionMouse(e.mouseButton.x, e.mouseButton.y);

                //Convertir coordenadas de ventana a coordenadas del mundo (útil si usas vistas/cámaras)
                Vector2f worldPos = app.mapPixelToCoords(posicionMouse);

                cout << worldPos.x << " " << worldPos.y << endl;

                worldPos.x -= TILE_SIZE/2; worldPos.y -= TILE_SIZE/2;

                //gem.getSprite().setPosition(worldPos);

                //if (click == 0) {
                //    click = 1;
                //    position1 = Mouse::getPosition(app);
                //}
                //else if (click == 1) {
                //    position2 = Mouse::getPosition(app);

                //    // Llamar al método para actualizar posición
                //    

                //    click = 0; // reiniciar
                //}
            }


        }

        Texture gems;
        gems.loadFromFile("assets/spritesheet.png");
        
        app.clear();
        app.draw(background);
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                grid.getGem(i, j).draw(app, gems);
            }
        }
        //gem.draw(app, t2);
        app.display();
    }

    return 0;
}