#pragma once
#include "Gem.h"
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <iostream>

using namespace std;
using namespace sf;

const int ROWS = 8;
const int COLS = 8;

const int BOARD_WIDTH = ROWS * GEM_WIDTH;
const int BOARD_HEIGTH = COLS * GEM_HEIGHT;

class Board
{
private:
	
	Gem grid[ROWS][COLS];

	Gem* firstGem = nullptr;
	Gem* secondGem = nullptr;
	bool isSwapping = false;

	Vector2f getGridPosition(RenderWindow& window);

	Texture texture;

public:

	void initialize();

	void saveTexture();

	void draw(RenderWindow& window);

	bool areAdjacent() const;

	Gem& getGem(int x, int y);

	bool isInBounds(RenderWindow& window);

	void prepareSwap(RenderWindow& window);

	void updateSwap(float dt);


};

