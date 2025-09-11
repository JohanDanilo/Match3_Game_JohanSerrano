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

	bool scoring = false;

	bool pendingScore = false;   // hay matches pendientes por contabilizar (esperando desaparición)
	bool isReverting = false;    // estamos animando la reversión del swap (no limpiar punteros hasta que termine)
	bool scoreFromSwap = false;  // indica si el match que generó pendingScore vino de un swap del jugador



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

	bool isScoring() const;
};
