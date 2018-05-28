#include <iostream>
#include <Windows.h>
#include "../config.h"
#include "IOComponent.h"
#include "OComponent.h"
using namespace std;

bool Initialize::initialized = false;
Console* Initialize::_Console() {
	if (!initialized) {
		Console& ref = Console::_GetInstance();
		Console* myConsole = &ref;
		myConsole->_Initialize();
		myConsole->_DrawFrame();

		initialized = true;
		return myConsole;
	}
	return nullptr;
}

Console::Console() : mainFrame(nullptr) {}

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
	char* widthChar = utility::_ConvertToChar(width);
	char* heightChar = utility::_ConvertToChar(height);
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
}
Cursor::Cursor(const Cursor &copy) : x(copy.x), y(copy.y), n(copy.n) {}

Cursor::Cursor(short x, short y) : x(x), y(y) {
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

Display::Display() : activePositions(nullptr), frames(nullptr), activePosNum(0), frameNum(0) {}

Display::~Display() {
	delete[]activePositions;
	delete[]frames;
}

void Display::_Display(unsigned char ch) {
	Cursor pos;
	cout << ch;
	pos._SetCharacterNumber(1);
	_AddActivePosition(pos);
}

void Display::_Display(const char* content, Cursor &pos) {
	pos._SetCursorPosition();
	cout << content;
	pos._SetCharacterNumber(utility::_CharLength(content));
	_AddActivePosition(pos);
}

void Display::_Display(const char* content) {
	Cursor pos;
	cout << content;
	pos._SetCharacterNumber((short)utility::_CharLength(content));
	_AddActivePosition(pos);
}

void Display::_Display(const char* content, unsigned int cut) {
	Cursor pos;
	int length = utility::_CharLength(content) - cut;
	for (int i = 0; i < length; i++)
		cout << content[i];

	pos._SetCharacterNumber(length);
	_AddActivePosition(pos);
}

void Display::_Display(Label& label, Cursor& pos) {
	pos._SetCursorPosition();
	cout << label.text;

	pos._SetCharacterNumber(label.length);
	_AddActivePosition(pos);
}

void Display::_Display(Label& label) {
	Cursor pos;
	cout << label.text;

	pos._SetCharacterNumber(label.length);
	_AddActivePosition(pos);
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
	Frame frame = parentFrame->_CreateSubFrame("ModuleMain");
	Separator menuLine(frame, 20, 1, 20, 0);
	Separator headerLine(frame, ::width-4, 0, 2, 2);
	frame._Split(headerLine, "ModuleHeader", "ModuleBody");
	frame._Select("ModuleHeader")->_Split(menuLine, "MenuTitle", "BodyTitle");
	frame._Select("ModuleBody")->_Split(menuLine, "MenuFrame", "ModuleFrame");
	
	frame._Select("MenuTitle")->_AddElement(Label("Main Menu"));


	Frame::Coordinates coord = frame._Select("MenuFrame")->_GetCoordinates();
	/*Cursor mainMenuPos(6, 1);
	const char* mainMenu = "Main Menu ";*/

	//Frame::Coordinates coord = header._GetCoordinates();

	Cursor test3;
	for (int i = coord.x1; i < coord.x2; i++)
		for (int j = coord.y1; j < coord.y2; j++) {
			test3._MoveToXY(i, j);
			cout << 'c';
		}
		
	int test = 0;

	_Display(menuLine);
	_Display(headerLine);
	//_DisplayContent(mainMenu, mainMenuPos);
	//_DisplayCharacter(::headerSymbol);		


	//topHeader._SetX(1, ::width-1);
	//topHeader._SetY(1, headerLine.y1);

	//short area = topHeader._GetArea();

	//Display mainWindow;
	//char* currentDate = utility::_GetCurrentDate();
	//Cursor datePos(width - utility::_CharLength(currentDate) - 2, 1);
	//mainWindow._DisplayContent(currentDate, datePos);
}