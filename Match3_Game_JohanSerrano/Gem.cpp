#include "Gem.h"

Gem::Gem()
{
	type = 0;
	boardPosition = Vector2i(0, 0);
}

Gem::Gem(int aType, const Texture& aTexture, const Vector2i aPosition)
{
	type = aType;
	sprite.setTexture(aTexture);
	boardPosition = aPosition;
	setBoardPosition(aPosition); // esto asegura que la gema se posicione bien
}

void Gem::setType(int aType)
{
	type = aType;
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
