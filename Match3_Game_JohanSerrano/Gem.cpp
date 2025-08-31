#include "Gem.h"

Gem::Gem()
{
	boardPosition = offset;
	x = y = 0.0;
	colum = row = kind = 0;
	alpha = 255;
}

Gem::Gem(int aKind, int aRow, int aCol)
{
	kind = aKind;
	row = aRow;
	colum = aCol;
	x = colum * TILE_SIZE;
	y = row * TILE_SIZE;
	alpha = 255;
}

Gem::~Gem()
{
}

void Gem::initialDraw(RenderWindow& window, Texture& texture) //getSprite(texture).setPosition(x + offset.x, y + offset.y);
{	
	Sprite gemSprite;
	gemSprite.setTexture(texture);
	gemSprite.setTextureRect(IntRect(kind * GEM_WIDTH, 0, GEM_WIDTH, GEM_HEIGHT));
	gemSprite.setPosition(x + offset.x, y + offset.y);
	gemSprite.setColor(Color(255, 255, 255, alpha));
	window.draw(gemSprite);
}


void Gem::setKind(int aKind)
{
	kind = aKind;
}

int Gem::getKind()
{
	return kind;
}

int& Gem::getRow()
{
	return row;
}

int& Gem::getColum()
{
	return colum;
}

float& Gem::getX()
{
	return x;
}

float& Gem::getY()
{
	return y;
}

void Gem::actualizarPosicionConClick(RenderWindow& ventana, Event evento) {
	// Obtener las coordenadas del mouse en la ventana
	Vector2i posicionMouse(evento.mouseButton.x, evento.mouseButton.y);

	// Convertir coordenadas de ventana a mundo
	Vector2f worldPos = ventana.mapPixelToCoords(posicionMouse);

	// Actualizar la posición lógica de la gema
	worldPos.x -= TILE_SIZE / 2; 
	worldPos.y -= TILE_SIZE / 2;

	//gemSprite.setPosition(x, y);
}

Vector2f Gem::getBoardPosition()
{
	return boardPosition;
}
