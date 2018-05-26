#include <iostream>
#include "Display.h"
#include "Cursor.h"
#include "../utility.h"
using namespace std;

Display::Display() : activePositions(nullptr), activePosNum(0){}

Display::~Display(){
	delete[]activePositions;
}

void Display::_DisplayContent(const char* test, Cursor &pos) {
	pos._SetCursorPosition();
	cout << test;
	pos._SetCharacterNumber(utility::_CharLength(test));
	_AddActivePosition(pos);
}

void Display::_DisplayContent(const char* test) {
	Cursor pos;
	cout << test;
	pos._SetCharacterNumber((short)utility::_CharLength(test));
	_AddActivePosition(pos);
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