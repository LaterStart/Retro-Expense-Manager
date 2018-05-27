#include "OComponent.h"
#include "../config.h"

//	General OComponent base constructor
OComponent::OComponent(const unsigned short max_x, const unsigned short min_x, const unsigned short max_y, const unsigned short min_y) :
	max_x(max_x), min_x(min_x), max_y(max_y), min_y(min_y) {}

//	OComponent copy constructor
OComponent::OComponent(OComponent& parentFrame) :
	max_x(parentFrame.max_x), min_x(parentFrame.min_x), max_y(parentFrame.max_y), min_y(parentFrame.min_y ) {}

OComponent::~OComponent() {}

Frame::Container::Container(Container& copy) {
	firstFrame = copy.firstFrame;
	copy.firstFrame = nullptr;   //FIX IT
	secondFrame = copy.secondFrame;
	copy.secondFrame = nullptr;
}

Frame::Container::Container() : firstFrame(nullptr), secondFrame(nullptr) {}

Frame::Container::~Container() {
	delete firstFrame;
	delete secondFrame;
}

Frame Frame::Container::_FirstFrame() {
	Frame tmp = *firstFrame;
	return tmp;
}

Frame Frame::Container::_SecondFrame() {
	return *secondFrame;
}

FrameElement::FrameElement(Frame& parentFrame) : OComponent(parentFrame), parentFrame(&parentFrame) {}

//	Main frame constructor
Frame::Frame(const unsigned short max_x, const unsigned short min_x, const unsigned short max_y, const unsigned short min_y) :
	OComponent(max_x, min_x, max_y, min_y), parentFrame(nullptr){}

//	Frame copy constructor
Frame::Frame(Frame& parentFrame) :
	OComponent(parentFrame), parentFrame(&parentFrame) {}

//	Frame constructor Used within split method setting new values using splitters
Frame::Frame(Frame& parentFrame, const unsigned short max_x, const unsigned short min_x, const unsigned short max_y, const unsigned short min_y) :
	OComponent(max_x, min_x, max_y, min_y), parentFrame(&parentFrame){}

//	Create new Frame within parent frame
Frame Frame::_CreateChildFrame() {
	Frame childFrame(*this, this->max_x - 1, this->min_x + 1, this->max_y - 1, this->min_y + 1);
	return childFrame;
}

Frame::Container Frame::_Split(Separator& separator) {
	Coordinates coord = separator._GetCoordinates();
	Container newFrames;
	if (separator.direction == 0) {
		newFrames.firstFrame = new Frame(*this, max_x, min_x, coord.y1, min_y);
		newFrames.secondFrame = new Frame(*this, max_x, min_x, max_y , coord.y1+1);
	}
	else if (separator.direction == 1) {
		newFrames.firstFrame = new Frame(*this, coord.x1, min_x, max_y, min_y);
		newFrames.secondFrame = new Frame(*this, max_x, coord.x1+1, max_y, min_y);
	}
	return newFrames;
}

// draw line - 0 = x spawn direction, 1 = y spawn direction
Separator::Separator(Frame& parentFrame, short length, bool direction, unsigned short start_X, unsigned short start_Y) : 
	FrameElement(parentFrame) , length(length), direction(direction)  {
	x1 = start_X;
	y1 = start_Y;
	x2 = x1;
	y2 = y1;
	if (this->direction == 0) {
		x2 = x1 + length;
	}
	else if (this->direction == 1) {
		y2 = x1 + length;
	}
	this->parentFrame = &parentFrame;
}

Separator::Separator(const Separator& copy) : FrameElement(*copy.parentFrame) ,length(copy.length), direction (copy.direction) {
	x1 = copy.x1;
	x2 = copy.x2;
	y1 = copy.y1;
	y2 = copy.y2;
	this->parentFrame = copy.parentFrame;
}

Menu::Menu(Frame* parentFrame, unsigned short width) : Frame(*parentFrame), width(width), elements(nullptr), size(0), x_max(::width - 2), y_max(::height - 1), x_min(2), y_min(1) {
	if (this->width > x_max)
		this->width = x_max;

	x1 = x_min;
	y1 = y_min;
	x2 = x1;
	y2 = y1;
}

Menu::Menu(Frame* parentFrame) : Frame(*parentFrame) ,width(0), elements(nullptr), size(0), x_max(::width - 2), y_max(::height - 1), x_min(2), y_min(1) {
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
		num[1] = (z + 1) + '0';
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
	if (utility::_CompareChar("left", position)) {
		x1 = 2; x2 = 2;
		y1 = 1; y2 = 1;
	}
	else if (utility::_CompareChar("right", position)) {
		x1 = (width > 0) ? ::width - width : ::width - 20;
		x2 = ::width;
		y1 = 3;
		y2 = y1;
	}
}

//	sets position of menu using coordinates 
void Menu::_SetPosition(short x, short y) {}


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
	for (unsigned int i = 0; i < separatorNum; i++)
		_ModifyBorder(separators[i]);
}

void Menu::_ModifyBorder(Separator separator) {
	Coordinates menuBorder = separator._GetCoordinates();
	short sep_x1 = menuBorder.x1;
	short sep_y1 = menuBorder.y1;
	short sep_x2 = menuBorder.x2;
	short sep_y2 = menuBorder.y2;

	if (separator.direction == 0) {
		sep_x2 = sep_x1 + separator.length;

		y_min = (sep_y1 > y_min) ? sep_y1 + 1 : y_min;
		y1 = (y_min > y1) ? y_min : y1;
		y2 = y1;

	}
	else if (separator.direction == 1) {
		sep_y2 = sep_y1 + separator.length;

		x_max = (sep_x1 < x_max) ? sep_x1 - 1 : x_max;
	}
}