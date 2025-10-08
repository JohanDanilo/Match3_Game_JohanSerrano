#include "Gem.h"
#include <cmath>

Gem::Gem()
{
    x = y = 0.0;
    column = row = kind = 0;
    alpha = 255.0;
}

void Gem::setSprite(Texture& texture)
{
    sprite.setTexture(texture);
    sprite.setTextureRect(IntRect(kind * GEM_WIDTH, 0, GEM_WIDTH, GEM_HEIGHT));
}

Sprite& Gem::getSprite()
{
    return sprite;
}

Gem::Gem(int aKind, int aRow, int aCol) {
    kind = aKind;
    row = aRow;
    column = aCol;
    x = (column * TILE_SIZE) + offset.x;
    y = (row * TILE_SIZE) + offset.y;
    alpha = 255.f;
	sprite.setPosition(x, y);
}

Gem::~Gem()
{
}

void Gem::draw(RenderWindow& window)
{
	sprite.setColor(Color(255, 255, 255, alpha));
	window.draw(sprite);
}

void Gem::onMatch(Board& board, int row, int col)
{
	startDisappearing();
}

string Gem::getType() const
{
	return string("Normal");
}

Gem* Gem::clone() const
{
	return new Gem(*this);
}


void Gem::setKind(int aKind)
{
	kind = aKind;
}

void Gem::setGridPositions(int aRow, int aColumn)
{
	row = aRow;
	column = aColumn;
}

int Gem::getKind()
{
	return kind;
}

int Gem::getRow()
{
	return row;
}

int Gem::getColum()
{
	return column;
}

float Gem::getX()
{
	return sprite.getPosition().x;
}

float Gem::getY()
{
	return sprite.getPosition().y;
}

void Gem::resetTransientState()
{
	isMoving = false;
	isDisappearing = false;
	alpha = 255.f;
	sprite.setColor(Color(255, 255, 255, 255));
}


void Gem::setDestination(const Vector2f& aDestination) {
	destination = aDestination;
	isMoving = true;
}

bool Gem::moveGem(float deltaTime) {
	if (!isMoving) { return true; }

	const float speed = 400.f;
	Vector2f currentPos = sprite.getPosition();
	Vector2f direction = destination - currentPos;
	float distance = sqrt(direction.x * direction.x + direction.y * direction.y);

	if (distance > 1.0f) {
		direction /= distance;
		sprite.move(direction * speed * deltaTime);
		return false;
	}
	else {
		sprite.setPosition(destination);
		isMoving = false;

		x = sprite.getPosition().x;
		y = sprite.getPosition().y;

		return true;
	}

}

bool Gem::dissapear(float deltaTime) {
	if (!isDisappearing) { return false; }

	const float fadeSpeed = 400.f;
	alpha -= fadeSpeed * deltaTime;

	if (alpha > 0.f) {
		return true;
	}
	else {
		alpha = 0.f;
		isDisappearing = false;
		sprite.setColor(Color(255, 255, 255, 0));
		return false;
	}
}

bool Gem::getDisappearingState()
{
	return isDisappearing;
}

void Gem::startDisappearing()
{
	isDisappearing = true;
}

bool Gem::isEmpty()
{
	return (alpha <= 0);
}