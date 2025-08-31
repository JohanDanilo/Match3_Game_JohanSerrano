#pragma once
#include "Gem.h"
#include <ctime>
#include <cstdlib>
using namespace std;
using namespace sf;

const int ROWS = 8;
const int COLS = 8;

class Board
{
private:
	
	Gem grid[ROWS][COLS];

public:

	void initialize();

	bool areAdjacent(int r1, int c1, int r2, int c2) const;

	bool areAdjacent(Gem& a, Gem& b) const;

	void moveGemTo(Gem& gem, Vector2f& destiny);

	Gem& getGem(int x, int y);
};

