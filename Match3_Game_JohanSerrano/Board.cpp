#include "Board.h"

Vector2f Board::getWindowPosition(RenderWindow& window)
{
    Vector2i position = Mouse::getPosition(window);
    Vector2f worldPosition = window.mapPixelToCoords(position);
    return worldPosition;
}

void Board::initialize()
{
	srand(static_cast<unsigned int>(time(0)));

    loadTexture();

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {

            int kind = rand() % 5, row = i, col = j;
            grid[i][j] = Gem(kind, row, col);
            grid[i][j].setSprite(texture);
        }
    }

}

void Board::loadTexture()
{
    texture.loadFromFile("assets/spritesheet.png");
    return;
}


void Board::draw(RenderWindow& window)
{
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < 8; j++) {
            grid[i][j].draw(window);
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

bool Board::isInBounds(RenderWindow& window)
{

    Vector2f position = getWindowPosition(window);
    float col = (position.x - offset.x) / TILE_SIZE;
    float row = (position.y - offset.y) / TILE_SIZE;

    if (col < 0 || col >= COLS || row < 0 || row >= ROWS) {
        return false;
    }
    return true;
}

void Board::prepareSwap(RenderWindow& window) {
    int row1 = 0, col1 = 0, row2 = 0, col2 = 0;
    Vector2f position;

    if (!firstGem && isInBounds(window)) {
        // Primer click
        position = getWindowPosition(window);
        col1 = static_cast<int>((position.x - offset.x) / TILE_SIZE);
        row1 = static_cast<int>((position.y - offset.y) / TILE_SIZE);
        firstGem = &grid[row1][col1];
        cout << "First gem selected in grid: " << firstGem->getRow() << ", " << firstGem->getColum() << endl;
    }
    else if (isInBounds(window)) {
        // Segundo click
        position = getWindowPosition(window);
        col2 = static_cast<int>((position.x - offset.x) / TILE_SIZE);
        row2 = static_cast<int>((position.y - offset.y) / TILE_SIZE);

        secondGem = &grid[row2][col2];
        cout << "Second gem selected in grid: " << secondGem->getRow() << ", " << secondGem->getColum() << endl;

        if (areAdjacent()) {
            // NO restar moves aquí
            originalPos1 = firstGem->getSprite().getPosition();
            originalPos2 = secondGem->getSprite().getPosition();

            firstGem->setDestination(originalPos2);
            secondGem->setDestination(originalPos1);

            isReverting = false;    // asegurar estado
            isSwapping = true;

            cout << "And they're adjacent, so they have to move" << endl << endl;
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
        // Si estábamos en la animación de revert y ya terminó:
        if (isReverting) {
            // revert terminado: limpiar y salir
            isReverting = false;
            isSwapping = false;
            firstGem = nullptr;
            secondGem = nullptr;
            cout << "Revert animation finished" << endl;
            return;
        }

        int r1 = firstGem->getRow();
        int c1 = firstGem->getColum();
        int r2 = secondGem->getRow();
        int c2 = secondGem->getColum();

        // Intercambiar lógicamente las piezas (ahora sus sprites ya están en el lugar)
        swap(grid[r1][c1], grid[r2][c2]);
        grid[r1][c1].setGridPositions(r1, c1);
        grid[r2][c2].setGridPositions(r2, c2);

        // Buscar matches resultantes del swap
        findMatches();

        bool anyMatch = false;
        for (int i = 0; i < ROWS && !anyMatch; i++) {
            for (int j = 0; j < COLS && !anyMatch; j++) {
                if (matches[i][j]) anyMatch = true;
            }
        }

        if (!anyMatch) {
            // No match -> revertir visualmente (animación)
            // restaurar lógica del grid para que el array represente el tablero visual
            swap(grid[r1][c1], grid[r2][c2]);
            grid[r1][c1].setGridPositions(r1, c1);
            grid[r2][c2].setGridPositions(r2, c2);

            // animar de vuelta a sus posiciones originales
            firstGem->setDestination(originalPos1);
            secondGem->setDestination(originalPos2);

            // Mantener punteros y dejar que la animación de revert continue en updateSwap
            isReverting = true;
            isSwapping = true;
            cout << "Swap reverted: no match found (animating revert)" << endl;

            // NO limpiar firstGem/secondGem aquí; los limpias cuando termine la animación de revert.
            return;
        }
        else {
            // Hay match: iniciar desaparición en las celdas marcadas
            for (int i = 0; i < ROWS; i++) {
                for (int j = 0; j < COLS; j++) {
                    if (matches[i][j]) {
                        grid[i][j].startDisappearing();
                    }
                }
            }

            pendingScore = true;
            scoreFromSwap = true;   // este match fue provocado por el jugador

            // Liberamos punteros para que update() continúe con desaparición/gravedad/refill
            firstGem = nullptr;
            secondGem = nullptr;
            isSwapping = false;

            cout << "Match found -> pendingScore = true" << endl;
            return;
        }
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

    stillMoving = false;

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (!grid[i][j].moveGem(dt)) {
                stillMoving = true;
            }
        }
    }

    if (!stillMoving) {

        // Detectar matches espontáneos (ej. cascada tras refill) y arrancar su desaparición.
        findMatches();
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                if (matches[i][j]) {
                    grid[i][j].startDisappearing();
                    matches[i][j] = false;
                    // Si se detecta un match aquí y no venía de un swap, también queremos contabilizarlo:
                    pendingScore = true;
                }
            }
        }

        // Revisar si hay gemas actualmente desapareciendo (animación en curso)
        bool anyDisappearing = false;
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                if (grid[i][j].getDisappearingState()) {
                    if (grid[i][j].dissapear()) {
                        anyDisappearing = true;
                    }
                }
            }
        }

        // Si no hay ninguna gema desapareciendo -> aplicar pipeline (clear -> gravedad -> refill)
        if (!anyDisappearing) {
            if (pendingScore) {
                clearMatches();
                pendingScore = false;

                // Solo restar un movimiento si el match vino de un swap del jugador
                if (scoreFromSwap) {
                    moves--;
                    scoreFromSwap = false;  // reseteamos
                }

                cout << "Score applied. New score: " << score << " Moves left: " << moves << endl;

                applyGravity();
                refill();
                scoring = true;
            }
            else {
                scoring = false;
            }
        }

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

