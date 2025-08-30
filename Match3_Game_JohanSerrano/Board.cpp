#include "Board.h"

void Board::initialize()
{
	srand(static_cast<unsigned int>(time(0)));
    //Texture gems;
    //gems.loadFromFile("assets/spritesheet.png");
    
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {

            int kind = rand() % 5, row = i, col = j;

            grid[i][j] = Gem(kind, row, col);

        }
    }
}

Gem& Board::getGem(int x, int y)
{
    return grid[x][y];
}
