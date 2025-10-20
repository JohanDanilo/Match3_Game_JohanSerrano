#include "Board.h"

Board::Board() {
    loadTexture();
    initialize();
    state = Idle;
}

Board::~Board() {
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            delete grid[r][c];
            grid[r][c] = nullptr;
        }
    }
}

void Board::initialize() {
    srand(static_cast<unsigned int>(time(0)));

    int totalCells = ROWS * COLS;

    for (int idx = 0; idx < totalCells; idx++) {
        int i = idx / COLS;
        int j = idx % COLS;

        int kind = rand() % 5;

        while (createsMatch(i, j, kind)) {
            kind = rand() % 5;
        }

        grid[i][j] = new NormalGem(kind, i, j);
        grid[i][j]->setSprite(texture);
        grid[i][j]->setGridPositions(i, j);

        Vector2f dest(j * TILE_SIZE + offset.x, i * TILE_SIZE + offset.y);
        grid[i][j]->setDestination(dest);
    }

}



bool Board::createsMatch(int row, int col, int kind) {

    if (col >= 2 && grid[row][col - 1]->getKind() == kind && grid[row][col - 2]->getKind() == kind) {
        return true;
    }

    if (row >= 2 && grid[row - 1][col]->getKind() == kind && grid[row - 2][col]->getKind() == kind) {
        return true;
    }

    return false;
}


void Board::loadTexture() {
    texture.loadFromFile("assets/spritesheet.png");
}

void Board::draw(RenderWindow& window) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            grid[i][j]->draw(window);
        }
    }
}

bool Board::areAdjacent(int row1, int col1, int row2, int col2) const {
    if (row1 == row2 && col1 == col2) { return false; }
    return (abs(row1 - row2) + abs(col1 - col2) == 1);
}

bool Board::trySwapIndices(int row1, int col1, int row2, int col2) {
    if (row1 < 0 || row1 >= ROWS || col1 < 0 || col1 >= COLS ||
        row2 < 0 || row2 >= ROWS || col2 < 0 || col2 >= COLS) { return false; }
    if (!areAdjacent(row1, col1, row2, col2)) { return false; }

    firstRow = row1; firstCol = col1;
    secondRow = row2; secondCol = col2;

    firstGem = grid[firstRow][firstCol];
    secondGem = grid[secondRow][secondCol];

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

    switch (state) {
    case Idle:
        handleIdleState();
        break;

    case Swapping:
        handleSwappingState(deltaTime, moveConsumed);
        break;

    case Reverting:
        handleRevertingState(deltaTime);
        break;

    case Scoring:
        handleScoringState(deltaTime, scoreGained, moveConsumed);
        break;

    case Moving:
        handleMovingState(deltaTime);
        break;
    }
}


void Board::handleIdleState() {
    findMatches();
    if (checkAnyMatch()) {
        triggerDisappearance();
        playerInitiatedMove = false;
        state = Scoring;
    }
}

void Board::handleSwappingState(float deltaTime, bool& moveConsumed) {
    bool done1 = firstGem ? firstGem->moveGem(deltaTime) : true;
    bool done2 = secondGem ? secondGem->moveGem(deltaTime) : true;

    if (done1 && done2) {
        swap(grid[firstRow][firstCol], grid[secondRow][secondCol]);
        grid[firstRow][firstCol]->setGridPositions(firstRow, firstCol);
        grid[secondRow][secondCol]->setGridPositions(secondRow, secondCol);

        if (firstGem) {
            string type1 = firstGem->getType();
            if (type1 == "Bomb") {
                activateBombEffect(secondRow, secondCol);
                playerInitiatedMove = false;
                moveConsumed = true;
                state = Scoring;
                return;
            }
            else if (type1 == "Ice") {
                activateIceEffect(secondRow);
                playerInitiatedMove = false;
                moveConsumed = true;
                state = Scoring;
                return;
            }
        }

        if (secondGem) {
            string type2 = secondGem->getType();
            if (type2 == "Bomb") {
                activateBombEffect(firstRow, firstCol);
                playerInitiatedMove = false;
                moveConsumed = true;
                state = Scoring;
                return;
            }
            else if (type2 == "Ice") {
                activateIceEffect(firstRow);
                playerInitiatedMove = false;
                moveConsumed = true;
                state = Scoring;
                return;
            }
        }


        findMatches();
        if (checkAnyMatch()) {
            triggerDisappearance();
            state = Scoring;
        }
        else {
            revertSwap();
        }
    }
}


