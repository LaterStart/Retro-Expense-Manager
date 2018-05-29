#include "OComponent.h"
#include "../config.h"

//	General OComponent base constructor
OComponent::OComponent(const unsigned short max_x, const unsigned short min_x, const unsigned short max_y, const unsigned short min_y) :
	max_x(max_x), min_x(min_x), max_y(max_y), min_y(min_y) {}

//	OComponent copy constructor
OComponent::OComponent(OComponent& parentFrame) :
	max_x(parentFrame.max_x), min_x(parentFrame.min_x), max_y(parentFrame.max_y), min_y(parentFrame.min_y ) {}

OComponent::OComponent(OComponent* null) : max_x(0), min_x(0), max_y(0), min_y(0) {}

// Frame Container default constructor
Frame::Container::Container() : frames(nullptr), frameNum(0){}

// Frame Container copy constructor
Frame::Container::Container(const Container& copy) {
	frames = new Frame*[copy.frameNum];
	for (int i = 0; i < copy.frameNum; i++)
		frames[i] = _MoveFrame(*copy.frames[i]);
	frameNum = copy.frameNum;
}

//	Give ownership of frame to another container
Frame* Frame::Container::_MoveFrame(Frame& source) {
	Frame* move = new Frame(source);
	return move;
}

// Frame Container destructor
Frame::Container::~Container() {
	for (int i = 0; i < frameNum; i++)
		delete frames[i];
	delete[]frames;
}

//	Main frame constructor
Frame::Frame(const unsigned short max_x, const unsigned short min_x, const unsigned short max_y, const unsigned short min_y) :
	OComponent(max_x, min_x, max_y, min_y), parentFrame(nullptr){}

//	Frame copy constructor
Frame::Frame(Frame& copy) :
	OComponent(copy), parentFrame(&copy) , container(copy.container), IDname(copy.IDname), dsp(copy.dsp){}

//	Frame constructor Used within split method setting new values using splitters
Frame::Frame(Frame& parentFrame, const unsigned short max_x, const unsigned short min_x, const unsigned short max_y, const unsigned short min_y) :
	OComponent(max_x, min_x, max_y, min_y), parentFrame(&parentFrame), dsp(parentFrame.dsp){}

//	Create new Frame within parent frame
Frame Frame::_CreateSubFrame(const char* IDname, Display* dsp) {
	Frame* newFrame = new Frame(*this, this->max_x - 1, this->min_x + 1, this->max_y - 1, this->min_y + 1);
	newFrame->dsp = dsp;
	newFrame->IDname = IDname;
	_UpdateContainer(newFrame);
	return *container.frames[container.frameNum - 1];
}

//	Update frames container recursive for all parent frames
void Frame::_UpdateContainer(Frame* newFrame) {
	Frame** ptr = new Frame*(nullptr);
	utility::_AddElement(container.frames, *ptr, container.frameNum);
	container.frames[container.frameNum - 1] = newFrame;
	delete ptr;
	if (parentFrame != nullptr)
		parentFrame->_UpdateContainer(newFrame);
}

//	Method used to create new frames within a frame using spliters
void Frame::_Split(Separator& separator, const char* firstID, const char* secondID) {
	Coordinates coord = separator._GetCoordinates();

	if (separator.direction == 0) {		
		Frame* newFrame = new Frame(*this, max_x, min_x, coord.y1, min_y);
		newFrame->IDname = firstID;
		_UpdateContainer(newFrame);		
		newFrame = new Frame(*this, max_x, min_x, max_y , coord.y1+1);
		newFrame->IDname = secondID;
		_UpdateContainer(newFrame);
	}
	else if (separator.direction == 1) {
		Frame* newFrame = new Frame(*this, coord.x1, min_x, max_y, min_y);
		newFrame->IDname = firstID;
		_UpdateContainer(newFrame);
		newFrame = new Frame(*this, max_x, coord.x1 + 1, max_y, min_y);
		newFrame->IDname = secondID;
		_UpdateContainer(newFrame);
	}
}

// Select frame from subframes container 
Frame* Frame::_Select(const char* IDname) {
	for (int i = 0; i < container.frameNum; i++) 
		if (utility::_CompareChar(container.frames[i]->IDname, IDname))
			return container.frames[i];
	return nullptr;	
}

void Frame::_AddElement(FrameElement& newElement) {
	FrameElement** element = new FrameElement*(nullptr);	
	utility::_AddElement(elements, *element, elNum);
	elements[elNum - 1] = &newElement;
	elements[elNum - 1]->_SetParentFrame(this);
}

void Frame::_ShowElements() {
	for (int i = 0; i < elNum; i++) {
		elements[i]->_Show();


	}
}

// FrameElement default constructor
FrameElement::FrameElement(Frame* parentFrame) : OComponent(*parentFrame), parentFrame(parentFrame) {}

FrameElement::FrameElement() : OComponent(nullptr), parentFrame(nullptr) {}

// FrameElement copy constructor
FrameElement::FrameElement(const FrameElement& copy, Frame* parentFrame) : OComponent(*parentFrame), parentFrame(parentFrame) {}

void Label::_Show() {
	Frame::Coordinates coord = parentFrame->_GetCoordinates();
	Cursor pos(coord.x1, coord.y1);
	parentFrame->dsp->_Display(*this);
}

// draw line - 0 = x spawn direction, 1 = y spawn direction
Separator::Separator(Frame& parentFrame, short length, bool direction, unsigned short start_X, unsigned short start_Y) : 
	FrameElement(&parentFrame) , length(length), direction(direction)  {
	Coordinates frameCoord = parentFrame._GetCoordinates();

	if (start_X < frameCoord.x1)
		start_X = frameCoord.x1;
	else if (start_X > frameCoord.x2)
		start_X = frameCoord.x2;
	if (start_Y < frameCoord.y1)
		start_Y = frameCoord.y1;
	else if (start_Y > frameCoord.y2)
		start_Y = frameCoord.y2;

	x1 = start_X;
	y1 = start_Y;
	x2 = x1;
	y2 = y1;
	if (this->direction == 0) {
		x2 = x1 + this->length;
		if (x2 > frameCoord.x2) {
			this->length -= x2 - frameCoord.x2;
			x2 = frameCoord.x2;
		}
	}
	else if (this->direction == 1) {
		y2 = y1 + this->length;
		if (y2 > frameCoord.y2) {
			this->length -= y2 - frameCoord.y2;
			y2 = frameCoord.y2;
		}			
	}
	this->parentFrame = &parentFrame;
}

Separator::Separator(const Separator& copy) : FrameElement(copy.parentFrame) ,length(copy.length), direction (copy.direction) {
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
		menuDisp._Display(elements[i], cut);
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