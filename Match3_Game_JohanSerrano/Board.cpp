#include "Board.h"
#include "ResourceManager.h"

Board::Board() {
    cout << "Inicializando tablero..." << endl;
    initialize();
    state = Idle;
    inputLocked = false;
}

Board::~Board() {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (grid[r][c]) {
                delete grid[r][c];
                grid[r][c] = nullptr;
            }
        }
    }
    clearObstacles();
}

/* ===================== INICIALIZACIÓN ===================== */

void Board::initialize() {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (grid[r][c]) {
                delete grid[r][c];
                grid[r][c] = nullptr;
            }
            matches[r][c] = false;
        }
    }

    srand(static_cast<unsigned int>(time(0)));

    try {
        const Texture& texture = ResourceManager::instance().getTexture("assets/spritesheet.png");

        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                int kind = rand() % 5;
                while (createsMatch(r, c, kind)) {
                    kind = rand() % 5;
                }

                grid[r][c] = new NormalGem(kind, r, c);
                grid[r][c]->setSprite(texture);
                grid[r][c]->setGridPositions(r, c);
                grid[r][c]->setDestination(Vector2f(c * TILE_SIZE + offset.x, r * TILE_SIZE + offset.y));
            }
        }
    }
    catch (const exception& e) {
        cerr << "[ERROR] Board::initialize() - No se pudo cargar spritesheet: " << e.what() << endl;
        throw;
    }

    if (currentLevel && currentLevel->getObstacleCount() > 0) {
        placeObstacles(currentLevel->getObstacleCount());
        cout << "[DEBUG] Obstáculos generados: " << currentLevel->getObstacleCount() << endl;
    }

    state = Idle;
    playerInitiatedMove = false;
    inputLocked = false;
    firstGem = secondGem = nullptr;
    firstRow = firstCol = secondRow = secondCol = -1;
}

/* ===================== DIBUJO ===================== */

void Board::draw(RenderWindow& window) {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            if (grid[row][col] && grid[row][col]->getKind() != -1) {
                grid[row][col]->draw(window);
            }
        }
    }

    for (Obstacle* obs : obstacles) {
        if (obs && !obs->isDestroyedState()) {
            obs->draw(window);
        }
    }
}

/* ===================== ESTADOS ===================== */

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
        inputLocked = true;
    }
}

void Board::handleSwappingState(float deltaTime, bool& moveConsumed) {
    bool done1 = firstGem ? firstGem->moveGem(deltaTime) : true;
    bool done2 = secondGem ? secondGem->moveGem(deltaTime) : true;

    if (!firstGem || !secondGem) {
        cerr << "[ERROR] Swap cancelado: puntero nulo.\n";
        cout << "[DEBUG] Limpieza de selección (puntero nulo durante swap).\n";
        firstGem = nullptr;
        secondGem = nullptr;
        firstRow = firstCol = secondRow = secondCol = -1;
        state = Idle;
        inputLocked = false;
        return;
    }

    if (firstGem->getKind() == -1 || secondGem->getKind() == -1) {
        cerr << "[ERROR] Swap cancelado: gema destruida.\n";
        cout << "[DEBUG] Limpieza de selección (gema destruida durante swap).\n";
        firstGem = nullptr;
        secondGem = nullptr;
        firstRow = firstCol = secondRow = secondCol = -1;
        state = Idle;
        inputLocked = false;
        return;
    }

    if (done1 && done2) {
        swap(grid[firstRow][firstCol], grid[secondRow][secondCol]);
        grid[firstRow][firstCol]->setGridPositions(firstRow, firstCol);
        grid[secondRow][secondCol]->setGridPositions(secondRow, secondCol);

        findMatches();

        if (checkAnyMatch()) {
            activateSpecialGemsInMatches();
            triggerDisappearance();
            state = Scoring;
            cout << "[DEBUG] Limpieza de selección (swap válido con match encontrado).\n";
            firstGem = nullptr;
            secondGem = nullptr;
            firstRow = firstCol = secondRow = secondCol = -1;
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
        cout << "[DEBUG] Limpieza de selección (swap revertido).\n";
        firstGem = nullptr;
        secondGem = nullptr;
        firstRow = firstCol = secondRow = secondCol = -1;
        state = Idle;
        inputLocked = false;
        cout << "[STATE] Board ahora en Idle (listo para input).\n";
    }
}

void Board::handleScoringState(float deltaTime, int& scoreGained, bool& moveConsumed) {
    bool anyStillAnimating = false;

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            Gem* gem = grid[row][col];
            if (!gem) continue;
            if (gem->getDisappearingState() && gem->dissapear(deltaTime)) {
                anyStillAnimating = true;
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
        // seguimos con inputLocked = true
    }
}

void Board::handleMovingState(float deltaTime) {
    bool stillMoving = false;

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            if (grid[row][col] && grid[row][col]->getKind() != -1) {
                if (!grid[row][col]->moveGem(deltaTime)) {
                    stillMoving = true;
                }
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
            inputLocked = false;
            cout << "[STATE] Board ahora en Idle (listo para input).\n";
        }
    }
}