void Board::handleRevertingState(float deltaTime) {
    bool done1 = firstGem ? firstGem->moveGem(deltaTime) : true;
    bool done2 = secondGem ? secondGem->moveGem(deltaTime) : true;

    if (done1 && done2) {
        firstGem = nullptr;
        secondGem = nullptr;
        state = Idle;
    }
}


void Board::handleScoringState(float deltaTime, int& scoreGained, bool& moveConsumed) {
    bool anyStillAnimating = false;

    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (grid[r][c]->getDisappearingState()) {
                if (grid[r][c]->dissapear(deltaTime)) {
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
}


void Board::handleMovingState(float deltaTime) {
    bool stillMoving = false;

    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (!grid[r][c]->moveGem(deltaTime)) {
                stillMoving = true;
            }
        }
    }

    if (!stillMoving) {
        findMatches();
        if (checkAnyMatch()) {
            triggerDisappearance();
            playerInitiatedMove = false;
            state = Scoring;
        }
        else {
            state = Idle;
        }
    }
}


bool Board::checkAnyMatch() {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (matches[r][c]) {
                return true;
            }
        }
    }
    return false;
}


void Board::triggerDisappearance() {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (matches[r][c]) {
                grid[r][c]->startDisappearing();
            }
        }
    }
}


void Board::revertSwap() {
    swap(grid[firstRow][firstCol], grid[secondRow][secondCol]);
    grid[firstRow][firstCol]->setGridPositions(firstRow, firstCol);
    grid[secondRow][secondCol]->setGridPositions(secondRow, secondCol);

    firstGem = grid[firstRow][firstCol];
    secondGem = grid[secondRow][secondCol];
    firstGem->setDestination(swapOrigPos1);
    secondGem->setDestination(swapOrigPos2);

    playerInitiatedMove = false;
    state = Reverting;
}


void Board::findMatches() {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            matches[r][c] = false;
        }
    }

    checkLineMatches(true);
    checkLineMatches(false);
}

void Board::checkLineMatches(bool horizontal) {
    int outerLimit = ROWS;
    int innerLimit = COLS;

    for (int outer = 0; outer < outerLimit; outer++) {
        int count = 1;

        for (int inner = 1; inner < innerLimit; inner++) {
            int r1 = horizontal ? outer : inner;
            int c1 = horizontal ? inner : outer;
            int r0 = horizontal ? outer : inner - 1;
            int c0 = horizontal ? inner - 1 : outer;

            if (!grid[r1][c1] || !grid[r0][c0]) {
                if (count >= 3) {
                    markMatches(horizontal, outer, inner - 1, count);
                }
                count = 1;
                continue;
            }

            int cur = grid[r1][c1]->getKind();
            int prev = grid[r0][c0]->getKind();

            bool bothNormal = (grid[r1][c1]->getType() == "Normal" && grid[r0][c0]->getType() == "Normal");

            bool same = (cur >= 0 && prev >= 0 && cur == prev && bothNormal);

            if (same) {
                count++;
            }
            else {
                if (count >= 3) {
                    markMatches(horizontal, outer, inner - 1, count);
                }
                count = 1;
            }
        }

        if (count >= 3) {
            markMatches(horizontal, outer, innerLimit - 1, count);
        }
    }
}


void Board::markMatches(bool horizontal, int outer, int lastIndex, int count) {
    int startIndex = lastIndex - count + 1;

    int destRow = -1;
    int destCol = -1;

    if (count >= 4) {

        auto inSequence = [&](int r, int c) -> bool {
            if (horizontal) {
                return (r == outer && c >= startIndex && c <= lastIndex);
            }
            else {
                return (c == outer && r >= startIndex && r <= lastIndex);
            }
            };

        if (playerInitiatedMove) {
            if (inSequence(secondRow, secondCol)) {
                destRow = secondRow;
                destCol = secondCol;
            }
            else if (inSequence(firstRow, firstCol)) {
                destRow = firstRow;
                destCol = firstCol;
            }
        }

        if (destRow == -1 || destCol == -1) {
            if (horizontal) {
                destRow = outer;
                destCol = lastIndex - (count / 2);
            }
            else {
                destRow = lastIndex - (count / 2);
                destCol = outer;
            }
        }

        spawnSpecialGem(destRow, destCol, horizontal);
    }

    for (int k = 0; k < count; ++k) {
        int r = horizontal ? outer : lastIndex - k;
        int c = horizontal ? lastIndex - k : outer;

        if (r < 0 || r >= ROWS || c < 0 || c >= COLS) {
            continue;
        }

        if (r == destRow && c == destCol) {
            continue;
        }

        matches[r][c] = true;
    }

}



