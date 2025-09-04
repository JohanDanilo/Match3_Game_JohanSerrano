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

    saveTexture();
}

void Board::saveTexture()
{
    Texture gems;
    gems.loadFromFile("assets/spritesheet.png");
    
    texture = gems;
}


void Board::draw(RenderWindow& window)
{
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < 8; j++) {
            getGem(i, j).initialDraw(window, texture);
        }
    }
}

bool Board::areAdjacent() const {

    int row1 = firstGem->getRow();
    int col1 = firstGem->getColum();
    int row2 = secondGem->getRow();
    int col2 = secondGem->getColum();
    
    if (row1 == row2 && col1 == col2) return false;
    
    return (abs(row1 - row2) + abs(col1 - col2) == 1);
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

void Board::prepareSwap(RenderWindow& window) {
    
    int row1 = 0, col1 = 0, row2 = 0, col2 = 0;

    if (!firstGem && isInBounds(window)) {
        // Primer click
        Vector2i pos = Mouse::getPosition(window);
        col1 = (pos.x - offset.x) / TILE_SIZE;
        row1 = (pos.y - offset.y) / TILE_SIZE;
        firstGem = &grid[row1][col1];
        cout << "First gem selected in grid: " << firstGem->getRow() << ", " << firstGem->getColum() <<endl;
    }
    else if (isInBounds(window)){
        // Segundo click
        Vector2i pos = Mouse::getPosition(window);
        col2 = (pos.x - offset.x) / TILE_SIZE;
        row2 = (pos.y - offset.y) / TILE_SIZE;

        secondGem = &grid[row2][col2];
        cout << "Second gem selected in grid: " << secondGem->getRow() << ", " << secondGem->getColum() << endl;

        if ( areAdjacent() ) {

            Vector2f pos1 = firstGem->getSprite().getPosition();
            Vector2f pos2 = secondGem->getSprite().getPosition();

            // Asignar destinos cruzados
            firstGem->setDestination(pos2);
            secondGem->setDestination(pos1);

            cout << "And they're adjacent, so they have to move" << endl <<endl;

            isSwapping = true;
        }
        else {
            // Si no son adyacentes, cancelar selección
            firstGem = nullptr;
            secondGem = nullptr;
            cout << "And they're not adjacent, so they cant move" << endl << endl;
        }
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
        grid[r1][c1].setGridPositions(r1, c1);
        grid[r2][c2].setGridPositions(r2, c2);

        // Reset
        firstGem = nullptr;
        secondGem = nullptr;
        isSwapping = false;
    }

}

