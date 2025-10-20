#include "BombGem.h"

BombGem::BombGem(int aKind, int aRow, int aCol)
{
    kind = aKind;
    row = aRow;
    column = aCol;
    x = (column * TILE_SIZE) + offset.x;
    y = (row * TILE_SIZE) + offset.y;
    alpha = 255.f;
    sprite.setPosition(x, y);
}

string BombGem::getType() const
{
	return string("Bomb");
}

void BombGem::draw(RenderWindow& window)
{
    sprite.setColor(Color(255, 100, 100, alpha));
    window.draw(sprite);
}
