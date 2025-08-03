#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Mover gema con click derecho");
    window.setFramerateLimit(60);

    // Cargar textura y sprite
    sf::Texture texture;
    if (!texture.loadFromFile("assets/TimeStone.png"))
        return -1;

    sf::Sprite sprite;
    sprite.setTexture(texture);

    sprite.setScale(0.2f, 0.2f);

    // Centrar el origen del sprite
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2, bounds.height / 2);

    // Posición inicial en el centro de la ventana
    sprite.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);

    bool dragging = false;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // Cuando presionas el botón derecho del mouse
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right)
            {
                // Obtener posición del mouse y convertirla a coordenadas del mundo
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                // Verificar si el mouse está sobre la gema
                if (sprite.getGlobalBounds().contains(mousePos))
                {
                    dragging = true;
                }
            }

            // Cuando sueltas el botón derecho del mouse
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right)
            {
                dragging = false;
            }
        }

        // Si estás arrastrando, mover la gema a la posición del mouse
        if (dragging)
        {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            sprite.setPosition(mousePos);
        }

        window.clear(sf::Color::Black);
        window.draw(sprite);
        window.display();
    }

    return 0;
}
