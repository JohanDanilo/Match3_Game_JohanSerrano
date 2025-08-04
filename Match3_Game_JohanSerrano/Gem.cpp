#include "Gem.h"
const int TILE_SIZE = 64;
const int BOARD_SIZE = 8;

Gem::Gem()
{
	type = 0;
	boardPosition = Vector2i(0, 0);
}

Gem::Gem(int aType, const Texture& aTexture, const Vector2i aPosition)
{
	type = aType;
	sprite.setTexture(aTexture);
	sprite.setTextureRect(sf::IntRect(type * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	boardPosition = aPosition;
	sprite.setPosition(boardPosition.x * TILE_SIZE, boardPosition.y * TILE_SIZE);
	setBoardPosition(aPosition); // esto asegura que la gema se posicione bien
}

void Gem::setType(int aType)
{
	type = aType;
	sprite.setTextureRect(IntRect(type * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
}

int Gem::getType()
{
	return type;
}

void Gem::setBoardPosition(const Vector2i& aPosition)
{
	boardPosition = aPosition;
	// Convertir coordenadas de tablero (x, y) a píxeles
	sprite.setPosition(static_cast<float>(aPosition.x * 64), static_cast<float>(aPosition.y * 64));
}


Vector2i Gem::getBoardPosition()
{
	return boardPosition;
}

void Gem::setPixelPosition(float x, float y)
{
	sprite.setPosition(x, y);
}

Sprite& Gem::getSprite()
{
	return sprite;
}