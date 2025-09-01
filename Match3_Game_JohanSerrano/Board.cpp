#include "Board.h"

Vector2f Board::getGridPosition(RenderWindow& window)
{
    Vector2i position;
    position = Mouse::getPosition(window);
    int mouseX = position.x, mouseY = position.y;
    Vector2f worldPosition = window.mapPixelToCoords(position);
    return worldPosition;
}

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

void Board::draw(RenderWindow& window)
{
    /*for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            getGem(i, j).initialDraw(window, gems);
        }
    }*/
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

void Board::moveGems(RenderWindow& window, float& deltaTime, int click)
{

    Vector2f origin, destiny;

    // Here we get the gems a & b by accesing the grid positions like (0, 0)

    int row1 = 0, row2 = 0, colum1 = 0, colum2 = 0;
    
    Gem gem1;
    if (isSelectedGem(window) && click == 0) {
        Vector2i position;
        position = Mouse::getPosition(window);
        int mouseX = position.x, mouseY = position.y;
        colum1 = (mouseX - offset.x) / TILE_SIZE;
        row1 = (mouseY - offset.y) / TILE_SIZE;
        gem1 = getGem(row1, colum1);
        origin = Vector2f(gem1.getX(), gem1.getY() );// Posición actual
    }

    Gem gem2;
    if (isSelectedGem(window) && click == 1) {
        Vector2i position2 = Mouse::getPosition(window);
        int mouseX = position2.x, mouseY = position2.y;
        colum2 = (mouseX - offset.x) / TILE_SIZE;
        row2 = (mouseY - offset.y) / TILE_SIZE;
        gem2 = getGem(row2, colum2);
        destiny = Vector2f(gem2.getX(), gem2.getY());// Posición actual
    }

    getGem(row1, colum1).moveGem(deltaTime);
    getGem(row2, colum2).moveGem(deltaTime);

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

    if (col < 0 || col >= COLS || row < 0 || row >= ROWS) {
        return false;
    }
    return true;
}

bool Board::isSelectedGem(RenderWindow& window)
{

    Vector2i position;
    position = Mouse::getPosition(window);
    int mouseX = position.x, mouseY = position.y;

    Vector2f worldPosition = getGridPosition(window);

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

void Board::prepareSwap(RenderWindow& window) {
    if (!firstGem) {
        // Primer click
        Vector2i pos = Mouse::getPosition(window);
        int col = (pos.x - offset.x) / TILE_SIZE;
        int row = (pos.y - offset.y) / TILE_SIZE;
        firstGem = &grid[row][col];
    }
    else {
        // Segundo click
        Vector2i pos = Mouse::getPosition(window);
        int col = (pos.x - offset.x) / TILE_SIZE;
        int row = (pos.y - offset.y) / TILE_SIZE;
        secondGem = &grid[row][col];

        // Guardar posiciones originales
        Vector2f pos1 = firstGem->getSprite().getPosition();
        Vector2f pos2 = secondGem->getSprite().getPosition();

        // Asignar destinos cruzados
        firstGem->setDestination(pos2);
        secondGem->setDestination(pos1);

        isSwapping = true;
    }
}

void Board::updateSwap(float dt) {
    if (!isSwapping) return;

    bool done1 = firstGem->moveGem(dt);
    bool done2 = secondGem->moveGem(dt);

    if (done1 && done2) {
        int r1 = firstGem->getRow();
        int c1 = firstGem->getColum();
        int r2 = secondGem->getRow();
        int c2 = secondGem->getColum();

        // Intercambiar en la matriz grid
        swap(grid[r1][c1], grid[r2][c2]);

        // Ajustar filas/columnas de cada gema
        grid[r1][c1].getRow() = r1;
        grid[r1][c1].getColum() = c1;

        grid[r2][c2].getRow() = r2;
        grid[r2][c2].getColum() = c2;

        // Reset
        firstGem = nullptr;
        secondGem = nullptr;
        isSwapping = false;
    }

}

