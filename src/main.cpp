#include "Modules/ModuleManagement.h"
#include "View/IOComponent.h"
#include "config.h"

int main() {
	//	Initialize program
	Console& myConsole = myConsole._GetInstance();
	myConsole._Initialize();
	myConsole._DrawFrame();

	//	Display current date and version label
	Display mainWindow; 
	char* currentDate = utility::_GetCurrentDate();
	Cursor datePos(width - utility::_CharLength(currentDate) - 2, 1);
	mainWindow._DisplayContent(currentDate, datePos);

	Cursor versionPos(width - utility::_CharLength(version) - 2, height - 2);
	mainWindow._DisplayContent(version, versionPos);
	mainWindow._LockContent(versionPos); // -> makes label text on given position permanent

	ModuleManagement moduler;  // -> main module management instance
	moduler._OpenModule("Login");	

	return 0;
}