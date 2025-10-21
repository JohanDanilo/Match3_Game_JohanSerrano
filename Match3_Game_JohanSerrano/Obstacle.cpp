#include "Obstacle.h"

const float TILE_SIZE_OBS = 58;
const int GEM_WIDTH_OBS = 47;
const int GEM_HEIGHT_OBS = 54;
const Vector2f offset_obs(171, 70);

Obstacle::Obstacle(int aRow, int aCol, int health){
    row = aRow;
    col = aCol;
    maxHealth = health;

}

Obstacle::~Obstacle() {}

void Obstacle::takeDamage() {
    if (isDestroyed) return;

    health--;
    if (health <= 0) {
        health = 0;
        isDestroyed = true;
    }
    updateVisual();
}

bool Obstacle::isDestroyedState() const {
    return isDestroyed;
}

int Obstacle::getRow() const {
    return row;
}

int Obstacle::getCol() const {
    return col;
}

int Obstacle::getHealth() const {
    return health;
}

void Obstacle::setSprite() {
    Texture texture;
    if (!texture.loadFromFile("assets/obstacle.png")) {
        cerr << "Error loading background.png\n";
    }
    else sprite.setTexture(texture);
}

Sprite& Obstacle::getSprite() {
    return sprite;
}

IronBlock::IronBlock(int row, int col) : Obstacle(row, col, 2) {
    visualBlock.setSize(Vector2f(GEM_WIDTH_OBS, GEM_HEIGHT_OBS));
    visualBlock.setFillColor(Color(255, 255, 255, 200));
    visualBlock.setOutlineColor(Color(100, 150, 255));
    visualBlock.setOutlineThickness(2);

    float x = col * TILE_SIZE_OBS + offset_obs.x;
    float y = row * TILE_SIZE_OBS + offset_obs.y;
    visualBlock.setPosition(x, y);
}

void IronBlock::draw(RenderWindow& window) {
    if (!isDestroyed) {
        window.draw(visualBlock);
    }
}

void IronBlock::updateVisual() {
    if (health == 2) {
        visualBlock.setFillColor(Color(255, 255, 255, 200));
    }
    else if (health == 1) {
        visualBlock.setFillColor(Color(255, 255, 255, 100));
    }
}

void IronBlock::setPosition(float x, float y) {
    visualBlock.setPosition(x, y);
}