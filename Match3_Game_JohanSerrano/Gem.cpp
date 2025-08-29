#include "Gem.h"

Gem::Gem()
{
	boardPosition = offset;
	x = y = 0.0;
	colum = row = kind = 0;
	alpha = 255;
}

Gem::Gem(int aKind, int aRow, int aCol)
{
	kind = aKind;
	row = aRow;
	colum = aCol;
	x = colum * TILE_SIZE;
	y = row * TILE_SIZE;
	alpha = 255;

}

void Gem::draw(RenderWindow& window, const Texture& texture) const
{
	Sprite gem(texture);
	gem.setTextureRect(IntRect(kind * GEM_WIDTH, 0, GEM_WIDTH, GEM_HEIGHT));
	gem.setColor(Color(255, 255, 255, alpha));
	gem.setPosition(x, y);
	gem.move(offset.x - TILE_SIZE, offset.y - TILE_SIZE);
	window.draw(gem);
}

void Gem::setKind(int aKind)
{
	kind = aKind;
}

int Gem::getKind()
{
	return kind;
}


Vector2f Gem::getBoardPosition()
{
	return boardPosition;
}
