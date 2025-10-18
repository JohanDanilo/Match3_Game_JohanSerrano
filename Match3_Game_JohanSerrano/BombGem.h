#pragma once
#include "Gem.h"
class BombGem : public Gem
{
	BombGem(int aKind, int aRow, int aCol);

	string getType() const override;

	void draw(RenderWindow& window) override;
};

