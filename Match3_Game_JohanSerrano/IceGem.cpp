#include "IceGem.h"

IceGem::IceGem(int aKind, int aRow, int aCol)
{
    kind = aKind;
    row = aRow;
    column = aCol;
    x = (column * TILE_SIZE) + offset.x;
    y = (row * TILE_SIZE) + offset.y;
    alpha = 255.f;
    sprite.setPosition(x, y);
}

string IceGem::getType() const
{
    return string("Ice");
}

void IceGem::draw(RenderWindow& window)
{
    sprite.setColor(Color(30, 80, 255, alpha));

    window.draw(sprite);
}