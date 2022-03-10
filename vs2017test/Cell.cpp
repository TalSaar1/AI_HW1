#include "Cell.h"

Cell::Cell(int r, int c, Cell * p)
{
	row = r;
	col = c;
	parent = p;
}

Cell::~Cell()
{
	delete this->parent;
}