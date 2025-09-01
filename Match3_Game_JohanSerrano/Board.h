#pragma once
#include "Gem.h"
#include <ctime>
#include <cstdlib>
#include <cmath>

using namespace std;
using namespace sf;

const int ROWS = 8;
const int COLS = 8;

const int BOARD_WIDTH = TILE_SIZE * ROWS;
const int BOARD_HEIGTH = TILE_SIZE * COLS;

class Board
{
private:
	
	Gem grid[ROWS][COLS];

	Gem* firstGem = nullptr;
	Gem* secondGem = nullptr;
	bool isSwapping = false;

	Vector2f getGridPosition(RenderWindow& window);

public:

	void initialize();

	void draw(RenderWindow& window);

	bool areAdjacent(int aRow, int aColum, int anotherRow, int anotherColum) const;

	bool areAdjacent(Gem& a, Gem& b) const;

	void moveGems(RenderWindow& window, float& deltaTime, int click);

	Gem& getGem(int x, int y);

	bool isInBounds(RenderWindow& window);

	bool isSelectedGem(RenderWindow& window);

	void prepareSwap(RenderWindow& window);

	void updateSwap(float dt);


};