int Board::clearMatches() {
    int score = 0;
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (matches[r][c]) {
                grid[r][c]->setKind(-1);
                grid[r][c]->getSprite().setColor(Color(255, 255, 255, 0));
                score += 10;
            }
        }
    }
    return score;
}

void Board::applyGravity() {
    for (int c = 0; c < COLS; c++) {
        for (int r = ROWS - 1; r >= 0; r--) {

            Gem* current = grid[r][c];
            if (!current || current->getKind() != -1) {
                continue;
            }

            int above = r - 1;
            while (above >= 0) {
                Gem* upper = grid[above][c];
                if (upper && upper->getKind() != -1) {
                    swap(grid[r][c], grid[above][c]);

                    Gem* fallingGem = grid[r][c];
                    fallingGem->resetTransientState();
                    fallingGem->setGridPositions(r, c);
                    Vector2f dest(c * TILE_SIZE + offset.x, r * TILE_SIZE + offset.y);
                    fallingGem->setDestination(dest);

                    grid[above][c]->setKind(-1);
                    grid[above][c]->getSprite().setPosition(
                        c * TILE_SIZE + offset.x, above * TILE_SIZE + offset.y);
                    grid[above][c]->getSprite().setColor(Color(255, 255, 255, 0));
                    break;
                }
                above--;
            }
        }
    }
}


void Board::refill() {

    for (int c = 0; c < COLS; c++) {
        for (int r = ROWS - 1; r >= 0; r--) {
            if (grid[r][c]->getKind() != -1) {
                continue;
            }
            spawnGem(r, c);
        }
    }

}

void Board::spawnGem(int r, int c) {
    int kind = rand() % 5;
    grid[r][c] = new NormalGem(kind, r, c);
    grid[r][c]->setSprite(texture);

    grid[r][c]->resetTransientState();
    grid[r][c]->setGridPositions(r, c);

    Vector2f spawn(c * TILE_SIZE + offset.x, -TILE_SIZE + offset.y);
    grid[r][c]->getSprite().setPosition(spawn);

    Vector2f destination(c * TILE_SIZE + offset.x,
        r * TILE_SIZE + offset.y);
    grid[r][c]->setDestination(destination);
}

void Board::spawnSpecialGem(int row, int col, bool horizontal) {
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS) return;

    if (!grid[row][col]) return;

    int kind = grid[row][col]->getKind();

    delete grid[row][col];
    grid[row][col] = nullptr;

    if (horizontal) {
        grid[row][col] = new BombGem(kind, row, col);
    }
    else {
        grid[row][col] = new IceGem(kind, row, col);
    }

    grid[row][col]->setSprite(texture);
    grid[row][col]->resetTransientState();
    grid[row][col]->setGridPositions(row, col);

    Vector2f dest(col * TILE_SIZE + offset.x, row * TILE_SIZE + offset.y);
    grid[row][col]->getSprite().setPosition(dest);
    grid[row][col]->setDestination(dest);
}


int Board::getState() const {
    return static_cast<int>(state);
}

Gem* Board::getGem(int row, int col) {
    return grid[row][col];
}

void Board::activateBombEffect(int row, int col) {
    const int RADIUS = 1;
    for (int r = row - RADIUS; r <= row + RADIUS; ++r) {
        for (int c = col - RADIUS; c <= col + RADIUS; ++c) {
            if (r >= 0 && r < ROWS && c >= 0 && c < COLS) {
                grid[r][c]->startDisappearing();
                matches[r][c] = true;
            }
        }
    }
}

void Board::activateIceEffect(int row) {
    for (int c = 0; c < COLS; ++c) {
        grid[row][c]->startDisappearing();
        matches[row][c] = true;
    }
}