#pragma once
#include "../utility.h"

class IOComponent {
public:
	static int idCounter;
	int id;
	IOComponent() {
		id = idCounter;
		idCounter++;
	}
};

class Frame;
class Console : public IOComponent {
private:
	char* _SystemMode();

	Console();
	~Console();

	Frame* mainFrame;

	void _Initialize();
	void _DrawFrame();
	static Console& _GetInstance();

	friend Console* Initialize::_Console();

public:
	Console(Console const&) = delete;
	void operator=(Console const&) = delete;

	Frame* _GetMainFrame() const;
};

inline Frame* Console::_GetMainFrame() const {
	return mainFrame;
}

class Cursor : public IOComponent {
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
	void _SetCharacterNumber(short);
	void _ChangeCharacterNumber(short);
	void _SetXY(short, short);
	void _MoveToXY(short x, short y);

	typedef void(Cursor::*Cptr)(short);
	Cptr ptr;
};

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
inline void Cursor::_SetCharacterNumber(short num) {
	n = num;
}
inline void Cursor::_ChangeCharacterNumber(short num) {
	n += num;
}
inline void Cursor::_SetXY(short x, short y) {
	this->x = x;
	this->y = y;
}

class Link : public IOComponent {
public:
	unsigned short selectValue;
	const char* moduleName;

	Link(unsigned short value, const char* name) : selectValue(value), moduleName(name){}
	Link(const char* name) : moduleName(name){}

};

class Separator; class Label; class MenuItem; class Model;
class Display : public IOComponent {
private:
	struct ActivePos {
		Cursor pos;
		int elementID;

		ActivePos() = default;
		ActivePos(Cursor pos, int elementID) : pos(pos), elementID(elementID){}
		ActivePos(const ActivePos& copy) : pos(copy.pos), elementID(copy.elementID){}
		bool operator == (ActivePos& equal) {
			if (elementID == equal.elementID && pos == equal.pos)
				return true;
			else return false;
		}

		void _ClearContent() {
			pos._ClearText();
			pos._SetCharacterNumber(0);
		}
	};

	int activePosNum = 0;
	ActivePos* activePositions = nullptr;

	void _AddActivePosition(ActivePos);
	void _ExtendRegister();
	void _Show(const char* content);
	void _Show(const char* content, unsigned char symbol);

public:
	Display() = default;
	~Display();

	template <typename T>
	void _Display(Cursor& pos, T item) {
		pos._SetCursorPosition();
		pos._SetCharacterNumber(utility::_CharLength(item));
		_Display(item);
		ActivePos apos(pos, -1);
		_AddActivePosition(apos);
	}
	template<typename T, typename ... TT>
	void _Display(Cursor& pos, T item, TT ... nextItems) {
		_Display(pos, item);
		pos._GetCursorPosition();
		_Display(pos, nextItems...);
	}

	bool _IsEmpty();
	void _Backspace();
	void _HideContent(int elementID);
	void _WipeContent();
	void _Display(char ch);
	void _Display(const char* text);
	void _Display(Label* label, Cursor& pos = Cursor());
	void _Display(MenuItem* item, Cursor& pos = Cursor());
	void _Display(Model& item, Cursor& pos = Cursor());
	void _Display(Separator& separator);
	void _Display(Cursor& pos, int num);
	void _LockContent(Cursor &pos);
	void _Loading();
};

inline bool Display::_IsEmpty() {
	if (activePosNum == 0)
		return true;
	else return false;
}
#include "OComponent.h"