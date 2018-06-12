#pragma once
#include "IOComponent.h"
#include "../config.h"

class IComponent : public IOComponent {}; 

class OComponent : public IOComponent {	
private:
	void _SetInitialCoordinates();
protected:
	OComponent(const unsigned short min_x, const unsigned short max_x, const unsigned short min_y, const unsigned short max_y);
	OComponent(OComponent& parentFrame);
	OComponent(OComponent* outOfFrame);

	const unsigned short min_x;
	const unsigned short max_x;
	const unsigned short min_y;
	const unsigned short max_y;

	unsigned short x1, x2;
	unsigned short y1, y2;	

public:
	struct Coordinates {
		unsigned short x1, x2;
		unsigned short y1, y2;

		void operator=(const Coordinates& equals) {
			x1 = equals.x1;
			x2 = equals.x2;
			y1 = equals.y1;
			y2 = equals.y2;
		}
	};

protected:
	Coordinates coord;

public:
	void _SetX1(unsigned short);
	void _SetX2(unsigned short);
	void _SetY1(unsigned short);
	void _SetY2(unsigned short);
	void _SetX(unsigned short, unsigned short);
	void _SetY(unsigned short, unsigned short);	
	short _GetArea();

	Coordinates _GetCoordinates();
	~OComponent() = default;
};

inline void OComponent::_SetX1(unsigned short x1) {
	this->x1 = x1;
}

inline void OComponent::_SetX2(unsigned short x2) {
	this->x2 = x2;
}

inline void OComponent::_SetY1(unsigned short y1) {
	this->y1 = y1;
}

inline void OComponent::_SetY2(unsigned short y2) {
	this->y2 = y2;
}

inline void OComponent::_SetX(unsigned short x1, unsigned short x2) {
	this->x1 = x1;
	this->x2 = x2;
}

inline void OComponent::_SetY(unsigned short y1, unsigned short y2) {
	this->y1 = y1;
	this->y2 = y2;
}

inline OComponent::Coordinates OComponent::_GetCoordinates()  {
	coord.x1 = x1;
	coord.x2 = x2;
	coord.y1 = y1;
	coord.y2 = y2;
	return coord;
}

