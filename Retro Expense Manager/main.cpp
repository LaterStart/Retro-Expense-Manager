#include <iostream> //debug
#include "Console.h"
#include "Display.h"
#include "Cursor.h"
#include "config.h"
#include "utility.h"
using namespace std; //debug

int main() {
	//	Initialize program
	Console& myConsole = myConsole._GetInstance();
	myConsole._Initialize();
	myConsole._DrawFrame();

	//	Display current date and version label
	Display mainWindow; // -> display module (tracks current active content positions)
	char* currentDate = utility::_GetCurrentDate();
	Cursor datePos(width - utility::_CharLength(currentDate) - 2, 1);
	mainWindow._DisplayContent(currentDate, datePos);
	Cursor versionPos(width - utility::_CharLength(version) - 2, height - 2);
	mainWindow._DisplayContent(version, versionPos);
	mainWindow._LockContent(versionPos); // -> makes label text on given position permanent
	
	




	return 0;
}