/* ===================== LÓGICA DE MATCHES ===================== */

bool Board::areAdjacent(int row1, int col1, int row2, int col2) const {
    if (row1 == row2 && col1 == col2) return false;
    return (abs(row1 - row2) + abs(col1 - col2) == 1);
}

bool Board::trySwapIndices(int row1, int col1, int row2, int col2) {
    bool rowsOut = (row1 < 0 || row1 >= ROWS || row2 < 0 || row2 >= ROWS);
    bool colsOut = (col1 < 0 || col1 >= COLS || col2 < 0 || col2 >= COLS);

    if (rowsOut || colsOut) return false;
    if (hasObstacleAt(row1, col1) || hasObstacleAt(row2, col2)) return false;
    if (!areAdjacent(row1, col1, row2, col2)) return false;

    firstRow = row1;
    firstCol = col1;
    secondRow = row2;
    secondCol = col2;

    firstGem = grid[firstRow][firstCol];
    secondGem = grid[secondRow][secondCol];
    if (!firstGem || !secondGem) return false;

    swapOrigPos1 = firstGem->getSprite().getPosition();
    swapOrigPos2 = secondGem->getSprite().getPosition();

    Vector2f pos1(col1 * TILE_SIZE + offset.x, row1 * TILE_SIZE + offset.y);
    Vector2f pos2(col2 * TILE_SIZE + offset.x, row2 * TILE_SIZE + offset.y);

    firstGem->setDestination(pos2);
    secondGem->setDestination(pos1);

    state = Swapping;
    playerInitiatedMove = true;
    inputLocked = true; // 🔒 bloquea input durante animación
    cout << "[LOCK] Input bloqueado (Swapping activo).\n";
    return true;
}

void Board::revertSwap() {
    if (!firstGem || !secondGem) {
        cerr << "[WARN] revertSwap abortado: puntero inválido.\n";
        cout << "[DEBUG] Limpieza de selección (revert abortado).\n";
        firstGem = nullptr;
        secondGem = nullptr;
        firstRow = firstCol = secondRow = secondCol = -1;
        state = Idle;
        inputLocked = false;
        return;
    }

    swap(grid[firstRow][firstCol], grid[secondRow][secondCol]);

    if (grid[firstRow][firstCol])
        grid[firstRow][firstCol]->setGridPositions(firstRow, firstCol);
    if (grid[secondRow][secondCol])
        grid[secondRow][secondCol]->setGridPositions(secondRow, secondCol);

    firstGem = grid[firstRow][firstCol];
    secondGem = grid[secondRow][secondCol];

    if (firstGem) firstGem->setDestination(swapOrigPos1);
    if (secondGem) secondGem->setDestination(swapOrigPos2);

    cout << "[INFO] Revirtiendo swap inválido entre (" << firstRow << "," << firstCol
        << ") y (" << secondRow << "," << secondCol << ").\n";

    playerInitiatedMove = false;
    state = Reverting;
}


bool Board::createsMatch(int row, int col, int kind) {
    if (col >= 2 && grid[row][col - 1] && grid[row][col - 2] &&
        grid[row][col - 1]->getKind() == kind && grid[row][col - 2]->getKind() == kind) {
        return true;
    }

    if (row >= 2 && grid[row - 1][col] && grid[row - 2][col] &&
        grid[row - 1][col]->getKind() == kind && grid[row - 2][col]->getKind() == kind) {
        return true;
    }

    return false;
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

            if (cur == prev) {
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

    // Detectar si hay gemas especiales
    bool containsSpecial = false;
    for (int k = 0; k < count; k++) {
        int r = horizontal ? outer : lastIndex - k;
        int c = horizontal ? lastIndex - k : outer;

        if (grid[r][c] && grid[r][c]->getType() != "Normal") {
            containsSpecial = true;
            break;
        }
    }

    // Calcular posición de gema especial
    if (count >= 4 && !containsSpecial) {
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

        if (grid[destRow][destCol] && grid[destRow][destCol]->getType() == "Normal") {
            spawnSpecialGem(destRow, destCol, horizontal);
        }
    }

    // Marcar gemas para eliminar
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
            if (matches[r][c] && grid[r][c]) {
                grid[r][c]->startDisappearing();
            }
        }
    }
}