void Board::clearMatches() {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            // Si la gema ya terminó de desaparecer : marcar como vacía
            if (grid[r][c].isEmpty()) {

                score += 10;
                grid[r][c].setKind(-1);
            }
        }
    }
}

void Board::applyGravity() {
    for (int c = 0; c < COLS; c++) {
        int writeRow = ROWS - 1; // posición donde debe caer la próxima gema
        for (int r = ROWS - 1; r >= 0; r--) {
            if (grid[r][c].getKind() != -1) {
                if (r != writeRow) {
                    // Mover gema hacia abajo
                    grid[writeRow][c] = grid[r][c];
                    grid[writeRow][c].setGridPositions(writeRow, c);

                    Vector2f dest(c * TILE_SIZE + offset.x, writeRow * TILE_SIZE + offset.y);
                    grid[writeRow][c].setDestination(dest);

                    // Marcar la celda original como vacía
                    grid[r][c].setKind(-1);
                }
                writeRow--;
            }
        }
    }
}

void Board::refill() {
    for (int c = 0; c < COLS; c++) {
        for (int r = 0; r < ROWS; r++) {
            if (grid[r][c].getKind() == -1) {
                int kind = rand() % 5;
                grid[r][c] = Gem(kind, r, c);
                grid[r][c].setSprite(texture);

                // Aparece por encima del tablero
                grid[r][c].getSprite().setPosition(c * TILE_SIZE + offset.x, -TILE_SIZE + offset.y);

                // Destino: su celda real
                grid[r][c].setDestination(Vector2f(c * TILE_SIZE + offset.x, r * TILE_SIZE + offset.y));
            }
        }
    }
}

int Board::getScore()
{
    return score;
}

int Board::getMoves()
{
    return moves;
}

void Board::refillMoves()
{
    moves = 20;
}

void Board::clearScore()
{
    score = 0;
}

bool Board::isResolving() const {
    return isSwapping || stillMoving || pendingScore || isReverting;
}


bool Board::isScoring() const
{
    return scoring;
}
