#pragma once
#include "PosManager.h"
//	CURSOR CLASS DECLARATION
/*---------------------------------------------------------------------------------------------------------------------*/
class Cursor {
private:
	short x : 8;
	short y : 8;
	short n : 8;

public:
	Cursor();
	Cursor(const Cursor&);
	Cursor(short x, short y);
	~Cursor();
		
	void _GetCursorPosition();
	void _SetCursorPosition();	
	
	/*
	void _SetCharacterNumber(const char*, int additional = 0);
	void _SetCharacterNumber(unsigned short value);
	
	int _GetN();
	*/

	bool operator==(const Cursor &second) const;	
	void _ChangeY(short);
	void _ChangeX(short);
	void _SetX(short);
	void _SetY(short);
	short _GetY();
	short _GetX();
	void _MoveX(short);
	void _MoveY(short);
	void _ClearText();
	void _SetCharacterNumber(unsigned short);
	void _SetXY(short,short);
};
/*---------------------------------------------------------------------------------------------------------------------*/

inline void Cursor::_ChangeY(short y_diff) {
	y += y_diff;
}
inline void Cursor::_ChangeX(short x_diff) {
	x += x_diff;
}
inline void Cursor::_SetX(short newValue) {
	x = newValue;
}
inline void Cursor::_SetY(short newValue) {
	y = newValue;
}
inline short Cursor::_GetY() {
	return y;
}
inline short Cursor::_GetX() {
	return x;
}
inline void Cursor::_SetCharacterNumber(unsigned short num) {
	n = num;
}
inline void Cursor::_SetXY(short x, short y) {
	this->x = x;
	this->y = y;
}

/*

class TextPositionManager {
private:
	int n;
	Cursor* activePositions;

	void _ExtendRegister();

public:
	TextPositionManager();
	~TextPositionManager();

	void _AddActivePosition(Cursor);
	void _WipeText();
	int _GetNum() const;
};*/