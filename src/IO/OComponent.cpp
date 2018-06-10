#include "OComponent.h"
#include "../Modules/ModuleManagement.h"

//	General OComponent base constructor
OComponent::OComponent(const unsigned short max_x, const unsigned short min_x, const unsigned short max_y, const unsigned short min_y) :
	max_x(max_x), min_x(min_x), max_y(max_y), min_y(min_y) {
	_SetInitialCoordinates();
}

//	OComponent copy constructor
OComponent::OComponent(OComponent& parentFrame) :
	max_x(parentFrame.max_x), min_x(parentFrame.min_x), max_y(parentFrame.max_y), min_y(parentFrame.min_y ) {
	_SetInitialCoordinates();
}

OComponent::OComponent(OComponent* null) : max_x(0), min_x(0), max_y(0), min_y(0) {
	_SetInitialCoordinates();
}

void OComponent::_SetInitialCoordinates() {
	x1 = min_x;
	x2 = max_x;
	y1 = min_y;
	y2 = max_y;
}

// Frame Container copy constructor
Frame::Container::Container(const Container& copy) {
	frames = new Frame*[copy.frameNum];
	for (int i = 0; i < copy.frameNum; i++)
		frames[i] = copy.frames[i];
	frameNum = copy.frameNum;
}

// Frame Container destructor
Frame::Container::~Container() {
	delete[]frames;
}

//	Main frame constructor
Frame::Frame(const unsigned short max_x, const unsigned short min_x, const unsigned short max_y, const unsigned short min_y) :
	OComponent(max_x, min_x, max_y, min_y), parentFrame(nullptr){}

//	Frame copy constructor
Frame::Frame(Frame& copy) :
	OComponent(copy), parentFrame(&copy) , container(copy.container), IDname(copy.IDname){}

//	Frame constructor Used within split method setting new values using splitters
Frame::Frame(Frame& parentFrame, const unsigned short max_x, const unsigned short min_x, const unsigned short max_y, const unsigned short min_y) :
	OComponent(max_x, min_x, max_y, min_y), parentFrame(&parentFrame) {}

//	Frame destructor
Frame::~Frame() {
	delete[]elements;
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

	//	If sperator is x direction
	if (separator.direction == 0) {		
		Frame* newFrame = new Frame(*this, max_x, min_x, coord.y1, min_y);
		newFrame->IDname = firstID;
		_UpdateContainer(newFrame);		
		newFrame = new Frame(*this, max_x, min_x, max_y , coord.y1+1);
		newFrame->IDname = secondID;
		_UpdateContainer(newFrame);
	}
	//	If separator is y direction
	else if (separator.direction == 1) {
		Frame* newFrame = new Frame(*this, coord.x1, min_x, max_y, min_y);
		newFrame->IDname = firstID;
		_UpdateContainer(newFrame);
		newFrame = new Frame(*this, max_x, coord.x1 + 1, max_y, min_y);
		newFrame->IDname = secondID;
		_UpdateContainer(newFrame);
	}
}

//	Method used to create new frames within a frame using width percentage and direction (horizontal/vertical)
void Frame::_Split(unsigned short percent, const char* direction,  const char* firstID, const char* secondID) {
	int dir = utility::_ReadDirection(direction);

	//	If sperator is x direction
	if (dir == 0) {
		unsigned short splitPos = max_y * percent / 100;
		Frame* newFrame = new Frame(*this, max_x, min_x, splitPos, min_y);
		newFrame->IDname = firstID;
		_UpdateContainer(newFrame);
		newFrame = new Frame(*this, max_x, min_x, max_y, splitPos + 1);
		newFrame->IDname = secondID;
		_UpdateContainer(newFrame);
	}
	//	If separator is y direction
	else if (dir == 1) {
		unsigned short splitPos = max_x * percent / 100;
		Frame* newFrame = new Frame(*this, splitPos, min_x, max_y, min_y);
		newFrame->IDname = firstID;
		_UpdateContainer(newFrame);
		newFrame = new Frame(*this, max_x, splitPos + 1, max_y, min_y);
		newFrame->IDname = secondID;
		_UpdateContainer(newFrame);
	}
}

