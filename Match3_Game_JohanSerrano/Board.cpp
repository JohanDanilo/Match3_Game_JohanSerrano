#include "Board.h"

void Board::initialize()
{
	srand(static_cast<unsigned int>(time(0)));
    
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {

            int kind = rand() % 5, row = i, col = j;
            grid[i][j] = Gem(kind, row, col);

        }
    }
}

bool Board::areAdjacent(int r1, int c1, int r2, int c2) const {
    
    if (r1 < 0 || r1 >= ROWS || r2 < 0 || r2 >= ROWS ||
        c1 < 0 || c1 >= COLS || c2 < 0 || c2 >= COLS) {
        return false;
    }

    
    if (r1 == r2 && c1 == c2) return false;

    int dr = abs(r1 - r2);
    int dc = abs(c1 - c2);

    
    return (dr == 1 && dc == 0) || (dr == 0 && dc == 1);
}

bool Board::areAdjacent( Gem& a, Gem& b) const {
    return areAdjacent(a.getRow(), a.getColum(), b.getRow(), b.getColum());
}

void Board::moveGemsIfHasMatch(int aRow, int aColum, int anotherRow, int anotherColum)
{
}



Gem& Board::getGem(int x, int y)
{
    return grid[x][y];
}

bool Board::isInBounds(RenderWindow& window)
{
    Vector2i position;
    position = Mouse::getPosition(window);
    int mouseX = position.x, mouseY = position.y;

    Vector2f worldPosition = window.mapPixelToCoords(position);

    int col = (mouseX - offset.x) / TILE_SIZE;
    int row = (mouseY - offset.y) / TILE_SIZE;

    if (col < 0 || col >= BOARD_WIDTH || row < 0 || row >= BOARD_HEIGTH) {
        return false;
    }
    return true;
}

bool Board::isSelectedGem(RenderWindow& window)
{

    Vector2i position;
    position = Mouse::getPosition(window);
    int mouseX = position.x, mouseY = position.y;

    Vector2f worldPosition = window.mapPixelToCoords(position);

    int col = (mouseX - offset.x) / TILE_SIZE;
    int row = (mouseY - offset.y) / TILE_SIZE;

    float gemXAxis = getGem(row, col).getX();
    float gemYAxis = getGem(row, col).getY();

    if ((gemXAxis + offset.x <= worldPosition.x && worldPosition.x <= gemXAxis + offset.x + TILE_SIZE)
        && (gemYAxis + offset.y <= worldPosition.y && worldPosition.y <= gemYAxis + offset.y + TILE_SIZE)) {
        return true;
    }
    return false;
}
