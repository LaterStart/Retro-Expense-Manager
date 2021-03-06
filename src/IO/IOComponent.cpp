#include <iostream>
#include <sstream>
#include <Windows.h>
#include "IOComponent.h"
#include "OComponent.h"
#include "../Models/_Model.h"
#include "../config.h"
using namespace std;

int IOComponent::idCounter = 0;

Console::Console() : mainFrame(nullptr) {
	this->componentType = ComponentType::console;
}

Console::~Console() {
	delete mainFrame;
}

//	singleton implementation
Console& Console::_GetInstance() {
	static Console myConsole;
	return myConsole;
}

//	Initialize windows console
void Console::_Initialize() {
	SetConsoleTitle(title);
	HANDLE hOut;
	COORD NewSBSize;

	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	NewSBSize.X = width;
	NewSBSize.Y = height;

	SetConsoleScreenBufferSize(hOut, NewSBSize);
	char* systemMode = _SystemMode();
	system(systemMode);
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)NULL, TRUE);
	delete[]systemMode;
}


//	Method which creates char* type system mode parameter to set console system mode using preconfigured width and height
char* Console::_SystemMode() {
	char* widthChar = utility::_IntToChar(width);
	char* heightChar = utility::_IntToChar(height);
	const char* mode = "MODE ";
	char ch = ',';

	char* result = utility::_ConcatenateChar(mode, widthChar, 0);
	int z = utility::_CharSize(result);
	char* tmp = new char[z + 1];
	int y = 0;
	while (y < z - 1) tmp[y] = result[y++];
	tmp[y] = ch; tmp[z] = '\0';
	delete[]result;

	result = utility::_ConcatenateChar(tmp, heightChar, 0);
	delete[]tmp;
	delete[]widthChar;
	delete[]heightChar;
	return result;
}

//	Draw console frame
void Console::_DrawFrame() {
	Cursor posA(0, 0), posB(width - 1, 0);
	//draw vertical lines
	for (int i = 0; i < height - 2; i++) {
		posA._MoveY(1);
		cout << verticalLine;
		posB._MoveY(1);
		cout << verticalLine;
	}
	posA._SetXY(0, 0);
	posB._SetXY(0, height - 1);

	//draw horizontal lines
	for (int i = 0; i < width - 2; i++) {
		posA._MoveX(1);
		cout << horizontalLine;
		posB._MoveX(1);
		cout << horizontalLine;
	}
	this->mainFrame = new Frame(::width, 0, ::height, 0);
	this->mainFrame->IDname = "MainFrame";
}

Cursor::Cursor() : x(0), y(0), n(0) {
	_GetCursorPosition();
	this->componentType = ComponentType::cursor;
}
Cursor::Cursor(const Cursor &copy) : x(copy.x), y(copy.y), n(copy.n) {
	this->componentType = ComponentType::cursor;
}

