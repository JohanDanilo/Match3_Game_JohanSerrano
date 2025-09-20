#include "Board.h"

Board::Board() {
    loadTexture();
    initialize();
    state = Idle;
}

void Board::initialize() {
    srand(static_cast<unsigned int>(time(0)));
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            int kind = rand() % 5;
            grid[i][j] = Gem(kind, i, j);
            grid[i][j].setSprite(texture);
            grid[i][j].setGridPositions(i, j);
            Vector2f dest(j * TILE_SIZE + offset.x, i * TILE_SIZE + offset.y);
            grid[i][j].setDestination(dest);
        }
    }
}

void Board::loadTexture() {
    texture.loadFromFile("assets/spritesheet.png");
}

void Board::draw(RenderWindow& window) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            grid[i][j].draw(window);
        }
    }
}

bool Board::areAdjacent(int row1, int col1, int row2, int col2) const {
    if (row1 == row2 && col1 == col2) return false;// CHECK: Los if deben tener brackets
    return (abs(row1 - row2) + abs(col1 - col2) == 1);
}

bool Board::trySwapIndices(int row1, int col1, int row2, int col2) {
    if (row1 < 0 || row1 >= ROWS || col1 < 0 || col1 >= COLS ||
        row2 < 0 || row2 >= ROWS || col2 < 0 || col2 >= COLS) return false;// CHECK: Los if deben tener brackets
    if (!areAdjacent(row1, col1, row2, col2)) return false;// CHECK: Los if deben tener brackets

    firstRow = row1; firstCol = col1;
    secondRow = row2; secondCol = col2;

    firstGem = &grid[firstRow][firstCol];
    secondGem = &grid[secondRow][secondCol];

    swapOrigPos1 = firstGem->getSprite().getPosition();
    swapOrigPos2 = secondGem->getSprite().getPosition();

    Vector2f pos1(col1 * TILE_SIZE + offset.x, row1 * TILE_SIZE + offset.y);
    Vector2f pos2(col2 * TILE_SIZE + offset.x, row2 * TILE_SIZE + offset.y);

    firstGem->setDestination(pos2);
    secondGem->setDestination(pos1);

    state = Swapping;
    playerInitiatedMove = true;
    return true;
}

void Board::update(float deltaTime, int& scoreGained, bool& moveConsumed) {
    scoreGained = 0;
    moveConsumed = false;
    bool anyMatching = false;

    switch (state) {
    case Idle:
        findMatches();
        for (int r = 0; r < ROWS && !anyMatching; r++)// CHECK: Los for deben tener brackets
            for (int c = 0; c < COLS && !anyMatching; c++)// CHECK: Los for deben tener brackets
                if (matches[r][c]) anyMatching = true;// CHECK: Los if deben tener brackets

        if (anyMatching) {
            for (int r = 0; r < ROWS; r++)// CHECK: Los for deben tener brackets
                for (int c = 0; c < COLS; c++)// CHECK: Los for deben tener brackets
                    if (matches[r][c]) grid[r][c].startDisappearing();// CHECK: Los if deben tener brackets
            playerInitiatedMove = false;
            state = Scoring;
        }
        break;

    case Swapping: {
        bool done1 = firstGem ? firstGem->moveGem(deltaTime) : true;
        bool done2 = secondGem ? secondGem->moveGem(deltaTime) : true;

        if (done1 && done2) {
            swap(grid[firstRow][firstCol], grid[secondRow][secondCol]);

            grid[firstRow][firstCol].setGridPositions(firstRow, firstCol);
            grid[secondRow][secondCol].setGridPositions(secondRow, secondCol);

            findMatches();
            bool anyMatch = false;
            // CHECK: Código muy anidado, máximo 3 anidaciones
            for (int r = 0; r < ROWS && !anyMatch; r++)// CHECK: Los for deben tener brackets
                for (int c = 0; c < COLS && !anyMatch; c++)// CHECK: Los for deben tener brackets
                    if (matches[r][c]) anyMatch = true;// CHECK: Los if deben tener brackets

            if (anyMatch) {
                for (int r = 0; r < ROWS; r++)// CHECK: Los if deben tener brackets
                    for (int c = 0; c < COLS; c++)// CHECK: Los if deben tener brackets
                        if (matches[r][c]) grid[r][c].startDisappearing();// CHECK: Los if deben tener brackets
                state = Scoring;
            }
            else {
                swap(grid[firstRow][firstCol], grid[secondRow][secondCol]);
                grid[firstRow][firstCol].setGridPositions(firstRow, firstCol);
                grid[secondRow][secondCol].setGridPositions(secondRow, secondCol);

                firstGem = &grid[firstRow][firstCol];
                secondGem = &grid[secondRow][secondCol];
                firstGem->setDestination(swapOrigPos1);
                secondGem->setDestination(swapOrigPos2);

                playerInitiatedMove = false;
                state = Reverting;
            }
        }
        break;
    }

    case Reverting: {
        bool done1 = firstGem ? firstGem->moveGem(deltaTime) : true;
        bool done2 = secondGem ? secondGem->moveGem(deltaTime) : true;
        if (done1 && done2) {
            firstGem = secondGem = nullptr;
            state = Idle;
        }
        break;
    }

    case Scoring: {
        bool anyStillAnimating = false;

        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                // CHECK: Código muy anidado, máximo 3 anidaciones
                if (grid[r][c].getDisappearingState()) {
                    if (grid[r][c].dissapear(deltaTime)) {
                        anyStillAnimating = true;
                    }
                }
            }
        }

        if (!anyStillAnimating) {
            scoreGained = clearMatches();
            moveConsumed = playerInitiatedMove;
            playerInitiatedMove = false;
            applyGravity();
            refill();
            state = Moving;
        }
        break;
    }

    case Moving: {
        bool stillMoving = false;
        // CHECK: Los for deben tener brackets
        for (int r = 0; r < ROWS; r++)
        // CHECK: Los for deben tener brackets
            for (int c = 0; c < COLS; c++)
                // CHECK: Código muy anidado, máximo 3 anidaciones
                if (!grid[r][c].moveGem(deltaTime)) stillMoving = true;

        if (!stillMoving) {
            findMatches();
            bool anyMatch = false;
            // CHECK: Código muy anidado, máximo 3 anidaciones
            // CHECK: Los for deben tener brackets
            for (int r = 0; r < ROWS && !anyMatch; r++)
            // CHECK: Los for deben tener brackets
                for (int c = 0; c < COLS && !anyMatch; c++)
                    if (matches[r][c]) anyMatch = true;

            if (anyMatch) {
                // CHECK: Los for deben tener brackets
                for (int r = 0; r < ROWS; r++)
                // CHECK: Los for deben tener brackets
                    for (int c = 0; c < COLS; c++)
                // CHECK: Los if deben tener brackets
                        if (matches[r][c]) grid[r][c].startDisappearing();
                playerInitiatedMove = false;
                state = Scoring;
            }
            else {
                state = Idle;
            }
        }
        break;
    }

    }
}