//	Select frame from subframes container 
Frame* Frame::_Select(const char* IDname) {
	for (int i = 0; i < container.frameNum; i++) 
		if (utility::_CompareChar(container.frames[i]->IDname, IDname))
			return container.frames[i];
	return nullptr;	
}

//	Add new FrameElement item 
void Frame::_AddElement(FrameElement& newElement) {
	FrameElement** element = new FrameElement*(nullptr);	
	utility::_AddElement(elements, *element, elNum);
	elements[elNum - 1] = &newElement;
	elements[elNum - 1]->_SetParentFrame(this);
	delete element;
}

//	Show all frame elements
void Frame::_ShowElements() {
	for (int i = 0; i < elNum; i++) {
		elements[i]->_SetYpos(nextYpos++);
		elements[i]->_Show();
	}
}

//	Add frame padding
void Frame::_AddPadding(unsigned short padding) {
	x1 += padding;
	x2 -= padding;
	y1 += padding;
	y2 -= padding;
}

//	Create subframe using new frame coordinates
void Frame::_CreateSubFrame(const char* IDname, Coordinates& coord) {
	if (coord.x2 > max_x)
		coord.x2 = max_x;
	if (coord.x1 < min_x)
		coord.x1 = min_x;
	if (coord.y2 > max_y)
		coord.y2 = max_y;
	if (coord.y1 < min_y)
		coord.y1 = min_y;

	Frame* newFrame = new Frame(*this, coord.x2, coord.x1, coord.y2, coord.y1);
	newFrame->IDname = IDname;
	_UpdateContainer(newFrame);
}

//	Change frame coordinates
void Frame::_ChangeCoordinates(Coordinates& coord) {
	x1 = coord.x1;
	x2 = coord.x2;
	y1 = coord.y1;
	y2 = coord.y2;
}

//	FrameElement default constructor
FrameElement::FrameElement(Frame* parentFrame) : OComponent(*parentFrame), parentFrame(parentFrame) {}

//	FrameElement "out of frame" null constructor
FrameElement::FrameElement() : OComponent(nullptr), parentFrame(nullptr) {}

//	FrameElement copy constructor
FrameElement::FrameElement(const FrameElement& copy, Frame* parentFrame) : OComponent(*parentFrame), parentFrame(parentFrame) {}

//	Find corresponding display for this frame element
Display* FrameElement::_GetDisplay() {
	Frame* select = parentFrame;
	do {
		if (select->dsp != nullptr)
			return select->dsp;
		select = select->parentFrame;
	} while (select!=nullptr);
	return nullptr;
}

//	Return label display position using align property
Cursor Label::_Align() {
	Cursor pos;
	Frame::Coordinates coord = parentFrame->_GetCoordinates();
	switch (utility::_ReadAlign(align)) {
	case 1:
		//	Left align
		pos._SetXY(coord.x1+padding, coord.y1+Ypos);
		break;
	case 2:
		//	 Right align
		pos._SetXY(coord.x2 - length-padding, coord.y1+Ypos);
		break;
	case 3:
		//	Center align
		short frameSize = coord.x2 - coord.x1;
		short margin = (frameSize- length)/2;
		pos._SetXY(coord.x1+margin, coord.y1+Ypos);
	}
	return pos;
}

//	Return menu item display position using align property
Cursor MenuItem::_Align() {
	Cursor pos;
	Frame::Coordinates coord = parentFrame->_GetCoordinates();
	switch (utility::_ReadAlign(align)) {
	case 1:
		//	Left align
		pos._SetXY(coord.x1 + padding, coord.y1 + Ypos);
		break;
	case 2:
		//	 Right align
		pos._SetXY(coord.x2 - (length + utility::_CharLength(orderNum)) - padding, coord.y1 + Ypos);
		break;
	case 3:
		//	Center align
		short frameSize = coord.x2 - coord.x1;
		short margin = (frameSize - (length + utility::_CharLength(orderNum))) / 2;
		pos._SetXY(coord.x1 + margin, coord.y1 + Ypos);
	}
	return pos;
}

