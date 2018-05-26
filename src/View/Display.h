#pragma once
#include "IOComponent.h"

class Separator : public OComponent {
public:
	short startX;
	short startY;
	short length;
	bool direction;

public:
	// draw line - 0 = x spawn direction, 1 = y spawn direction
	Separator(short startX, short startY, short length, bool direction) : startX{ startX }, startY{ startY }, length { length }, direction{ direction } {}
	Separator() {}
};

class Cursor;
class Display {
private:
	unsigned short activePosNum;
	Cursor* activePositions;

	void _AddActivePosition(Cursor pos);
	void _ExtendRegister();

	Separator* frameSeparators;
	int sepNum;

public:
	Display();
	~Display();
	
	void _ClearContent();
	void _DisplayCharacter(unsigned char ch);
	void _DisplayContent(const char* content, Cursor &pos);	
	void _DisplayContent(const char* content);	
	void _DisplayContent(const char* test, unsigned int cut);
	void _DisplayFrame_Default();
	void _DisplaySeparator(Separator& separator);
	void _LockContent(Cursor &pos);

	Separator* _GetFrameSeparators() const;
	int _GetFrameSeparatorsNum() const;
};

inline Separator* Display::_GetFrameSeparators() const {
	return frameSeparators;
}

inline int Display::_GetFrameSeparatorsNum() const {
	return sepNum;
}