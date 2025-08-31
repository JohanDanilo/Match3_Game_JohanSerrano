#pragma once
#include "Gem.h"
#include <ctime>
#include <cstdlib>

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

public:

	void initialize();

	bool areAdjacent(int aRow, int aColum, int anotherRow, int anotherColum) const;

	bool areAdjacent(Gem& a, Gem& b) const;

	void moveGemsIfHasMatch(int aRow, int aColum, int anotherRow, int anotherColum);

	Gem& getGem(int x, int y);

	bool isInBounds(RenderWindow& window);

	bool isSelectedGem(RenderWindow& window);


};

