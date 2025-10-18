#pragma once
#include "Gem.h"
class NormalGem : public Gem
{
public:
	NormalGem(int aKind, int aRow, int aCol);

	string getType() const override;

	void draw(RenderWindow& window) override;
	
};

