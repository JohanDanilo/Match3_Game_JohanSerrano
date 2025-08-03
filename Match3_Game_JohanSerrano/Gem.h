#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class Gem
{
private:
	int type;
	Sprite sprite;
	Vector2i boardPosition;

public:
	Gem();
	Gem(int aType, const Texture& aTexture, const Vector2i aPosition);
	void setType(int aType);
	int getType();
	void setBoardPosition(const Vector2i& aPosition);
	Vector2i getBoardPosition();
	void setPixelPosition(float x, float y);
	Sprite& getSprite();

};

