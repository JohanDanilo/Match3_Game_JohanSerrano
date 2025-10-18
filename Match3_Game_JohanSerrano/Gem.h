#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>

using namespace sf;
using namespace std;

const float TILE_SIZE = 58;
const int GEM_WIDTH = 47;
const int GEM_HEIGHT = 54;
const Vector2f offset(171, 70);

class Board; // evita includes circulares

class Gem {
protected:

    float x;
    float y;
    int column;
    int row;
    float alpha;
    int kind;
    Vector2f destination;
    bool isMoving = false;
    bool isDisappearing = false;
    Sprite sprite;

public:
    Gem();
    Gem(int aKind, int aRow, int aColumn);
    virtual ~Gem();

    virtual void setSprite(Texture& texture);
    Sprite& getSprite();

    virtual void draw(RenderWindow& window) = 0;

    //virtual void onMatch(Board& board, int row, int col);

    virtual string getType() const = 0;

    //virtual Gem* clone() const;

    void setKind(int aType);
    void setGridPositions(int aRow, int aColum);
    int getKind();
    int getRow();
    int getColum();
    float getX();
    float getY();

    void setDestination(const Vector2f& destination);
    bool moveGem(float deltaTime);

    bool dissapear(float deltaTime);
    bool getDisappearingState();
    void startDisappearing();
    bool isEmpty();
    void resetTransientState();

};
