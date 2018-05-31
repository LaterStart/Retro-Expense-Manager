#include "OComponent.h"

//	General OComponent base constructor
OComponent::OComponent(const unsigned short max_x, const unsigned short min_x, const unsigned short max_y, const unsigned short min_y) :
	max_x(max_x), min_x(min_x), max_y(max_y), min_y(min_y) {}

//	OComponent copy constructor
OComponent::OComponent(OComponent& parentFrame) :
	max_x(parentFrame.max_x), min_x(parentFrame.min_x), max_y(parentFrame.max_y), min_y(parentFrame.min_y ) {}

OComponent::OComponent(OComponent* null) : max_x(0), min_x(0), max_y(0), min_y(0) {}

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
		short margin = (frameSize-length)/2;
		pos._SetXY(coord.x1+margin, coord.y1+Ypos);
	}
	return pos;
}

//	Display label
void Label::_Show() {
	Cursor pos = _Align();
	Display* dsp = _GetDisplay();
	if(symbol==0)
		dsp->_Display(*this, pos);
	else dsp->_Display(*this, symbol, pos);
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
	
	Separator menuLine(*this, 26, 1, 20, 0);
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