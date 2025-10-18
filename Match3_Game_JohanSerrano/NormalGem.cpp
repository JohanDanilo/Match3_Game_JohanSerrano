#include "NormalGem.h"

NormalGem::NormalGem(int aKind, int aRow, int aCol)
{
    kind = aKind;
    row = aRow;
    column = aCol;
    x = (column * TILE_SIZE) + offset.x;
    y = (row * TILE_SIZE) + offset.y;
    alpha = 255.f;
    sprite.setPosition(x, y);
}

string NormalGem::getType() const
{
	return string("Normal");
}

void NormalGem::draw(RenderWindow& window)
{
    sprite.setColor(Color(255, 255, 255, alpha));
    window.draw(sprite);
}