Cursor::Cursor(short x, short y) : x(x), y(y) {
	_SetCursorPosition();
	this->componentType = ComponentType::cursor;
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

Display::~Display() {
	for (int i = 0; i < activePosNum; i++)
		activePositions[i]._ClearContent();
	delete[]activePositions;
}

void Display::_HideContent(int elementID) {
	for (int i = 0; i < activePosNum; i++) {
		if (activePositions[i].elementID == elementID) {
			activePositions[i]._ClearContent();
			utility::_RemoveElement(activePositions, i, activePosNum);
			i--;
		}
	}
}

void Display::_RemoveID(int elementID) {
	for (int i = 0; i < activePosNum; i++) {
		if (activePositions[i].elementID == elementID) {
			utility::_RemoveElement(activePositions, i, activePosNum);
			i--;
		}
	}
}

void Display::_Backspace() {
	Cursor tmp;
	tmp._ChangeX(-1);
	_Display(tmp, " ");
	tmp._SetCursorPosition();
}

void Display::_Show(const char* content) {
	if(content!=nullptr)
		cout << content;
}

void Display::_Show(const char* content, unsigned char symbol) {
	cout << content;
	cout << symbol;
}

void Display::_Display(char ch) {
	Cursor pos;
	cout << ch;
	if (ch == ::spaceKey)
		return;
	pos._SetCharacterNumber(1);
	ActivePos apos(pos, -1);
	_AddActivePosition(apos);
}

void Display::_Display(const char* text) {
	Cursor pos;
	cout << text;
	pos._SetCharacterNumber(utility::_CharLength(text));
	ActivePos apos(pos, -1);
	_AddActivePosition(apos);
}

void Display::_Display(Label* label, Cursor& pos) {
	pos._SetCursorPosition();
	if (!label->symbolPos) {
		if (label->symbol != 0)
			cout << label->symbol;		
	}

	if (label->cut > 0) {
		for (int i = 0; i < (label->length - label->cut - 2); i++)
			cout << label->text[i];
		for (int i = 0; i < 2; i++)
			cout << '.';
	}
	else cout << label->text;

	pos._SetCharacterNumber((label->length - label->cut));
	if(!label->symbolPos)
		pos._ChangeCharacterNumber(1);

	if (label->symbolPos) {
		if (label->symbol != 0) {
			cout << label->symbol;
			pos._ChangeCharacterNumber(1);
		}
	}

	ActivePos apos(pos, label->id);
	if (label->_Original() != nullptr) {
		ActivePos apos(pos, label->_Original()->id);
		_AddActivePosition(apos);
	}
	_AddActivePosition(apos);
}

void Display::_Display(MenuItem* item, Cursor& pos) {
	pos._SetCursorPosition();
	if (item->orderNum != nullptr) {
		cout << item->orderNum;
		pos._SetCharacterNumber(utility::_CharLength(item->orderNum));
	}		
	_Display((Label*)item);	
	
	if (item->_Original() != nullptr) {
		ActivePos apos(pos, item->_Original()->id);
		_AddActivePosition(apos);
	}
	ActivePos apos(pos, item->id);
	_AddActivePosition(apos);
}

void Display::_Display(Separator& separator) {
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
			ActivePos apos(pos, separator.id);
			_AddActivePosition(apos);
		}
		(pos.*(pos.ptr))(1);
	}
	if (separator.direction == 0) {
		Cursor pos(separator.x1, separator.y1);
		pos._SetCharacterNumber(separator.length);
		ActivePos apos(pos, separator.id);
		_AddActivePosition(apos);
	}
}

void Display::_Display(Cursor& pos, int num) {
	pos._SetCursorPosition();
	pos._SetCharacterNumber(utility::_DigitNumberInt(num));
	cout << num;
	ActivePos apos(pos, -1);
	_AddActivePosition(apos);
}

void Display::_Display(Model& model, Cursor& pos) {
	int cut = 0;
	if (parentFrame != nullptr) {
		Frame::Coordinates coord = parentFrame->_GetCoordinates();
		int width = coord.x2 - pos._GetX();
		cut = model._DisplayLength() - width;
		cut = (cut < 0) ? 0 : cut;	
	}
	if (cut > 0) {
		std::ostringstream oss;
		oss << model;
		string str = oss.str();
		Label lbl(str.c_str());
		lbl.cut = cut;
		_Display(&lbl, pos);		
	}
	else {
		pos._SetCursorPosition();
		cout << model;
	}
	pos._SetCharacterNumber(model._DisplayLength()-cut);
	ActivePos apos(pos, -1);
	_AddActivePosition(apos);
}

void Display::_Loading() {
	Cursor loadng;
	const char* load = "...";
	int j = 0;
	for (int i = 0; i < 300000000; i++) {
		if (i % 100000000 == 0) {
			cout << load[j++];
		}
	}
	loadng._SetCharacterNumber(utility::_CharLength(load));
	ActivePos apos(loadng, -1);
	_AddActivePosition(apos);
}

void Display::_AddActivePosition(ActivePos apos) {
	bool newPos = true;
	if (activePositions != nullptr) 
		for (int i = 0; i < activePosNum; i++) 
			if (activePositions[i] == apos) {
				newPos = false;
				break;
			}	
	if (newPos) {
		_ExtendRegister();
		activePositions[activePosNum++] = apos;
	}
}

void Display::_ExtendRegister() {
	ActivePos* tmp = new ActivePos[activePosNum + 1];
	for (int i = 0; i < activePosNum; i++)
		tmp[i] = activePositions[i];

	delete[]activePositions;
	activePositions = tmp;
}

void Display::_WipeContent() {
	for (int i = 0; i < activePosNum; i++)
		activePositions[i]._ClearContent();
	delete[]activePositions;
	activePositions = nullptr;
	activePosNum = 0;
}

void Display::_LockContent(Cursor &pos) {
	for (int i = 0; i < activePosNum; i++)
		if (activePositions[i].pos == pos) 
			utility::_RemoveElement(activePositions, i, activePosNum);
}

void Display::_LockContent() {
	delete[]activePositions;
	activePositions = nullptr;
	activePosNum = 0;
}