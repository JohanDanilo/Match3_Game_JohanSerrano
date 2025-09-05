#include "Gem.h"

Gem::Gem()
{
	x = y = 0.0;
	colum = row = kind = 0;
	alpha = 255;
}

Sprite& Gem::getSprite()
{
	return copySprite;
}

Gem::Gem(int aKind, int aRow, int aCol)
{
	kind = aKind;
	row = aRow;
	colum = aCol;
	x = (colum * TILE_SIZE)+ offset.x;
	y = (row * TILE_SIZE)+ offset.y;
	alpha = 255;
	copySprite.setPosition(x , y );
}

Gem::~Gem()
{
}

void Gem::initialDraw(RenderWindow& window, Texture texture)
{	
	copySprite.setTexture(texture);
	copySprite.setTextureRect(IntRect(kind * GEM_WIDTH, 0, GEM_WIDTH, GEM_HEIGHT));
	copySprite.setColor(Color(255, 255, 255, alpha));
	window.draw(copySprite);
}


void Gem::setKind(int aKind)
{
	kind = aKind;
}

void Gem::setGridPositions(int aRow, int aColum)
{
	row = aRow;
	colum = aColum;
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
	return colum;
}

float Gem::getX()
{
	return x;
}

float Gem::getY()
{
	return y;
}

void Gem::setDestination(const Vector2f& d) {
	destiny = d;
	isMoving = true;
}

bool Gem::moveGem(float dt) {
	if (!isMoving) return true;

	float velocidad = 400.f;
	Vector2f currentPos = copySprite.getPosition();
	Vector2f direction = destiny - currentPos;
	float distance = sqrt(direction.x * direction.x + direction.y * direction.y);

	if (distance > 1.0f) {
		direction /= distance;
		copySprite.move(direction * velocidad * dt);
		return false;
	}
	else {
		
		isMoving = false;

		x = (colum * TILE_SIZE);
		y = (row * TILE_SIZE);

		return true;
	}

}

bool Gem::dissapear() {
	if (!isDisappearing)
		return false;  

	if (alpha > 10) {
		alpha -= 3;   
		return true;  
	}
	else {
		alpha = 0;     
		isDisappearing = false; 
		return false; 
	}

}

bool Gem::getDisappearingState()
{
	return isDisappearing;
}
