#pragma once
#include "IOComponent.h"

class Separator : public FrameElement {
	friend class Display;
public:
	short length;
	bool direction;

public:
	// draw line - 0 = x spawn direction, 1 = y spawn direction
	Separator(short length, bool direction, unsigned short start_X, unsigned short start_Y) : length{ length }, direction{ direction } {
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
	}
	Separator() = default;
	Separator(const Separator& copy) : length{ copy.length }, direction{ copy.direction } {
		x1 = copy.x1;
		x2 = copy.x2;
		y1 = copy.y1;
		y2 = copy.y2;
	}
};

class Frame : public FrameElement {
	friend class Display;
public:
	short _GetArea();
};

inline short Frame::_GetArea() {
	return (x2 - x1) * (y2 - y1);
}

class Cursor;
class Display : public OComponent {
private:
	unsigned short activePosNum;
	Cursor* activePositions;

	void _AddActivePosition(Cursor pos);
	void _ExtendRegister();

	Separator* frameSeparators;
	int sepNum;

	Frame* frames;
	int frameNum;

public:
	Display();
	~Display();
	
	void _ClearContent();	
	void _DisplayCharacter(unsigned char ch);
	void _DisplayContent(const char* content, Cursor &pos);	
	void _DisplayContent(const char* content);	
	void _DisplayContent(const char* test, unsigned int cut);	
	void _DisplaySeparator(Separator& separator);
	void _DrawLayout_default();
	void _LockContent(Cursor &pos);

	Separator* _GetFrameSeparators() const;
	int _GetFrameSeparatorsNum() const;

	Frame* _GetFrames() const;
	int _GetFrameNum() const;
};

inline Separator* Display::_GetFrameSeparators() const {
	return frameSeparators;
}

inline int Display::_GetFrameSeparatorsNum() const {
	return sepNum;
}

inline Frame* Display::_GetFrames() const {
	return frames;
}

inline int Display::_GetFrameNum() const {
	return frameNum;
}