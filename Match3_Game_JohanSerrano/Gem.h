#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

const float TILE_SIZE = 58;
const float GEM_WIDTH = 47;  // ancho de cada gema
const float GEM_HEIGHT = 54; // alto real de la imagen
const Vector2f offset(170, 70);

class Gem
{
private:
	float x, y;       // Posición visual (en píxeles)
	int colum, row;   // Posición lógica en la matriz
	int alpha;      // Transparencia (255 visible, 0 invisible)
	int kind;
	Vector2f boardPosition;
	
public:
	Gem();
	Gem(int aKind, int aRow, int aCol);

	void draw(RenderWindow& window, const Texture& texture) const;
	void setKind(int aType);
	int getKind();
	Vector2f getBoardPosition();

};


