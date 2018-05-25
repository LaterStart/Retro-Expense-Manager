#include "Menu.h"

Menu::Menu() : elements(nullptr), size(0) {}

Menu::~Menu() {
	delete[]elements;
}

void Menu::_CreateMenu(utility::ElementsList<const char*> list) {
	size = list._GetSize();
	elements = new const char*[size];
	unsigned int z = 0;
	while (z < size) {
		elements[z++] = list.node;
		list._NextNode();		
	}	
}