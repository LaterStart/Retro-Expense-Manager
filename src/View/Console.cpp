#include <iostream>
#include <Windows.h>
#include "Console.h"
#include "Cursor.h"
#include "../utility.h"
#include "../config.h"
using namespace std;

Console::Console(){}

Console::~Console(){}

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
	Cursor posA(0,0), posB(25,0), posC(width-1,0);

	//draw vertical lines
	for (int i = 0; i < height - 2; i++) {
		posA._MoveY(1);
		cout << verticalLine;
		if (i < height - 3) {
			posB._MoveY(1);
			cout << verticalLine;
		}
		posC._MoveY(1);
		cout << verticalLine;
	}

	posA._SetXY(0, 0);
	posB._SetXY(1, 2);
	posC._SetXY(0, height - 1);

	//draw horizontal lines
	for (int i = 0; i < width - 2; i++) {
		posA._MoveX(1);
		cout << horizontalLine;
		if (i < width - 4) {
			posB._MoveX(1);
			cout << horizontalLine;
		}
		posC._MoveX(1);
		cout << horizontalLine;
	}
}