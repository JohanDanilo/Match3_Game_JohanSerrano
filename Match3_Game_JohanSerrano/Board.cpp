#include "Board.h"

Board::Board() {
	cout << "Inicializando tablero..." << endl;
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
    clearObstacles();
}

void Board::initialize() {

    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            // NOTE: Code too nested
            if (grid[r][c] != nullptr) {
                delete grid[r][c];
                grid[r][c] = nullptr;
            }
        }
    }

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

    state = Idle;
    playerInitiatedMove = false;
    firstGem = nullptr;
    secondGem = nullptr;
    firstRow = firstCol = secondRow = secondCol = -1;
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
    // NOTE: Complete this code with try catch, it is important deal with errors
    texture.loadFromFile("assets/spritesheet.png");
}

void Board::draw(RenderWindow& window) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            grid[i][j]->draw(window);
        }
    }

    for (Obstacle* obs : obstacles) {
        if (obs && !obs->isDestroyedState()) {
            obs->draw(window);
        }
    }
}

bool Board::areAdjacent(int row1, int col1, int row2, int col2) const {
    if (row1 == row2 && col1 == col2) { return false; }
    return (abs(row1 - row2) + abs(col1 - col2) == 1);
}

bool Board::trySwapIndices(int row1, int col1, int row2, int col2) {
    // NOTE: use var to define the complex evaluation
    if (row1 < 0 || row1 >= ROWS || col1 < 0 || col1 >= COLS ||
        row2 < 0 || row2 >= ROWS || col2 < 0 || col2 >= COLS) {
        return false;
    }

    if (hasObstacleAt(row1, col1) || hasObstacleAt(row2, col2)) {
        return false;
    }

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
        activateSpecialGemsInMatches();
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

        findMatches();

        if (checkAnyMatch()) {
			activateSpecialGemsInMatches();
            triggerDisappearance();
            state = Scoring;
        }
        else {
			// NO HAY MATCH: Revertir el swap
			cout << "[DEBUG] No hay matches, revirtiendo swap" << endl;
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
            // NOTE: Code too nested
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
            // NOTE: Code too nested
            if (!grid[r][c]->moveGem(deltaTime)) {
                stillMoving = true;
            }
        }
    }

    if (!stillMoving) {
        findMatches();
        if (checkAnyMatch()) {

			activateSpecialGemsInMatches();

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
            // NOTE: Code too nested
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
            // NOTE: Code too nested
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
                // NOTE: Code too nested
                if (count >= 3) {
                    markMatches(horizontal, outer, inner - 1, count);
                }
                count = 1;
                continue;
            }

            int cur = grid[r1][c1]->getKind();
            int prev = grid[r0][c0]->getKind();

            bool same = (cur == prev);

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

    // --- PASO 1: detectar si hay gemas especiales en la secuencia ---
    bool containsSpecial = false;
    for (int k = 0; k < count; k++) {
        int r = horizontal ? outer : lastIndex - k;
        int c = horizontal ? lastIndex - k : outer;

        if (grid[r][c] && grid[r][c]->getType() != "Normal") {
            containsSpecial = true;
            break;
        }
    }

    // --- PASO 2: calcular la posición donde nacería la especial ---
    if (count >= 4 && !containsSpecial) { // solo si NO hay especiales
        auto inSequence = [&](int r, int c) -> bool {
            if (horizontal)
                return (r == outer && c >= startIndex && c <= lastIndex);
            else
                return (c == outer && r >= startIndex && r <= lastIndex);
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

        // Crear la gema especial solo si la celda es Normal
        if (grid[destRow][destCol] && grid[destRow][destCol]->getType() == "Normal") {
            spawnSpecialGem(destRow, destCol, horizontal);
        }
    }

    // --- PASO 4: marcar las demás gemas del match para eliminar ---
    for (int k = 0; k < count; ++k) {
        int r = horizontal ? outer : lastIndex - k;
        int c = horizontal ? lastIndex - k : outer;

        if (r < 0 || r >= ROWS || c < 0 || c >= COLS)
            continue;

        if (r == destRow && c == destCol)
            continue;

        matches[r][c] = true;
    }
}

int Board::clearMatches() {
    int score = 0;
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            // NOTE: Code too nested
            if (matches[r][c] && grid[r][c]) {

                cout << "[DELETE] Eliminando gema en (" << r << "," << c << ") PTR: "
                    << grid[r][c] << " Kind: " << grid[r][c]->getKind()
                    << " Tipo: " << grid[r][c]->getType() << endl;

                updateObjectivesOnMatch(r, c);

                damageAdjacentObstacles(r, c);

                grid[r][c]->setKind(-1);
                grid[r][c]->getSprite().setColor(Color(255, 255, 255, 0));
                score += 10;
            }
        }
    }
    return score;
}

