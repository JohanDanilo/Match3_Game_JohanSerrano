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
	bool matches[ROWS][COLS] = { false };

	Gem* firstGem = nullptr;
	Gem* secondGem = nullptr;

	Vector2f originalPos1;
	Vector2f originalPos2;

	bool isSwapping = false;

	Vector2f getWindowPosition(RenderWindow& window);

	Texture texture;

	int score = 0;

	int moves = 20;

	bool stillMoving = false;

public:

	void initialize();

	void loadTexture();

	void draw(RenderWindow& window);

	bool areAdjacent() const;

	bool isInBounds(RenderWindow& window);

	void prepareSwap(RenderWindow& window);

	void updateSwap(float dt);

	void findMatches();

	bool removeMatches();

	void update(float dt);

	bool checkLineMatch(int row, int col);

	void clearMatches();

	void applyGravity();

	void refill();

	int getScore();

	int getMoves();

	void refillMoves();

	void clearScore();

	bool isResolving() const;

};
