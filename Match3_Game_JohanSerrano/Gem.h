#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

const float TILE_SIZE = 58;
const int GEM_WIDTH = 47;
const int GEM_HEIGHT = 54;
const Vector2f offset(171, 70);

class Gem
{
private:
	float x, y;
	int column, row;
	float alpha;  
	int kind;
	Vector2f destination;
	bool isMoving = false;
	bool isDisappearing = false;
	Sprite sprite;
	
public:
	Gem();

	void setSprite(Texture& texture);

	Sprite& getSprite();

	Gem(int aKind, int aRow, int aColumn);
	~Gem();

	void draw(RenderWindow& window);
	
	void setKind(int aType);

	void setGridPositions(int aRow, int aColum);


	int getKind();

	int getRow();
	int getColum();

	float getX();
	float getY();

	void setDestination(const Vector2f& destination);

	bool moveGem(float deltaTime);

	bool dissapear(float deltaTime);

	bool getDisappearingState();

	void startDisappearing();

	bool isEmpty();

	void resetTransientState();



};