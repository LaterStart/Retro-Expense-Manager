#pragma once

class Console;
class Initialize {
private:
	Initialize() = delete;
	~Initialize() = delete;

	static bool initialized;

public:
	static Console* _Console();
};

class IOComponent {};

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

class Separator; class Frame; class Label; class Layout;
class Display : public IOComponent {
private:
	unsigned short activePosNum = 0;
	Cursor* activePositions = nullptr;

	void _AddActivePosition(Cursor pos);
	void _ExtendRegister();
	void _Show(const char* content);
	void _Show(const char* content, unsigned char symbol);

public:
	Display() = default;
	~Display();

	template <typename T>
	void _Display(Cursor& pos, unsigned char symbol, T element) {
		pos._SetCursorPosition();
		pos._SetCharacterNumber(utility::_CharLength(element)+1);
		_AddActivePosition(pos);
		_Show(element, symbol);
	}
	template<typename T, typename ... TT>
	void _Display(Cursor& pos, unsigned char symbol, T element, TT ... nextElements) {
		pos._SetCursorPosition();
		pos._SetCharacterNumber(utility::_CharLength(element));
		_AddActivePosition(pos);
		_Show(element);
		pos._GetCursorPosition();
		_Display(pos, symbol, nextElements...);
	}
	
	template <typename T>
	void _Display(Cursor& pos, T element) {
		pos._SetCursorPosition();
		pos._SetCharacterNumber(utility::_CharLength(element));
		_AddActivePosition(pos);
		_Show(element);
	}
	template<typename T, typename ... TT>
	void _Display(Cursor& pos, T element, TT ... nextElements) {
		pos._SetCursorPosition();
		pos._SetCharacterNumber(utility::_CharLength(element));
		_AddActivePosition(pos);
		_Show(element);
		pos._GetCursorPosition();
		_Display(pos, nextElements...);
	}

	void _ClearContent();
	void _Display(unsigned char ch);
	/*void _Display(const char* content);
	void _Display(const char* content, Cursor &pos);
	void _Display(const char* content, int cut);	
	void _Display(const char* content, Cursor& pos, int cut);
	void _Display(const char* content, unsigned char symbol);
	void _Display(const char* content, unsigned char symbol, Cursor& pos);*/

	void _Display(Separator& separator);
	void _LockContent(Cursor &pos);
};