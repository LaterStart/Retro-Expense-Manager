#include "OComponent.h"
#include "../Modules/ModuleManagement.h"

//	General OComponent base constructor
OComponent::OComponent(const unsigned short max_x, const unsigned short min_x, const unsigned short max_y, const unsigned short min_y) :
	max_x(max_x), min_x(min_x), max_y(max_y), min_y(min_y) {
	_SetInitialCoordinates();
}

//	OComponent overloaded copy constructor
OComponent::OComponent(const OComponent& copy) :
	max_x(copy.max_x), min_x(copy.min_x), max_y(copy.max_y), min_y(copy.min_y ) {
	x1 = copy.x1;
	x2 = copy.x2;
	y1 = copy.y1;
	y2 = copy.y2;
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
	this->componentType = copy.componentType;
}

// Frame Container destructor
Frame::Container::~Container() {
	delete[]frames;
}

//	Main frame constructor
Frame::Frame(const unsigned short max_x, const unsigned short min_x, const unsigned short max_y, const unsigned short min_y) :
	OComponent(max_x, min_x, max_y, min_y), parentFrame(nullptr){
	this->componentType = ComponentType::frame;
}

//	Frame copy constructor
Frame::Frame(Frame& copy) :
	OComponent(copy), parentFrame(&copy) , container(copy.container), IDname(copy.IDname){
	this->componentType = ComponentType::frame;
	this->leftPadding = copy.leftPadding;
}

//	Frame constructor Used within split method setting new values using splitters
Frame::Frame(Frame& parentFrame, const unsigned short max_x, const unsigned short min_x, const unsigned short max_y, const unsigned short min_y) :
	OComponent(max_x, min_x, max_y, min_y), parentFrame(&parentFrame) {
	this->componentType = ComponentType::frame;
}

//	Frame destructor
Frame::~Frame() {
	for (int i = 0; i < elNum; i++) {	
		delete elements[i];
	}
	delete[]elements;
}

//	Update frames container recursive for all parent frames
void Frame::_AddFrame(Frame* newFrame) {
	Frame** ptr = new Frame*(nullptr);
	utility::_AddElement(container.frames, *ptr, container.frameNum);
	container.frames[container.frameNum - 1] = newFrame;
	delete ptr;
	if (parentFrame != nullptr)
		parentFrame->_AddFrame(newFrame);
}

void Frame::_RemoveFrame(Frame* frame) {
	for (int i = 0; i < container.frameNum; i++) {
		if (container.frames[i]->id == frame->id) {
			utility::_RemoveElement(container.frames, i, container.frameNum);
			break;
		}
	}
	if (parentFrame != nullptr)
		parentFrame->_RemoveFrame(frame);
}

void Frame::_RemoveFrame() {
	if (parentFrame != nullptr)
		parentFrame->_RemoveFrame(this);
}