inline short OComponent::_GetArea() {
	return (x2 - x1) * (y2 - y1);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

class FrameElement;
class Frame : public OComponent{	
private:
	friend class Console;
	friend class Layout;
	Frame(const unsigned short max_x, const unsigned short min_x, const unsigned short max_y, const unsigned short min_y);
	Frame(Frame& parentFrame, const unsigned short max_x, const unsigned short min_x, const unsigned short max_y, const unsigned short min_y);	
	void _UpdateContainer(Frame* newFrame);	

	void _AddElement(FrameElement& newElement);
public:
	struct Container {
		Frame** frames = nullptr;
		int frameNum = 0;
		Container() = default;
		Container(const Container& copy);
		~Container();
	};

	short nextYpos = 0;
	const char* IDname = nullptr;
	Frame(Frame& parentFrame);
	Container _GetSubFrames();
	void _Split(Separator& separator, const char* firstID, const char* secondID);
	void _Split(unsigned short percent, const char* direction, const char* firstID, const char* secondID);
	void _CreateSubFrame(const char* IDname, Coordinates& coord);
	void _SetIDname(const char* IDname);
	void _SetDisplay(Display& dsp);
	void _ChangeCoordinates(Coordinates& coord);
	Frame* _Select(const char* IDname);
	Frame* _GetParentFrame();

	Frame * parentFrame = nullptr;
	Display* dsp = nullptr;
	FrameElement** elements = nullptr;
	int elNum = 0;		

	void _AddPadding(unsigned short padding);	
	void _ShowElements();

	//	Add more FrameElements using variadic template
	template <typename T>
	void _AddElements(T& element) {
		_AddElement(element);
	}
	template<typename T , typename ... TT>
	void _AddElements(T& element, TT& ... nextElements ) {
		_AddElement(element);
		_AddElements(nextElements...);
	}

	~Frame();

private:
	Container container;
};

inline void Frame::_SetIDname(const char* IDname) {
	this->IDname = IDname;
}

inline Frame::Container Frame::_GetSubFrames() {
	return container;
}

inline Frame* Frame::_GetParentFrame() {
	return parentFrame;
}

inline void Frame::_SetDisplay(Display& dsp) {
	this->dsp = &dsp;
}

class FrameElement : public OComponent {	
protected:
	const char* align = "left";
	unsigned short padding = 0;
	short Ypos = 0;
	Frame* parentFrame;
	FrameElement();
	FrameElement(Frame* parentFrame);	

	Display* _GetDisplay();
	virtual Cursor _Align();

public:
	FrameElement(const FrameElement& copy, Frame* parentFrame);
	void _SetParentFrame(Frame* parentFrame);
	
	virtual void _Show();
	virtual void _Hide();
	void _SetAlign(const char* align);
	void _SetYpos(short y);
	void _SetPadding(unsigned short padding);
};

inline void FrameElement::_Show() {}

inline void FrameElement::_Hide() {}

inline void FrameElement::_SetAlign(const char* align) {
	this->align = align;
}

inline void FrameElement::_SetPadding(unsigned short padding) {
	this->padding = padding;
}

inline Cursor FrameElement::_Align() {
	Cursor pos;
	return pos;
}

inline void FrameElement::_SetYpos(short y) {
	this->Ypos = y;
}

inline void FrameElement::_SetParentFrame(Frame* parentFrame) {
	this->parentFrame = parentFrame;
}

class Separator : public FrameElement {
private:
	friend void Display::_Display(Separator& separator);
	void _SetValue(Frame& parentFrame, unsigned short start_X, unsigned short start_Y);

public:
	short length;
	bool direction;

	// draw line - 0 = x spawn direction, 1 = y spawn direction
	Separator(Frame& parentFrame, short length, bool direction, unsigned short start_X, unsigned short start_Y);
	Separator(Layout& layout, short length, bool direction, unsigned short start_X, unsigned short start_Y);
	Separator(const Separator& copy);
};


class Label : public FrameElement {
protected:
	Cursor _Align();

public:	
	char* text = nullptr;
	short length = 0;
	unsigned char symbol = 0;
	unsigned short cut = 0;

	Label() = default;
	Label(const Label& copy) : text(copy.text), length(copy.length) {}
	Label(const char* text) : text((char*)text) { length = utility::_CharLength(text); }
	Label(const char* text, const char* align) : text((char*)text) { length = utility::_CharLength(text); this->align = align; }
	Label(const char* text, unsigned char symbol, const char* align) : text((char*)text), symbol(symbol) {length = utility::_CharLength(text) + 1; this->align = align;  }

	virtual void _Show() override;
	virtual void _Hide() override;
};

class Layout {	
private:
	Frame * parentFrame;
	Frame * frame = nullptr;

public:
	Layout(Frame* parentFrame) : parentFrame(parentFrame) {}
	Frame * _Select();
	Frame* _Select(const char* IDname);
	void _Split(Separator& separator, const char* firstID, const char* secondID);
	void _DefaultFrameTemplate(Display& dsp);
	void _ShowElements();

	~Layout();
};

class MenuItem;
class Menu : public FrameElement {
private:
	void _AddItem(MenuItem& item);

public:
	int size = 0;
	int linkNum = 0;
	MenuItem** items = nullptr;	

	//	Add more Menu items using variadic template
	template <typename T>
	void _AddItems(T& item) {
		_AddItem(item);
	}
	template<typename T, typename ... TT>
	void _AddItems(T& item, TT& ... nextItems) {
		_AddItem(item);
		_AddItems(nextItems...);
	}
	const char* _GetLink(int selection);
	void _ChangeItem(MenuItem& item, int pos);
	void _Show() override;

	Menu() = default;
	Menu(const Menu& copy){}
	~Menu();
};

class Module;
class MenuItem : public Label {
private:
	friend const char* Menu::_GetLink(int selection);

	const char* prefix = nullptr;
	const char* link = nullptr;

public:
	char* orderNum = nullptr;
	MenuItem(const char* text, const char* moduleName) : Label(text), link(moduleName){}
	MenuItem(const char* text, Module* previousModule);
	void _SetOrderNumber(char* orderNum);
	void _SetSpecialPrefix(const char* prefix);
	void _Show() override;
};

inline void MenuItem::_SetOrderNumber(char* orderNum) {
	if(prefix==nullptr)
		this->orderNum = orderNum;
	else {
		this->orderNum = (char*)prefix;
		Ypos++;
	}
	length += utility::_CharLength(orderNum);
}

inline const char* Menu::_GetLink(int selection) {
	return items[selection-1]->link;
}

inline void MenuItem::_SetSpecialPrefix(const char* prefix) {
	this->prefix = prefix;
}