void Board::findMatches() {
    // CHECK: Los for deben tener brackets
    for (int r = 0; r < ROWS; r++)
    // CHECK: Los for deben tener brackets
        for (int c = 0; c < COLS; c++)
            matches[r][c] = false;

    for (int r = 0; r < ROWS; r++) {
        int count = 1;
        for (int c = 1; c < COLS; c++) {
            int cur = grid[r][c].getKind();
            int prev = grid[r][c - 1].getKind();
            // CHECK: Código muy anidado, máximo 3 anidaciones
            // CHECK: Los for deben tener brackets
            if (cur >= 0 && prev >= 0 && cur == prev)
                count++;
            else {
                // CHECK: Los for deben tener brackets
                if (count >= 3)
                // CHECK: Los for deben tener brackets
                    for (int k = 0; k < count; k++) matches[r][c - 1 - k] = true;
                count = 1;
            }
        }
        // CHECK: Los if deben tener brackets
        if (count >= 3)
        // CHECK: Los for deben tener brackets
            for (int k = 0; k < count; k++) matches[r][COLS - 1 - k] = true;
    }

    for (int c = 0; c < COLS; c++) {
        int count = 1;
        for (int r = 1; r < ROWS; r++) {
            int cur = grid[r][c].getKind();
            int prev = grid[r - 1][c].getKind();
            // CHECK: Código muy anidado, máximo 3 anidaciones
            // CHECK: Los if deben tener brackets
            if (cur >= 0 && prev >= 0 && cur == prev)
                count++;
            else {
                // CHECK: Los if deben tener brackets
                if (count >= 3)
                // CHECK: Los for deben tener brackets
                    for (int k = 0; k < count; k++) matches[r - 1 - k][c] = true;
                count = 1;
            }
        }
        // CHECK: Los if deben tener brackets
        if (count >= 3)
        // CHECK: Los for deben tener brackets
            for (int k = 0; k < count; k++) matches[ROWS - 1 - k][c] = true;
    }
}

int Board::clearMatches() {
    int score = 0;
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (matches[r][c]) {
                grid[r][c].setKind(-1);
                grid[r][c].getSprite().setColor(Color(255, 255, 255, 0));
                score += 10;
            }
        }
    }
    return score;
}

void Board::applyGravity() {
    for (int c = 0; c < COLS; c++) {
        int writeRow = ROWS - 1;
        for (int r = ROWS - 1; r >= 0; r--) {
            // CHECK: Código muy anidado, máximo 3 anidaciones
            if (grid[r][c].getKind() != -1) {
                if (r != writeRow) {
                    grid[writeRow][c] = grid[r][c];
                    grid[writeRow][c].resetTransientState();
                    grid[writeRow][c].setGridPositions(writeRow, c);
                    Vector2f dest(c * TILE_SIZE + offset.x,
                        writeRow * TILE_SIZE + offset.y);
                    grid[writeRow][c].setDestination(dest);
                    grid[r][c].setKind(-1);
                    grid[r][c].getSprite().setPosition(c * TILE_SIZE + offset.x, r * TILE_SIZE + offset.y);
                }
                writeRow--;
            }
        }
    }
}

void Board::refill() {
    for (int c = 0; c < COLS; c++) {
        for (int r = ROWS - 1; r >= 0; r--) {
            // CHECK: Código muy anidado, máximo 3 anidaciones
            if (grid[r][c].getKind() == -1) {
                int kind = rand() % 5;
                grid[r][c] = Gem(kind, r, c);
                grid[r][c].setSprite(texture);

                grid[r][c].resetTransientState();
                grid[r][c].setGridPositions(r, c);

                Vector2f spawn(c * TILE_SIZE + offset.x, -TILE_SIZE + offset.y);
                grid[r][c].getSprite().setPosition(spawn);

                Vector2f destination(c * TILE_SIZE + offset.x,
                    r * TILE_SIZE + offset.y);
                grid[r][c].setDestination(destination);
            }
        }
    }
}


int Board::getState() const {
    return static_cast<int>(state);
}

Gem& Board::getGem(int row, int col) {
    return grid[row][col];
}