//	Method used to create new frames within a frame using spliters
void Frame::_Split(Separator& separator, const char* firstID, const char* secondID) {
	Coordinates coord = separator._GetCoordinates();

	//	If sperator is x direction
	if (separator.direction == 0) {		
		Frame* newFrame = new Frame(*this, max_x, min_x, coord.y1, min_y);
		newFrame->IDname = firstID;
		_AddFrame(newFrame);
		newFrame = new Frame(*this, max_x, min_x, max_y , coord.y1+1);
		newFrame->IDname = secondID;
		_AddFrame(newFrame);
	}
	//	If separator is y direction
	else if (separator.direction == 1) {
		Frame* newFrame = new Frame(*this, coord.x1, min_x, max_y, min_y);
		newFrame->IDname = firstID;
		_AddFrame(newFrame);
		newFrame = new Frame(*this, max_x, coord.x1 + 1, max_y, min_y);
		newFrame->IDname = secondID;
		_AddFrame(newFrame);
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
		_AddFrame(newFrame);
		newFrame = new Frame(*this, max_x, min_x, max_y, splitPos + 1);
		newFrame->IDname = secondID;
		_AddFrame(newFrame);
	}
	//	If separator is y direction
	else if (dir == 1) {
		unsigned short splitPos = (max_x - min_x) * percent / 100;
		Frame* newFrame = new Frame(*this, min_x + splitPos-1, min_x, max_y, min_y);
		newFrame->IDname = firstID;
		_AddFrame(newFrame);
		newFrame = new Frame(*this, max_x, min_x + splitPos-1, max_y, min_y);
		newFrame->IDname = secondID;
		_AddFrame(newFrame);
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
	elements[elNum - 1] = newElement._Clone();
	elements[elNum - 1]->_SetParentFrame(this);
	elements[elNum - 1]->_SetX1(this->x1);
	newElement._SetParentFrame(this);
	newElement._SetX1(this->x1);
	delete element;
}

//	Show all frame elements
void Frame::_ShowElements() {
	nextYpos = 0;
	for (int i = 0; i < elNum; i++) {
		if(elements[i]->_YposSet() == false)
			elements[i]->_SetYpos(nextYpos++);
		if(elements[i]->_AutoDisplay() == true)
			elements[i]->_Show();
	}
}

//	Add frame padding
void Frame::_AddPadding(short padding) {
	x1 += padding;
	x2 -= padding;
	y1 += padding;
	y2 -= padding;
}

void Frame::_AddLeftPadding(short padding) {
	x1 += padding;
	this->leftPadding = padding;
}

void Frame::_AddRightPadding(short padding) {
	x2 -= padding;
}

void Frame::_AddTopPadding(short padding) {
	y1 += padding;
}

void Frame::_AddBottomPadding(short padding) {
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
	_AddFrame(newFrame);
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

// FrameElement copy constructor
FrameElement::FrameElement(const FrameElement& copy) : OComponent(copy){
	this->paddingSet = copy.paddingSet;
	this->yposSet = copy.yposSet;
	this->align = copy.align;
	this->padding = copy.padding;
	this->Ypos = copy.Ypos;
	this->parentFrame = copy.parentFrame;
	this->original = copy.original;
	this->clone = copy.clone;
	this->autoDisplay = copy.autoDisplay;
}

//	FrameElement overloaded copy constructor
FrameElement::FrameElement(const FrameElement& copy, Frame* parentFrame) : OComponent(*parentFrame), parentFrame(parentFrame) {}
	
//	Find corresponding display for this frame element
Display* FrameElement::_GetDisplay() {
	Frame* select = parentFrame;
	if (select != nullptr) {
		do {
			if (select->dsp != nullptr)
				return select->dsp;
			select = select->parentFrame;
		} while (select != nullptr);
	}
	return nullptr;
}

//	Return label display position using align property
Cursor Label::_Align() {
	Cursor pos;
	Frame::Coordinates coord = parentFrame->_GetCoordinates();
	switch (utility::_ReadAlign(align)) {
	case 1:
		//	Left align
		pos._SetXY(coord.x1+padding+offset, coord.y1+Ypos);	
		break;
	case 2:
		//	 Right align
		pos._SetXY(coord.x2 - _Length()-padding-offset, coord.y1+Ypos);		
		break;
	case 3:
		//	Center align
		short frameSize = coord.x2 - coord.x1;
		short margin = (frameSize- _Length())/2;
		pos._SetXY(coord.x1+margin+offset, coord.y1+Ypos);		
	}
	return pos;
}

//	Display label
void Label::_Show() {
	short max_x = parentFrame->_GetCoordinates().x2;
	Cursor pos = _Align();
	Display* dsp = _GetDisplay();	
	cut = ((pos._GetX() + _Length()) > max_x) ? ((pos._GetX() + _Length())-max_x) : 0;

	dsp->_Display(this, pos);
	this->_SetVisible(true);
}

void Label::_Hide() {
	Display* dsp = _GetDisplay();
	if (dsp != nullptr) {
		dsp->_HideContent(this->id);
	}
	this->_SetVisible(false);

	if (clone != nullptr) {
		dsp = clone->_GetDisplay();
		dsp->_HideContent(clone->id);
	}
	else if (original != nullptr) {
		dsp = original->_GetDisplay();
		dsp->_HideContent(original->id);
	}
}

void MenuItem::_Hide() {
	Label::_Hide();
	if (parentMenu != nullptr) {
		Menu* clonedMenu = dynamic_cast<Menu*>(parentMenu->_Cloned());
		if (clonedMenu != nullptr) {
			MenuItem* clone = clonedMenu->_GetItem(this->text);
			if (clone != nullptr) {
				clone->_SetVisible(false);
				Display* dsp = clone->_GetDisplay();
				dsp->_RemoveID(clone->id);
			}
		}
		Menu* originalMenu = dynamic_cast<Menu*>(parentMenu->_Original());
		if (originalMenu != nullptr) {
			MenuItem* original = originalMenu->_GetItem(this->text);
			if (original != nullptr) {
				original->_SetVisible(false);
				Display* dsp = original->_GetDisplay();
				dsp->_RemoveID(original->id);
			}
		}
	}
}

MenuItem::MenuItem(const char* text, Module* previousModule) : Label(text) {
	link = *previousModule;
	this->componentType = ComponentType::menuItem;
}

//	Display menu item
void MenuItem::_Show() {
	short max_x = parentFrame->_GetCoordinates().x2;
	Cursor pos = _Align();
	Display* dsp = _GetDisplay();
	cut = ((pos._GetX() + _Length()) > max_x) ? ((pos._GetX() + _Length()) - max_x) : 0;
	
	if (id == 2137) {
		int test = 0;
	}

	dsp->_Display(this, pos);
	this->visible = true;

	if (clone != nullptr)
		clone->_SetVisible(true);
	else if (original != nullptr)
		original->_SetVisible(true);

	if (parentMenu != nullptr) {
		Menu* clonedMenu = dynamic_cast<Menu*>(parentMenu->_Cloned());
		if (clonedMenu != nullptr) {
			MenuItem* clone = clonedMenu->_GetItem(this->text);
			if (clone != nullptr) {
				clone->_SetVisible(true);
			}
		}
		Menu* originalMenu = dynamic_cast<Menu*>(parentMenu->_Original());
		if (originalMenu != nullptr) {
			MenuItem* original = originalMenu->_GetItem(this->text);
			if (original != nullptr) {
				original->_SetVisible(true);
			}
		}
	}
}

//	draw line - 0 = x spawn direction, 1 = y spawn direction
Separator::Separator(Layout& layout, short length, bool direction, unsigned short start_X, unsigned short start_Y) :
	FrameElement(layout._Select()), length(length), direction(direction){
	_SetValue(*layout._Select(), start_X, start_Y);
	this->componentType = ComponentType::separator;
}

Separator::Separator(Frame& parentFrame, short length, bool direction, unsigned short start_X, unsigned short start_Y) : 
	FrameElement(&parentFrame) , length(length), direction(direction)  {
	_SetValue(parentFrame, start_X, start_Y);
	this->componentType = ComponentType::separator;
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
	this->componentType = ComponentType::separator;
}

void Layout::_DefaultFrameTemplate(Display& dsp) {
	Frame::Coordinates coord = parentFrame->_GetCoordinates();
	frame = new Frame(coord.x2 - 1, coord.x1 + 1, coord.y2 - 1, coord.y1 + 1);
	frame->_SetIDname("DefaultLayout");
	frame->_SetDisplay(dsp);
	
	Separator menuLine(*this, 24, 1, 22, 0);
	Separator headerLine(*this, ::width - 4, 0, 2, 2);
	Separator footerLine(*this, 80 , 0, 2, ::height - 3);

	frame->_Split(headerLine, "Header", "Body");
	frame->_Select("Body")->_Split(footerLine, "Center", "Footer");
	frame->_Select("Header")->_Split(menuLine, "MenuHeader", "BodyHeader");
	frame->_Select("BodyHeader")->_Split(90,"vertical", "Title", "Date");
	frame->_Select("BodyHeader")->_Split(50, "vertical", "SelectionTitle", "ExtensionTitle");
	frame->_Select("Center")->_Split(menuLine, "Menu", "Content");
	frame->_Select("Content")->_Split(50, "vertical", "MainForm", "ExtensionForm");

	frame->_Select("SelectionTitle")->_AddLeftPadding(4);
	frame->_Select("ExtensionTitle")->_AddLeftPadding(2);
	frame->_Select("Menu")->_AddLeftPadding(1);
	frame->_Select("Content")->_AddLeftPadding(4);
	frame->_Select("MainForm")->_AddLeftPadding(4);
	frame->_Select("ExtensionForm")->_AddLeftPadding(2);
	frame->_Select("Footer")->_AddLeftPadding(1);

	dsp._Display(menuLine);
	dsp._Display(headerLine);
	dsp._Display(footerLine);
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

Menu::Menu(const Menu& copy) : FrameElement(copy), size(copy.size), linkNum(copy.linkNum){
	this->componentType = ComponentType::menu;
	items = new MenuItem*[size];
	for (int i = 0; i < size; i++)
		items[i] = new MenuItem(*copy.items[i]);
}

Menu::~Menu() {
	for (int i = 0; i < size; i++)
		delete items[i];
	delete[]items;
}

// Add new menu element
void Menu::_AddItem(MenuItem& item) {
	MenuItem** pp = new MenuItem*(nullptr);
	utility::_AddElement(items,*pp, size);
	items[size - 1] = item._Clone();
	items[size - 1]->_SetParentMenu(this);
	item._SetParentFrame(this->parentFrame);
	item._SetParentMenu(this);
	delete pp;
}

// Change menu item using label text
void Menu::_ChangeItem(const char* text, const char* newText, const char* newLink, bool recursionFlag) {
	for (int i = 0; i < size; i++) {
		if (utility::_CompareChar(items[i]->text, (char*)text)) {
			if (items[i]->id == 2137) {
				int test = 0;
			}

			items[i]->_SetText(newText);
			items[i]->_SetLink(newLink);
			
			if (items[i]->_ParentFrame() != nullptr && items[i]->_Visible()) {				
				items[i]->_Hide();
				items[i]->_Show();
			}
		}
	}
	if (!recursionFlag) {
		if (clone != nullptr)
			dynamic_cast<Menu*>(clone)->_ChangeItem(text, newText, newLink, true);
		else if (original != nullptr)
			dynamic_cast<Menu*>(original)->_ChangeItem(text, newText, newLink, true);
	}	
}

//	display menu items
void Menu::_Show() {
	if (this->visible == false) {
		char num[] = "[ ] ";
		Menu* org = nullptr;
		Menu* clon = nullptr;
		if (original != nullptr)
			org = dynamic_cast<Menu*>(original);
		else if (clone != nullptr)
			clon = dynamic_cast<Menu*>(clone);
		for (int i = 0; i < size; i++) {
			num[1] = i + 1 + '0';
			items[i]->_SetParentFrame(parentFrame);
			items[i]->_SetOrderNumber(num);
			if (items[i]->_YposSet() == false)
				items[i]->_SetYpos(i);
			if (org != nullptr) {
				org->items[i]->_SetParentFrame(parentFrame);
				org->items[i]->_SetOrderNumber(num);
				if (org->items[i]->_YposSet() == false)
					org->items[i]->_SetYpos(i);
				
			}
			if (clon != nullptr) {
				clon->items[i]->_SetParentFrame(parentFrame);
				clon->items[i]->_SetOrderNumber(num);
				if (clon->items[i]->_YposSet() == false)
					clon->items[i]->_SetYpos(i);
			
			}
			if (items[i]->_AutoDisplay()) {
				items[i]->_Show();
				if(clon != nullptr)
					clon->items[i]->_SetVisible(true);
				if(org != nullptr)
					org->items[i]->_SetVisible(true);
			}
		}
		this->visible = true;
	}
}

//	hide menu items
void Menu::_Hide() {
	for (int i = 0; i < size; i++)
		items[i]->_Hide();
	this->visible = false;
}

void TextBar::_AddItem(Label& item) {
	Label** pp = new Label*(nullptr);
	utility::_AddElement(items, *pp, num);
	items[num - 1] = new Label(item);
	delete pp;
}

void TextBar::_Show() {
	this->length = 0;
	for (int i = 0; i < num; i++)
		length += items[i]->length;
	length += ((num - 1)*spacing);

	Cursor pos = this->_Align();
	Frame::Coordinates coord = parentFrame->_GetCoordinates();
	int offset = pos._GetX() - coord.x1;
	int padding = 0;
	for (int i = 0; i < num; i++) {
		items[i]->_SetOffset(offset);
		items[i]->_SetPadding(padding);
		items[i]->_SetParentFrame(parentFrame);
		items[i]->_SetYpos(this->Ypos);
		items[i]->_Show();
		padding = items[i]->length + spacing;
	}
	this->visible = true;
}

TextBar::TextBar(TextBar& copy) : num(copy.num), spacing(copy.spacing) {
	this->componentType = ComponentType::textBar;
	this->align = copy.align;
	items = new Label*[num];
	for (int i = 0; i < num; i++)
		items[i] = new Label(*copy.items[i]);
}

TextBar::~TextBar() {
	for (int i = 0; i < num; i++)
		delete items[i];
	delete[]items;
}

std::vector<FrameElement*> Frame::_SelectElements(ComponentType type) {
	std::vector<FrameElement*> container;
	for (int i = 0; i < elNum; i++) {
		if (elements[i]->componentType == type) {
			if(elements[i]->_Original() != nullptr)
				container.push_back(elements[i]->_Original());
			else container.push_back(elements[i]);
		}
	}
	return container;
}

std::vector<FrameElement*> Layout::_SelectElements(ComponentType type) {
	std::vector<FrameElement*> container;
	Frame::Container sub = frame->_GetSubFrames();
	for (int i = 0; i < sub.frameNum; i++) {
		std::vector<FrameElement*> subContainer = sub.frames[i]->_SelectElements(type);
		for (unsigned int j = 0; j < subContainer.size(); j++)
			container.push_back(subContainer.at(j));
	}
	return container;	
}

Table::Table(Frame* parentFrame, int rowNum, int colNum) : rowNum(rowNum), colNum(colNum), FrameElement(parentFrame){
	this->componentType = ComponentType::table;

	Frame::Coordinates coord = parentFrame->_GetCoordinates();
	int cellWidth = (coord.x2 - coord.x1 ) / colNum;

	char cellName[] = { "0,0" };
	int cellY = coord.y1;
	cells = new Frame**[rowNum];
	for (int i = 0; i < rowNum; i++) {
		cells[i] = new Frame*[colNum];
		int cellX = coord.x1;
		cellName[0] = (i+1) + '0';
		for (int j = 0; j < colNum; j++) {
			cellName[2] = (j+1) + '0';
			cells[i][j] = new Frame(*parentFrame, cellX + cellWidth, cellX, cellY + 1, cellY);
			cells[i][j]->IDname = utility::_CopyChar(cellName);
			cellX = cellX + cellWidth;
			if (j == colNum - 1) 
				cells[i][j]->x2 = coord.x2-2;
		}
		cellY++;
	}	
	borderPos = new std::vector<Cursor>;
}

Table::Table(const Table& copy) : rowNum(copy.rowNum), colNum(copy.colNum), showBorder(copy.showBorder) {
	this->componentType = ComponentType::table;
	cells = new Frame**[rowNum];
	for (int i = 0; i < rowNum; i++) {
		cells[i] = new Frame*[colNum];
		for (int j = 0; j < colNum; j++) 
			cells[i][j] = copy.cells[i][j];			
	}
	deleteCells = false;
	borderPos = new std::vector<Cursor>(*copy.borderPos);
}

void Table::_DrawBorder() {
	Frame::Coordinates coord = parentFrame->_GetCoordinates();
	Display* dsp = _GetDisplay();
	for (int i = 0; i < rowNum; i++) {
		for (int j = 0; j < colNum; j++) {
			Frame::Coordinates coord = cells[i][j]->_GetCoordinates();
			Cursor pos(coord.x1-1, coord.y1-1 + i);
			for (int k = coord.x1; k < coord.x2; k++) 
				dsp->_Display(::horizontalLine);
			pos._ChangeY(1);
			pos._SetCursorPosition();
			dsp->_Display(::verticalLine);
			Cursor pos2(coord.x2-1, pos._GetY());
			dsp->_Display(::verticalLine);
			pos._ChangeY(1);
			pos._SetCursorPosition();
			for (int k = coord.x1; k < coord.x2; k++)
				dsp->_Display(::horizontalLine);
		}
	}
}

void Table::_Show() {
	if (this->visible == false) {
		Display*dsp = _GetDisplay();
		Cursor pos;
		if (showBorder)
			_DrawBorder();
		for (int i = 0; i < rowNum; i++) {
			for (int j = 0; j < colNum; j++) {
				if (showBorder)
					cells[i][j]->_AddTopPadding(i);
				else if (cellSeparator && j > 0)
					cells[i][j]->_AddLeftPadding(2);
				cells[i][j]->_ShowElements();
				if (cellSeparator && j < colNum - 1) {
					Frame::Coordinates coord = cells[i][j]->_GetCoordinates();
					pos._SetXY(coord.x2, coord.y1);
					pos._SetCursorPosition();
					dsp->_Display(::verticalLine);
					pos._SetCharacterNumber(1);
					borderPos->push_back(pos);
				}
				if (cellSeparator && j > 0)
					cells[i][j]->_AddLeftPadding(-2);
				if (showBorder)
					cells[i][j]->_AddTopPadding(-i);
				pos._GetCursorPosition();
			}
		}
		this->visible = true;
	}
}

Table::~Table() {
	for (int i = 0; i < rowNum; i++) {
		if (deleteCells) {
			for (int j = 0; j < colNum; j++) {
				delete[] cells[i][j]->IDname;
				delete cells[i][j];
			}
		}
		delete[]cells[i];
	}
	delete[]cells;
	delete borderPos;
}

FrameElement::~FrameElement() {
	if (this->clone != nullptr) 
		clone->original = nullptr;
	else if (this->original != nullptr)
		original->clone = nullptr;
}

void Table::_SetColumnWidth(int columnIndex, int newWidth) {
	for (int i = 0; i < rowNum; i++) {
		Frame::Coordinates coord = cells[i][columnIndex]->_GetCoordinates();
		int oldWidth = coord.x2 - coord.x1;
		int diff = newWidth - oldWidth + cells[i][columnIndex]->parentFrame->leftPadding;
		coord.x2 += diff;		
		cells[i][columnIndex]->_ChangeCoordinates(coord);
		if (columnIndex + 1 < colNum) {
			Frame::Coordinates coord = cells[i][columnIndex + 1]->_GetCoordinates();
			coord.x1 += diff;
			cells[i][columnIndex + 1]->_ChangeCoordinates(coord);
		}
	}
}

Frame* Frame::_GetMainFrame() {
	Frame* selector = this;
	while (true) {
		if(selector->parentFrame!= nullptr)
			selector = selector->parentFrame;
		else break;
	}
	return selector;
}

void IDLabel::_Show() {
	Label::_Show();
	if (parentFrame != nullptr) {
		Cursor pos;
		pos._ChangeX(1);
		Frame::Coordinates coord = parentFrame->_GetCoordinates();
		int length = coord.x2 - pos._GetX();
		int digitNum = utility::_DigitNumberInt(this->id);
		length = length - digitNum;
		Display* dsp = _GetDisplay();
		for (int i = 0; i < length; i++) {
			dsp->_Display(pos, 0);
			pos._ChangeX(1);
		}
		dsp->_Display(pos, id);
	}
	this->visible = true;
}

void Menu::_SetParentFrame(Frame* parentFrame) {
	FrameElement::_SetParentFrame(parentFrame);
	for (int i = 0; i < size; i++)
		items[i]->_SetParentFrame(parentFrame);
}

void Frame::_HideElements() {
	for (int i = 0; i < elNum; i++)
		elements[i]->_Hide();
}

void Table::_Hide() {
	for (int i = 0; i < rowNum; i++) {
		for (int j = 0; j < colNum; j++) 
			cells[i][j]->_HideElements();
	}
	for (size_t i = 0; i < borderPos->size(); i++)
		borderPos->at(i)._ClearText();
	if (clone != nullptr) 
		dynamic_cast<Table*>(clone)->_Hide();
	this->visible = false;
}

void TextBar::_Hide() {
	for (int i = 0; i < num; i++)
		items[i]->_Hide();
	this->visible = false;
}

void IDLabel::_Hide() {
	Display* dsp = _GetDisplay();
	Cursor pos(parentFrame->_X1(), parentFrame->_Y1());
	for (int i = 0; i < parentFrame->_Width(); i++)
		dsp->_Display(::spaceKey);
	this->visible = false;
}

void Frame::_ClearFrame() {
	for (int i = 0; i < _Width(); i++) {
		for (int j = 0; j < _Height(); j++) {
			dsp->_Display(::spaceKey);
		}
	}
}