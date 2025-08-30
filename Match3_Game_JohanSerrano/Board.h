#pragma once
#include "Gem.h"
#include <ctime>
#include <random>
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

	Gem& getGem(int x, int y);
};