int Board::clearMatches() {
    int score = 0;
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (matches[r][c] && grid[r][c]) {
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

/* ===================== OBJETIVOS Y OBSTÁCULOS ===================== */

void Board::updateObjectivesOnMatch(int row, int col) {
    if (!currentLevel) {
        return;
    }

    Objective* objective = currentLevel->getObjective();
    if (!objective) {
        return;
    }

    Gem* gem = grid[row][col];
    if (!gem) {
        return;
    }

    int gemKind = gem->getKind();
    if (objective->getType() == ObjectiveType::CollectGems) {
        if (objective->getGemKind() == gemKind) {
            objective->addProgress(1);
        }
    }
}

void Board::updateScoreObjective(int scoreGained) {
    if (!currentLevel) {
        return;
    }

    Objective* objective = currentLevel->getObjective();
    if (objective && objective->getType() == ObjectiveType::ReachScore) {
        objective->addProgress(scoreGained);
    }
}

void Board::damageAdjacentObstacles(int row, int col) {
    static const int dr[] = { -1, 1, 0, 0 };
    static const int dc[] = { 0, 0, -1, 1 };

    for (int i = 0; i < 4; i++) {
        int nr = row + dr[i];
        int nc = col + dc[i];

        if (nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS) {
            continue;
        }

        for (Obstacle* obs : obstacles) {
            if (obs && !obs->isDestroyedState() && obs->getRow() == nr && obs->getCol() == nc) {
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

void Board::activateSpecialGemsInMatches() {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (!(matches[r][c] && grid[r][c])) {
                continue;
            }

            if (grid[r][c]->getType() == "Bomb") {
                activateBombEffect(r, c);
            }
            else if (grid[r][c]->getType() == "Ice") {
                activateIceEffect(r);
            }
        }
    }
}

void Board::activateBombEffect(int row, int col) {
    const int RADIUS = 1;
    for (int r = row - RADIUS; r <= row + RADIUS; ++r) {
        for (int c = col - RADIUS; c <= col + RADIUS; ++c) {
            if (r >= 0 && r < ROWS && c >= 0 && c < COLS && grid[r][c]) {
                grid[r][c]->startDisappearing();
                matches[r][c] = true;
            }
        }
    }
}

void Board::activateIceEffect(int row) {
    for (int c = 0; c < COLS; ++c) {
        if (grid[row][c]) {
            grid[row][c]->startDisappearing();
            matches[row][c] = true;
        }
    }
}

/* ===================== FÍSICA Y REFILL ===================== */

void Board::applyGravity() {
    for (int c = 0; c < COLS; ++c) {
        for (int r = ROWS - 1; r >= 0; --r) {
            Gem* current = grid[r][c];

            if (current && current->getKind() != -1) {
                continue;
            }

            int above = r - 1;
            while (above >= 0 && (!grid[above][c] || grid[above][c]->getKind() == -1)) {
                above--;
            }

            if (above >= 0 && grid[above][c]) {
                swap(grid[r][c], grid[above][c]);
                Gem* g = grid[r][c];
                g->resetTransientState();
                g->setGridPositions(r, c);
                g->setDestination(Vector2f(c * TILE_SIZE + offset.x, r * TILE_SIZE + offset.y));

                if (grid[above][c]) {
                    grid[above][c]->setKind(-1);
                    grid[above][c]->getSprite().setColor(Color(255, 255, 255, 0));
                }
            }
            else {
                if (grid[r][c] && grid[r][c]->getKind() == -1) {
                    delete grid[r][c];
                    grid[r][c] = nullptr;
                }
            }
        }
    }
}

void Board::refill() {
    try {
        const Texture& texture = ResourceManager::instance().getTexture("assets/spritesheet.png");

        for (int c = 0; c < COLS; c++) {
            for (int r = ROWS - 1; r >= 0; r--) {
                if (grid[r][c] != nullptr) {
                    continue;
                }

                if (hasObstacleAt(r, c)) {
                    continue;
                }

                spawnGem(r, c);
            }
        }
    }
    catch (const exception& e) {
        cerr << "[ERROR] Board::refill() - " << e.what() << endl;
    }
}

void Board::spawnGem(int r, int c) {
    try {
        const Texture& texture = ResourceManager::instance().getTexture("assets/spritesheet.png");

        if (grid[r][c]) {
            delete grid[r][c];
            grid[r][c] = nullptr;
        }

        int kind = rand() % 5;
        grid[r][c] = new NormalGem(kind, r, c);
        grid[r][c]->setSprite(texture);
        grid[r][c]->resetTransientState();
        grid[r][c]->setGridPositions(r, c);
        grid[r][c]->getSprite().setPosition(Vector2f(c * TILE_SIZE + offset.x, -TILE_SIZE + offset.y));
        grid[r][c]->setDestination(Vector2f(c * TILE_SIZE + offset.x, r * TILE_SIZE + offset.y));
    }
    catch (const exception& e) {
        cerr << "[ERROR] Board::spawnGem() - " << e.what() << endl;
    }
}

void Board::spawnSpecialGem(int row, int col, bool horizontal) {
    try {
        const Texture& texture = ResourceManager::instance().getTexture("assets/spritesheet.png");

        if (row < 0 || row >= ROWS || col < 0 || col >= COLS) {
            return;
        }

        Gem* target = grid[row][col];
        if (!target || target->getType() != "Normal") {
            return;
        }

        int kind = target->getKind();
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
    catch (const exception& e) {
        cerr << "[ERROR] Board::spawnSpecialGem() - " << e.what() << endl;
    }
}

/* ===================== OBSTÁCULOS ===================== */

void Board::placeObstacles(int count) {
    clearObstacles();

    int placed = 0;
    int maxAttempts = ROWS * COLS * 10;

    while (placed < count && maxAttempts-- > 0) {
        int r = rand() % ROWS;
        int c = rand() % COLS;

        if (hasObstacleAt(r, c))
            continue;

        if (!grid[r][c] || grid[r][c]->getKind() == -1)
            continue;

        obstacles.push_back(new IronBlock(r, c));
        placed++;
    }

    if (placed < count) {
        cout << "[WARN] Solo se pudieron colocar " << placed
            << " de " << count << " obstáculos.\n";
    }
    else {
        cout << "[DEBUG] Se colocaron " << placed << " obstáculos de hierro.\n";
    }
}



void Board::clearObstacles() {
    for (Obstacle* obs : obstacles) {
        delete obs;
    }
    obstacles.clear();
}

bool Board::hasObstacleAt(int row, int col) const {
    for (Obstacle* obs : obstacles) {
        if (obs && !obs->isDestroyedState() && obs->getRow() == row && obs->getCol() == col) {
            return true;
        }
    }
    return false;
}

/* ===================== NIVEL ===================== */

void Board::setCurrentLevel(Level* level) {
    currentLevel = level;
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

/* ===================== UTILIDADES ===================== */

int Board::getState() const {
    return static_cast<int>(state);
}

Gem* Board::getGem(int row, int col) {
    return grid[row][col];
}

/* ===================== INPUT ===================== */

void Board::handleGemClick(int row, int col) {
    if (inputLocked || state != Idle) {
        cout << "[WARN] Click ignorado: tablero ocupado o animando.\n";
        return;
    }
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS) {
        cout << "[WARN] Click fuera del rango del tablero.\n";
        return;
    }
    if (!grid[row][col]) {
        cout << "[WARN] Click en celda vacía (" << row << "," << col << ").\n";
        return;
    }
    if (grid[row][col]->getKind() == -1) {
        cout << "[WARN] Click en gema inactiva (" << row << "," << col << ").\n";
        return;
    }

    if (!firstGem) {
        firstRow = row;
        firstCol = col;
        firstGem = grid[row][col];
        cout << "[INPUT] Primera gema seleccionada en (" << row << ", " << col
            << ") tipo=" << firstGem->getKind() << "\n";
        return;
    }

    if (firstRow == row && firstCol == col) {
        cout << "[INFO] Misma gema clickeada dos veces, deseleccionando.\n";
        cout << "[DEBUG] Limpieza de selección (click repetido).\n";
        firstGem = nullptr;
        firstRow = firstCol = -1;
        return;
    }

    cout << "[INPUT] Segunda gema seleccionada en (" << row << ", " << col
        << ") tipo=" << grid[row][col]->getKind() << "\n";

    bool accepted = trySwapIndices(firstRow, firstCol, row, col);

    if (accepted) {
        cout << "[SWAP] Intercambio aceptado entre (" << firstRow << "," << firstCol
            << ") y (" << row << "," << col << ")\n";
    }
    else {
        cout << "[ERROR] Swap cancelado entre (" << firstRow << "," << firstCol
            << ") y (" << row << "," << col << ")\n";
        cout << "[DEBUG] Limpieza de selección (swap no válido).\n";
        firstGem = nullptr;
        firstRow = firstCol = -1;
    }
}
