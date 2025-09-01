#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

const float TILE_SIZE = 58;
const int GEM_WIDTH = 47;  // ancho de cada gema
const int GEM_HEIGHT = 54; // alto real de la imagen
const Vector2f offset(171, 70);

class Gem
{
private:
	float x, y;       // Posición visual (en píxeles)
	int colum, row;   // Posición lógica en la matriz
	int alpha;      // Transparencia (255 visible, 0 invisible)
	int kind;
	Vector2f destiny;
	bool isMoving = false;
	
public:
	Gem();

	Sprite copySprite;

	Sprite& getSprite();

	Gem(int aKind, int aRow, int aCol);
	~Gem();

	void initialDraw(RenderWindow& window,Texture& texture);
	
	void setKind(int aType);
	int getKind();

	int& getRow();
	int& getColum();

	float& getX();
	float& getY();

	void setDestination(const Vector2f& d);

	bool moveGem(float dt);

	//Vector2f getBoardPosition();

};


