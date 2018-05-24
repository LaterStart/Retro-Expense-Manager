#pragma once

#include "../utility.h"
class Menu {
public:
	unsigned int size;
	const char** elements;
public:
	Menu();
	~Menu();
	
	void _CreateMenu(utility::ElementsList<const char*> list);
};