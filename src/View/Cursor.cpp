#include <iostream>
#include <Windows.h>
#include "Cursor.h"
#include "../config.h"
using namespace std;

Cursor::Cursor() : x(0), y(0), n(0) {
	_GetCursorPosition();
}
Cursor::Cursor(const Cursor &copy) : x(copy.x), y(copy.y), n(copy.n) {}

Cursor::Cursor(short x, short y) : x(x), y(y){
	_SetCursorPosition();
}

Cursor::~Cursor() {}

void Cursor::_GetCursorPosition() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
		x = csbi.dwCursorPosition.X;
		y = csbi.dwCursorPosition.Y;
	}
}

void Cursor::_SetCursorPosition() {
	COORD coord = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

bool Cursor::operator==(const Cursor &second) const {
	if (x == second.x && y == second.y && n == second.n)
		return true;
	else return false;
}

void Cursor::_MoveX(short x_diff) {
	x += x_diff;
	_SetCursorPosition();
}

void Cursor::_MoveY(short y_diff) {
	y += y_diff;
	_SetCursorPosition();
}

void Cursor::_ClearText() {
	_SetCursorPosition();
	for (int i = 0; i < n; i++) {
		cout << spaceKey;
	}
	_SetCursorPosition();
}

void Cursor::_MoveToXY(short x_in, short y_in) {
	x = x_in;
	y = y_in;
	_SetCursorPosition();
}