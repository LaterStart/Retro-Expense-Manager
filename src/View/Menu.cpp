#include "Menu.h"

Menu::Menu(unsigned short width) : width(width), elements(nullptr), size(0), x_max(::width-2), y_max(::height-1), x_min(2), y_min(1) {
	if (this->width > x_max)
		this->width = x_max;

	x1 = x_min;
	y1 = y_min;
	x2 = x1;
	y2 = y1;
}

Menu::Menu() : width(0), elements(nullptr), size(0) , x_max(::width-2), y_max(::height-1), x_min(2), y_min(1){
	x1 = x_min;
	y1 = y_min;
	x2 = x1;
	y2 = y1;
}

Menu::~Menu() {
	delete[]elements;
}

//	add menu elements
void Menu::_AddElements(utility::ElementsList<const char*> list) {
	size = list._GetSize();
	elements = new const char*[size];
	unsigned int z = 0; 
	char num[] = "[0] ";
	while (z < size) {
		num[1] = (z+1) + '0';
		elements[z++] = utility::_InsertChar((char*)list.node, num);
		list._NextNode();		
	}	
}

//	add menu links
void Menu::_AddLinks(utility::ElementsList<const char*> list) {
	unsigned int linksNum = list._GetSize();
	links = new const char*[linksNum];
	unsigned int z = 0;
	while (z < linksNum) {
		links[z++] = list.node;
		list._NextNode();
	}
}

//	sets desired position of menu - can be left, top, right or bottom
void Menu::_SetPosition(const char* position) {
	if (utility::_CompareChar("left", position) ){
		x1 = 0; x2 = 0;
		y1 = 0; y2 = 0;
	}
	else if (utility::_CompareChar("right", position)) {
		x1 = ::width - width; x2 = ::width;
		y1 = 3; y2 = 0;
	}
}

//	sets position of menu using coordinates 
void Menu::_SetPosition(short x, short y){}


//	display menu elements
void Menu::_ShowMenu() {
	Cursor pos(x1, y1);
	Display menuDisp; unsigned int cut = 0, length;
	for (unsigned int i = 0; i < size; i++) {
		length = utility::_CharLength(elements[i]); cut = 0;
		if (length > width && (width > 0 || length > x_max)) {
			int cutter = (width > 0) ? width : x_max;
			cut = length - (cutter - x1);
		}			
		menuDisp._DisplayContent(elements[i], cut);
		pos._MoveToXY(x1, ++y2);
	}
	height = y2 - y1;
}

//	adjust menu border and position with display frame separators
void Menu::_ModifyBorder(Separator* separators, unsigned int separatorNum) {
	for (int i = 0; i < separatorNum; i++)
		_ModifyBorder(separators[i]);
}

void Menu::_ModifyBorder(Separator separator) {
	short sep_x1 = separator.startX;
	short sep_y1 = separator.startY;
	short sep_x2 = 0;
	short sep_y2 = 0;

	if (separator.direction == 0) {
		sep_x2 = sep_x1 + separator.length;

		y_min = (sep_y1 > y_min) ? sep_y1+1 : y_min;
		y1 = (y_min > y1) ? y_min : y1;
		y2 = y1;

	}
	else if (separator.direction == 1) {
		sep_y2 = sep_y1 + separator.length;

		x_max = (sep_x1 < x_max) ? sep_x1-1 : x_max;
	}	
}