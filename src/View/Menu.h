#pragma once
#include "IOComponent.h"
#include "../config.h"

class Separator;
class Menu : public OComponent{
private:
	short x1, x2;
	short y1, y2;
	short x_min;
	short x_max;
	short y_min;
	short y_max;

public:
	unsigned int size;
	const char** elements;
	const char** links;
	unsigned short width;
	unsigned short height;
	
	void _AddLinks(utility::ElementsList<const char*> list);
	void _AddElements(utility::ElementsList<const char*> list);
	void _ModifyBorder(Separator* separators, unsigned int separatorNum);
	void _ModifyBorder(Separator separator);
	void _SetPosition(const char* position);
	void _SetPosition(short x, short y);
	void _ShowMenu();

	Menu();
	Menu(unsigned short width);
	~Menu();
};