//	Display label
void Label::_Show() {
	short max_x = parentFrame->_GetCoordinates().x2;
	Cursor pos = _Align();
	Display* dsp = _GetDisplay();	
	unsigned int cut = ((pos._GetX() + length) > max_x) ? ((pos._GetX() + length)-max_x) : 0;

	if (cut == 0) {
		if (symbol == 0) 
			dsp->_Display(pos, text);
		else 
			dsp->_Display(pos, symbol, text);
	}
	else 
		dsp->_Display(pos, text);
}

MenuItem::MenuItem(const char* text, Module* previousModule) : Label(text) {
	link = *previousModule;
}

//	Display menu item
void MenuItem::_Show() {
	short max_x = parentFrame->_GetCoordinates().x2;
	Cursor pos = _Align();
	Display* dsp = _GetDisplay();
	unsigned int cut = ((pos._GetX() + length) > max_x) ? ((pos._GetX() + length) - max_x) : 0;

	if (cut == 0) {
		if (symbol == 0)
			dsp->_Display(pos, orderNum, text);
		else
			dsp->_Display(pos, symbol, orderNum, text);
	}
	else
		dsp->_Display(pos, text);
}

//	draw line - 0 = x spawn direction, 1 = y spawn direction
Separator::Separator(Layout& layout, short length, bool direction, unsigned short start_X, unsigned short start_Y) :
	FrameElement(layout._Select()), length(length), direction(direction){
	_SetValue(*layout._Select(), start_X, start_Y);
}

Separator::Separator(Frame& parentFrame, short length, bool direction, unsigned short start_X, unsigned short start_Y) : 
	FrameElement(&parentFrame) , length(length), direction(direction)  {
	_SetValue(parentFrame, start_X, start_Y);
}

void Separator::_SetValue(Frame& parentFrame, unsigned short start_X, unsigned short start_Y) {
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

void Layout::_DefaultFrameTemplate(Display& dsp) {
	Frame::Coordinates coord = parentFrame->_GetCoordinates();
	frame = new Frame(coord.x2 - 1, coord.x1 + 1, coord.y2 - 1, coord.y1 + 1);
	frame->_SetIDname("DefaultLayout");
	frame->_SetDisplay(dsp);
	
	Separator menuLine(*this, 26, 1, 22, 0);
	Separator headerLine(*this, ::width - 4, 0, 2, 2);

	frame->_Split(headerLine, "Header", "Body");
	frame->_Select("Header")->_Split(menuLine, "MenuHeader", "BodyHeader");
	frame->_Select("BodyHeader")->_Split(90,"vertical", "SelectionTitle", "Date");
	frame->_Select("Body")->_Split(menuLine, "Menu", "Content");

	dsp._Display(menuLine);
	dsp._Display(headerLine);
}

Frame* Layout::_Select() {
	return this->frame;
}

Frame* Layout::_Select(const char* IDname) {
	return this->frame->_Select(IDname);
}

void Layout::_Split(Separator& separator, const char* firstID, const char* secondID) {
	this->frame->_Split(separator, firstID, secondID);
}

void Layout::_ShowElements() {
	Frame::Container sub = frame->_GetSubFrames();
	for (int i = 0; i < sub.frameNum; i++)
		sub.frames[i]->_ShowElements();
}

Layout::~Layout() {
	Frame::Container sub = frame->_GetSubFrames();
	for (int i = 0; i < sub.frameNum; i++) 
		delete sub.frames[i];
	delete frame;
}

Menu::~Menu() {
	delete[]items;
}

// Add new menu element
void Menu::_AddItem(MenuItem& item) {
	MenuItem** pp = new MenuItem*(nullptr);
	utility::_AddElement(items,*pp, size);
	items[size - 1] = &item;
	delete pp;
}

// Change existing menu item with new item
void Menu::_ChangeItem(MenuItem& item, int pos) {
	items[pos] = &item;
}

//	display menu items
void Menu::_Show() {
	char num[] = "[ ] ";
	for (int i = 0; i < size; i++) {
		num[1] = i + 1 + '0';
		items[i]->_SetParentFrame(parentFrame);
		items[i]->_SetYpos(i);
		items[i]->_SetOrderNumber(num);
		items[i]->_SetPadding(padding);
		items[i]->_Show();
	}
}