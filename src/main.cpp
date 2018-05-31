#include "Modules/ModuleManagement.h"
#include "View/IOComponent.h"
#include "config.h"

int main() {
	//	Initialize program
	Console* myConsole = Initialize::_Console();

	//	Display version label
	Display main;
	Cursor versionPos(width - utility::_CharLength(::version) - 2, height - 2);
	main._Display(::version, versionPos);	

	//	Initialize modules
	ModuleManagement moduler(myConsole);  // -> main module management instance

	moduler._OpenModule("Login");

	delete[]::currentDate;
	return 0;
}