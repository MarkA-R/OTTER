#include "VBO.h"

VBO::VBO(int i) {
	pos = i * 4;
	colour = pos + 1;
	norm = pos + 2;
	uv = pos + 3;
	posi = i * 4;
	colouri = posi + 1;
	normi = posi + 2;
	uvi = posi + 3;
	//model = pos + 4;

}