void Board::updateObjectivesOnMatch(int row, int col) {
    if (!currentLevel) return;

    Objective* objective = currentLevel->getObjective();
    if (!objective) return;

    Gem* gem = grid[row][col];
    if (!gem) return;

    int gemKind = gem->getKind();

    if (objective->getType() == ObjectiveType::CollectGems) {
        if (objective->getGemKind() == gemKind) {
            objective->addProgress(1);
        }
    }
}

void Board::damageAdjacentObstacles(int row, int col) {
    int dr[] = { -1, 1, 0, 0 };
    int dc[] = { 0, 0, -1, 1 };

    for (int i = 0; i < 4; i++) {
        int nr = row + dr[i];
        int nc = col + dc[i];

        if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS) {
            // NOTE: Code too nested
            for (Obstacle* obs : obstacles) {
                if (obs && !obs->isDestroyedState()) {
                    if (obs->getRow() == nr && obs->getCol() == nc) {
                        obs->takeDamage();

                        if (obs->isDestroyedState() && currentLevel) {
                            Objective* objective = currentLevel->getObjective();
                            if (objective && objective->getType() == ObjectiveType::ClearObstacles) {
                                objective->addProgress(1);
                            }
                        }
                    }
                }
            }
        }
    }
}

bool Board::hasObstacleAt(int row, int col) const {
    for (Obstacle* obs : obstacles) {
        if (obs && !obs->isDestroyedState()) {
            // NOTE: Code too nested
            if (obs->getRow() == row && obs->getCol() == col) {
                return true;
            }
        }
    }
    return false;
}

void Board::applyGravity() {
    cout << "\n[GRAVITY] Aplicando gravedad...\n";

    for (int c = 0; c < COLS; ++c) {
        for (int r = ROWS - 1; r >= 0; --r) {

            Gem* current = grid[r][c];

            // Si la celda ya está ocupada por una gema viva, saltar
            if (current && current->getKind() != -1) {
                continue;
            }

            // Buscar la primera gema viva arriba que pueda caer
            int above = r - 1;
            while (above >= 0) {

                Gem* upper = grid[above][c];

                if (!upper) {
                    // No hay gema arriba, seguir buscando
                    above--;
                    continue;
                }

                if (upper->getKind() == -1) {
                    // Gema muerta: continuar buscando más arriba
                    above--;
                    continue;
                }

                // Gema válida encontrada → hacerla caer
                swap(grid[r][c], grid[above][c]);

                Gem* fallingGem = grid[r][c];
                fallingGem->resetTransientState();
                fallingGem->setGridPositions(r, c);

                Vector2f dest(c * TILE_SIZE + offset.x, r * TILE_SIZE + offset.y);
                fallingGem->setDestination(dest);

                cout << "[MOVE] Gema bajando de (" << above << "," << c << ") a (" << r << "," << c << ") "
                    << "PTR: " << fallingGem
                    << " Kind: " << fallingGem->getKind()
                    << " Tipo: " << fallingGem->getType() << endl;

                // Marcar la celda original como vacía
                grid[above][c]->setKind(-1);
                grid[above][c]->getSprite().setPosition(
                    c * TILE_SIZE + offset.x, above * TILE_SIZE + offset.y);
                grid[above][c]->getSprite().setColor(Color(255, 255, 255, 0));
                break;
            }

            // Si no se encontró ninguna gema viva arriba
            if (above < 0) {
                // Dejar la celda lista para refill
                if (grid[r][c]) {
                    grid[r][c]->setKind(-1);
                    grid[r][c]->getSprite().setColor(Color(255, 255, 255, 0));
                }
                else {
                    grid[r][c] = nullptr;
                }

                cout << "[EMPTY] Celda vacía en (" << r << "," << c << "), lista para refill\n";
            }
        }
    }

    cout << "[GRAVITY] Finalizado.\n";
}


void Board::refill() {
    for (int c = 0; c < COLS; c++) {
        for (int r = ROWS - 1; r >= 0; r--) {
            // NOTE: Code too nested
            if (grid[r][c]->getKind() != -1) {
                continue;
            }

            if (hasObstacleAt(r, c)) {
                continue;
            }

            spawnGem(r, c);
        }
    }
}


