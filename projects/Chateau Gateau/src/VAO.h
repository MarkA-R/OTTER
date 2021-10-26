#pragma once
#include "globalHeaders.h"
class VAO
{

public:
	VAO();
	void loadData(std::string);
	void Draw();
protected:
	int trisToDraw;
};

