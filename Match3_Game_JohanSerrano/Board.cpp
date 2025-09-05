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

            originalPos1 = firstGem->getSprite().getPosition();
            originalPos2 = secondGem->getSprite().getPosition();

            
            firstGem->setDestination(originalPos2);
            secondGem->setDestination(originalPos1);

            cout << "And they're adjacent, so they have to move" << endl <<endl;

            isSwapping = true;
        }
        else {
            
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

        
        swap(grid[r1][c1], grid[r2][c2]);
        grid[r1][c1].setGridPositions(r1, c1);
        grid[r2][c2].setGridPositions(r2, c2);

        bool anyMatch = checkLineMatch(r1, c1) || checkLineMatch(r2, c2);

        if (!anyMatch) {
            
            swap(grid[r1][c1], grid[r2][c2]);
            grid[r1][c1].setGridPositions(r1, c1);
            grid[r2][c2].setGridPositions(r2, c2);

            firstGem->setDestination(originalPos1);
            secondGem->setDestination(originalPos2);

            isSwapping = true;
            cout << "Swap reverted: no match found" << endl;
        }
        else {
            
            for (int i = 0; i < ROWS; i++)
                for (int j = 0; j < COLS; j++)
                    if (matches[i][j])
                        grid[i][j].startDisappearing();

            
        }

        firstGem = nullptr;
        secondGem = nullptr;
        isSwapping = false;

    }
}


void Board::findMatches() {

    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            matches[r][c] = false;
    
    for (int row = 0; row < ROWS; row++) {
        int horizontalMatch = 1;
        for (int col = 1; col < COLS; col++) {
            if (grid[row][col].getKind() == grid[row][col - 1].getKind()) {
                horizontalMatch++;
            }
            else {
                if (horizontalMatch >= 3) {
                    int start = col - horizontalMatch;
                    int end = col - 1;
                    for (int c = start; c <= end; c++) {
                        matches[row][c] = true;
                    }
                        
                        
                }
                horizontalMatch = 1;
            }
        }
        if (horizontalMatch >= 3) {
            int start = COLS - horizontalMatch;
            int end = COLS - 1;
            for (int c = start; c <= end; c++) {
                matches[row][c] = true;
            }
        }
    }

    
    for (int col = 0; col < COLS; col++) {
        int verticalMatch = 1;
        for (int row = 1; row < ROWS; row++) {
            if (grid[row][col].getKind() == grid[row - 1][col].getKind()) {
                verticalMatch++;
            }
            else {
                if (verticalMatch >= 3) {
                    int start = row - verticalMatch;
                    int end = row - 1;
                    for (int r = start; r <= end; r++) {
                        matches[r][col] = true;
                    }
                }
                verticalMatch = 1;
            }
        }
        if (verticalMatch >= 3) {
            int start = ROWS - verticalMatch;
            int end = ROWS - 1;
            for (int r = start; r <= end; r++) {
                matches[r][col] = true;
            }
        }
    }
}

bool Board::removeMatches()
{
    bool anyFading = false;

    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {

            if (grid[r][c].dissapear()) {
                anyFading = true;
            }
        }
    }

    return anyFading;
}

void Board::update(float dt) {

    if (isSwapping) {
        updateSwap(dt);
        return;
    }

    bool stillMoving = false;

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (!grid[i][j].moveGem(dt)) {
                stillMoving = true;
            }
        }
    }

    if (!stillMoving) {

        findMatches();

        bool anyDisappearing = false;
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                if (matches[i][j]) {
                    grid[i][j].startDisappearing();
                    matches[i][j] = false;
                }

                if (grid[i][j].getDisappearingState()) {
                    if (grid[i][j].dissapear())
                        anyDisappearing = true;
                }
            }
        }

        // Aquí más adelante va "gravedad" y "refill"
    }

}

bool Board::checkLineMatch(int row, int col) {
    int kind = grid[row][col].getKind();

    int count = 1;

    for (int c = col - 1; c >= 0; c--) {
        if (grid[row][c].getKind() == kind) count++;
        else break;
    }

    for (int c = col + 1; c < COLS; c++) {
        if (grid[row][c].getKind() == kind) count++;
        else break;
    }

    if (count >= 3) return true;

    count = 1;

    for (int r = row - 1; r >= 0; r--) {
        if (grid[r][col].getKind() == kind) count++;
        else break;
    }

    for (int r = row + 1; r < ROWS; r++) {
        if (grid[r][col].getKind() == kind) count++;
        else break;
    }

    if (count >= 3) return true;

    return false;
}

