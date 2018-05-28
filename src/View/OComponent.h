#pragma once
#include "IOComponent.h"

class IComponent : public IOComponent {}; 

class OComponent : public IOComponent {	
protected:
	OComponent(const unsigned short min_x, const unsigned short max_x, const unsigned short min_y, const unsigned short max_y);
	OComponent(OComponent& parentFrame);

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
	Frame(const unsigned short max_x, const unsigned short min_x, const unsigned short max_y, const unsigned short min_y);
	Frame(Frame& parentFrame, const unsigned short max_x, const unsigned short min_x, const unsigned short max_y, const unsigned short min_y);	
	void _UpdateContainer(Frame* newFrame);

protected:
	Frame * parentFrame;	

public:
	class Container {
	private:
		Frame * _MoveFrame(Frame& source);		

	public:
		Frame** frames;
		int frameNum;
		Container();
		Container(const Container& copy);
		~Container();
	};

	const char* IDname = nullptr;
	Frame(Frame& parentFrame);
	Coordinates _GetCoordinates();
	Frame _CreateSubFrame(const char* IDname);
	Container _GetSubFrames();
	void _Split(Separator& separator, const char* firstID, const char* secondID);
	void _SetIDname(const char* IDname);
	Frame* _Select(const char* IDname);

	FrameElement** elements = nullptr;
	int elNum = 0;

	void _AddElement(FrameElement newElement);

private:
	Container container;
};

inline Frame::Coordinates Frame::_GetCoordinates() {
	coord.x1 = min_x;
	coord.x2 = max_x;
	coord.y1 = min_y;
	coord.y2 = max_y;
	return coord;
}

inline void Frame::_SetIDname(const char* IDname) {
	this->IDname = IDname;
}

inline Frame::Container Frame::_GetSubFrames() {
	return container;
}

class FrameElement : public OComponent {
protected:
	Frame* parentFrame;
	FrameElement(Frame* parentFrame);	

public:
	FrameElement(const FrameElement& copy, Frame* parentFrame);
	void _SetParentFrame(Frame* parentFrame);
};

inline void FrameElement::_SetParentFrame(Frame* parentFrame) {
	this->parentFrame = parentFrame;
}

class Separator : public FrameElement {
	friend void Display::_Display(Separator& separator);
public:
	short length;
	bool direction;

	// draw line - 0 = x spawn direction, 1 = y spawn direction
	Separator(Frame& parentFrame, short length, bool direction, unsigned short start_X, unsigned short start_Y);	
	Separator(const Separator& copy);
};

class Label : public FrameElement {
	friend void Display::_Display(Label& label);
public:
	char* text;
	short length;
	
	Label(const char* text) : FrameElement(nullptr){}
	Label(Frame* parentFrame, char* text) : text(text), FrameElement(parentFrame){}
	Label(Frame* parentFrame, const char* text) : text((char*)text), FrameElement(parentFrame){}
	Label(Frame* parentFrame) : text(nullptr), FrameElement(parentFrame){}
};

#include "../utility.h"
class Menu : public Frame {
private:
	unsigned short x1, x2;
	unsigned short y1, y2;
	unsigned short x_min;
	unsigned short x_max;
	unsigned short y_min;
	unsigned short y_max;

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

	Menu(Frame*);
	Menu(Frame*, unsigned short width);
	~Menu();
};