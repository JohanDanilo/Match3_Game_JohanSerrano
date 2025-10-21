#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
using namespace sf;
using namespace std;

class Obstacle {
protected:
    int row, col;
    int health = 0;
    int maxHealth = 0;
    Sprite sprite;
    bool isDestroyed = false;

public:
    Obstacle(int aRow, int aCol, int health);
    virtual ~Obstacle();

    void takeDamage();
    virtual void draw(RenderWindow& window) = 0;
    virtual void updateVisual() = 0;

    bool isDestroyedState() const;
    int getRow() const;
    int getCol() const;
    int getHealth() const;

    void setSprite();
    Sprite& getSprite();
};

class IronBlock : public Obstacle {
private:
    RectangleShape visualBlock;

public:
    IronBlock(int row, int col);

    void draw(RenderWindow& window) override;
    void updateVisual() override;
    void setPosition(float x, float y);
};