void Board::spawnGem(int r, int c) {
    // Si ya existe una gema "muerta" (-1), eliminar antes de crear una nueva
    if (grid[r][c]) {
        delete grid[r][c];
        grid[r][c] = nullptr;
    }

    // Crear una gema normal aleatoria
    int kind = rand() % 5;
    grid[r][c] = new NormalGem(kind, r, c);

    // Asignar textura y restablecer estado transitorio
    grid[r][c]->setSprite(texture);
    grid[r][c]->resetTransientState();
    grid[r][c]->setGridPositions(r, c);

    // Posición inicial: aparece desde arriba del tablero
    Vector2f spawn(c * TILE_SIZE + offset.x, -TILE_SIZE + offset.y);
    grid[r][c]->getSprite().setPosition(spawn);

    // Posición final (destino)
    Vector2f destination(c * TILE_SIZE + offset.x, r * TILE_SIZE + offset.y);
    grid[r][c]->setDestination(destination);

    // DEBUG
    cout << "[SPAWN] Nueva gema creada en (" << r << "," << c << ") "
        << "PTR: " << grid[r][c]
        << " Kind: " << grid[r][c]->getKind()
        << " Tipo: " << grid[r][c]->getType() << endl;
}


void Board::spawnSpecialGem(int row, int col, bool horizontal) {
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS) return;

    Gem* target = grid[row][col];
    if (!target) return;

    // Si la gema ya es especial, no volver a crearla
    string currentType = target->getType();
    if (currentType == "Bomb" || currentType == "Ice") {
        cout << "[INFO] Gema especial ya existente en (" << row << "," << col << "), no se reemplaza.\n";
        return;
    }

    int kind = target->getKind();

    // Eliminar la gema anterior (seguro)
    delete grid[row][col];
    grid[row][col] = nullptr;

    // Crear la nueva gema especial
    if (horizontal) {
        grid[row][col] = new BombGem(kind, row, col);
    }
    else {
        grid[row][col] = new IceGem(kind, row, col);
    }

    // Configurar sprite y posición
    grid[row][col]->setSprite(texture);
    grid[row][col]->resetTransientState();
    grid[row][col]->setGridPositions(row, col);

    Vector2f dest(col * TILE_SIZE + offset.x, row * TILE_SIZE + offset.y);
    grid[row][col]->getSprite().setPosition(dest);
    grid[row][col]->setDestination(dest);

    cout << "[SPAWN SPECIAL] Nueva gema especial en (" << row << "," << col << ") "
        << "Tipo: " << grid[row][col]->getType()
        << " Kind: " << grid[row][col]->getKind()
        << " PTR: " << grid[row][col] << endl;
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
            // NOTE: Code too nested
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

void Board::setCurrentLevel(Level* level) {
    currentLevel = level;
}

void Board::placeObstacles(int count) {
    clearObstacles();

    for (int i = 0; i < count; i++) {
        bool placed = false;
        int attempts = 0;

        while (!placed && attempts < 50) {
            int r = rand() % ROWS;
            int c = rand() % COLS;

            // NOTE: Code too nested
            if (!hasObstacleAt(r, c)) {
                IronBlock* iron = new IronBlock(r, c);
                obstacles.push_back(iron);
                placed = true;
            }
            attempts++;
        }
    }
}

void Board::clearObstacles() {
    for (Obstacle* obs : obstacles) {
        delete obs;
    }
    obstacles.clear();
}

void Board::updateScoreObjective(int scoreGained) {
    if (!currentLevel) return;

    Objective* objective = currentLevel->getObjective();
    if (objective && objective->getType() == ObjectiveType::ReachScore) {
        objective->addProgress(scoreGained);
    }
}

void Board::clearCurrentLevel() {
    currentLevel = nullptr;

    for (Obstacle* obs : obstacles) {
        if (obs) {
            delete obs;
        }
    }
    obstacles.clear();
}

void Board::activateSpecialGemsInMatches() {
    // Recorrer todas las celdas que tienen matches
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (matches[r][c] && grid[r][c]) {
                string type = grid[r][c]->getType();

                if (type == "Bomb") {
                    cout << "[SPECIAL AUTO] Activando Bomb en (" << r << ", " << c << ")" << endl;
                    activateBombEffect(r, c);
                }
                else if (type == "Ice") {
                    cout << "[SPECIAL AUTO] Activando Ice en fila " << r << endl;
                    activateIceEffect(r);
                }
            }
        }
    }
}