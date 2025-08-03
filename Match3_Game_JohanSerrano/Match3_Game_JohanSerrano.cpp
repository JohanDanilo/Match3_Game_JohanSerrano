#include <SFML/Graphics.hpp>
#include "Gem.h"
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <cmath> // Para abs()

const int tileSize = 64;
const int boardSize = 8;

int board[boardSize][boardSize]; // Gemas como enteros (tipo)
sf::Sprite gems[boardSize][boardSize]; // Gemas visibles (sprites)
sf::Vector2i selectedGem(-1, -1); // Gema seleccionada con clic derecho

// Verifica si hay combinación de 3 o más
bool hayCombinacion(int b[8][8]) {
    // Horizontal
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 6; ++x) {
            int tipo = b[y][x];
            if (tipo == b[y][x + 1] && tipo == b[y][x + 2])
                return true;
        }
    }

    // Vertical
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 6; ++y) {
            int tipo = b[y][x];
            if (tipo == b[y + 1][x] && tipo == b[y + 2][x])
                return true;
        }
    }

    return false;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Match-3");

    // Cargar textura con todas las gemas (una fila horizontal con 5 tipos por ejemplo)
    sf::Texture texture;
    texture.loadFromFile("assets/gems1.png");

    // Inicializa tablero con valores aleatorios (del 0 al 4)
    for (int y = 0; y < boardSize; ++y) {
        for (int x = 0; x < boardSize; ++x) {
            board[y][x] = rand() % 5;

            gems[y][x].setTexture(texture);
            gems[y][x].setTextureRect(sf::IntRect(board[y][x] * tileSize, 0, tileSize, tileSize));
            gems[y][x].setPosition(x * tileSize, y * tileSize);
        }
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Lógica de clic derecho para seleccionar e intercambiar
        if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            int x = mousePos.x / tileSize;
            int y = mousePos.y / tileSize;

            if (x >= 0 && x < boardSize && y >= 0 && y < boardSize) {
                if (selectedGem == sf::Vector2i(-1, -1)) {
                    selectedGem = sf::Vector2i(x, y); // Selecciona primera gema
                }
                else {
                    sf::Vector2i secondGem(x, y);
                    int dx = abs(secondGem.x - selectedGem.x);
                    int dy = abs(secondGem.y - selectedGem.y);

                    // Si es adyacente
                    if ((dx == 1 && dy == 0) || (dx == 0 && dy == 1)) {
                        // Intercambia en la matriz lógica
                        std::swap(board[selectedGem.y][selectedGem.x], board[secondGem.y][secondGem.x]);

                        // Verifica combinación
                        if (!hayCombinacion(board)) {
                            // Si no hay combinación, revertimos
                            std::swap(board[selectedGem.y][selectedGem.x], board[secondGem.y][secondGem.x]);
                        }
                        else {
                            // Si hubo combinación, actualizamos sprites
                            for (int y = 0; y < boardSize; ++y) {
                                for (int x = 0; x < boardSize; ++x) {
                                    gems[y][x].setTextureRect(sf::IntRect(board[y][x] * tileSize, 0, tileSize, tileSize));
                                }
                            }
                        }
                    }

                    selectedGem = sf::Vector2i(-1, -1); // Reset selección
                    sf::sleep(sf::milliseconds(150)); // Pequeño delay para evitar múltiples clics
                }
            }
        }

        // Dibujar
        window.clear();
        for (int y = 0; y < boardSize; ++y) {
            for (int x = 0; x < boardSize; ++x) {
                gems[y][x].setPosition(x * tileSize, y * tileSize);
                window.draw(gems[y][x]);
            }
        }
        window.display();
    }

    return 0;
}
