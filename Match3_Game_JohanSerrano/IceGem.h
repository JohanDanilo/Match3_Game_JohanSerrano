#pragma once
#include "Gem.h"
class IceGem : public Gem
{
	IceGem(int aKind, int aRow, int aCol);

	string getType() const override;

	void draw(RenderWindow& window) override;
};

