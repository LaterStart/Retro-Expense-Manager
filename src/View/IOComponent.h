#pragma once

class IOComponent {


};

class OComponent : public IOComponent {
	

};


class IComponent : public IOComponent {

	
}; 

struct Coordinates {
	unsigned short x1, x2;
	unsigned short y1, y2;
};

class FrameElement :  OComponent {
protected:
	unsigned short x1, x2;
	unsigned short y1, y2;
	Coordinates* coord;

public:
	void _SetX1(unsigned short);
	void _SetX2(unsigned short);
	void _SetY1(unsigned short);
	void _SetY2(unsigned short);
	void _SetX(unsigned short, unsigned short);
	void _SetY(unsigned short, unsigned short);
	Coordinates* _GetCoordinates() const;

	FrameElement() {
		coord = new Coordinates;
	}
	~FrameElement() {
		delete coord;
	}
};

inline void FrameElement::_SetX1(unsigned short x1){
	this->x1 = x1;
	coord->x1 = x1;
}

inline void FrameElement::_SetX2(unsigned short x2){
	this->x2 = x2;
	coord->x2 = x2;
}

inline void FrameElement::_SetY1(unsigned short y1){
	this->y1 = y1;
	coord->y1 = y1;
}

inline void FrameElement::_SetY2(unsigned short y2){
	this->y2 = y2;
	coord->y2 = y2;
}

inline void FrameElement::_SetX(unsigned short x1, unsigned short x2){
	this->x1 = x1;
	this->x2 = x2;
	coord->x1 = x1;
	coord->x2 = x2;
}

inline void FrameElement::_SetY(unsigned short y1, unsigned short y2){
	this->y1 = y1;
	this->y2 = y2;
	coord->y1 = y1;
	coord->y2 = y2;
}

inline Coordinates* FrameElement::_GetCoordinates() const {
	return coord;
}

#include "Cursor.h"
#include "Console.h"
#include "Display.h"
#include "Menu.h"