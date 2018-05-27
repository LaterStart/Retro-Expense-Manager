#include <iostream>
#include "Display.h"
#include "../config.h"
using namespace std;

Display::Display() : activePositions(nullptr), frameSeparators(nullptr), activePosNum(0), sepNum(0) {}

Display::~Display(){
	delete[]activePositions;
	delete[]frameSeparators;
}

void Display::_DisplayCharacter(unsigned char ch) {
	Cursor pos;
	cout << ch;
	pos._SetCharacterNumber(1);
	_AddActivePosition(pos);
}

void Display::_DisplayContent(const char* content, Cursor &pos) {
	pos._SetCursorPosition();
	cout << content;
	pos._SetCharacterNumber(utility::_CharLength(content));
	_AddActivePosition(pos);
}

void Display::_DisplayContent(const char* content) {
	Cursor pos;
	cout << content;
	pos._SetCharacterNumber((short)utility::_CharLength(content));
	_AddActivePosition(pos);
}

void Display::_DisplayContent(const char* content, unsigned int cut) {
	Cursor pos;
	int length = utility::_CharLength(content) - cut;
	for (int i = 0; i < length; i++)
		cout << content[i];
	
	pos._SetCharacterNumber(length);
	_AddActivePosition(pos);
}

void Display::_DisplaySeparator(Separator& separator) {
	Cursor pos(separator.x1, separator.y1);
	unsigned char line;
	if (separator.direction == 0) {
		pos.ptr = &Cursor::_MoveX;
		line = ::horizontalLine;
	}
	else if (separator.direction == 1) {	
		pos.ptr = &Cursor::_MoveY;
		line = ::verticalLine;
	}
	for (int i = 0; i < separator.length; i++) {
		cout << line;
		if (separator.direction == 1) {
			pos._SetCharacterNumber(1);
			_AddActivePosition(pos);
		}
		(pos.*(pos.ptr))(1);
	}
	if (separator.direction == 0) {
		Cursor pos(separator.x1, separator.y1);
		pos._SetCharacterNumber(separator.length);
		_AddActivePosition(pos);
	}
}

void Display::_AddActivePosition(Cursor pos) {
	bool newPos = true;

	if (activePositions != nullptr) {
		for (int i = 0; i < activePosNum; i++) {
			if (activePositions[i] == pos) {
				newPos = false;
				break;
			}
		}
	}
	if (newPos) {
		_ExtendRegister();
		activePositions[activePosNum++] = pos;
	}
}

void Display::_ExtendRegister() {
	Cursor* tmp = new Cursor[activePosNum + 1];
	for (int i = 0; i < activePosNum; i++)
		tmp[i] = activePositions[i];

	delete[]activePositions;
	activePositions = tmp;
}

void Display::_ClearContent() {
	for (int i = 0; i < activePosNum; i++)
		activePositions[i]._ClearText();
	delete[]activePositions;
	activePositions = nullptr;
	activePosNum = 0;
}

void Display::_LockContent(Cursor &pos) {
	for (int i = 0; i < activePosNum; i++)
		if (activePositions[i] == pos) {
			utility::_RemoveElement(activePositions, i, activePosNum);
			activePosNum--;
		}	
}

void Display::_DrawLayout_default() {
	Separator menuLine(20, 1, 20, 1);
	Separator headerLine(::width - 4, 0, 2, 2);

	Frame mainWindow;

	/*Cursor mainMenuPos(6, 1);
	const char* mainMenu = "Main Menu ";*/

	_DisplaySeparator(menuLine);
	_DisplaySeparator(headerLine);
	//_DisplayContent(mainMenu, mainMenuPos);
	//_DisplayCharacter(::headerSymbol);		

	
	//topHeader._SetX(1, ::width-1);
	//topHeader._SetY(1, headerLine.y1);

	//short area = topHeader._GetArea();

	utility::_AddElement(frameSeparators, menuLine, sepNum);
	utility::_AddElement(frameSeparators, headerLine, sepNum);

	//Display mainWindow;
	char* currentDate = utility::_GetCurrentDate();
	Cursor datePos(width - utility::_CharLength(currentDate) - 2, 1);
	//mainWindow._DisplayContent(currentDate, datePos);
}