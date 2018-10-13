#include "Modules/ModuleManagement.h"
#include "IO/IOComponent.h"
#include "config.h"
#pragma comment(lib, "winhttp")

int main() {
	//	Initialize program
	Console* myConsole = Initialize::_Console();	

	//	Display version label
	Display main;
	Cursor versionPos(width - utility::_CharLength(::version) - 2, height - 2);
	main._Display(versionPos, ::version);	

	//	Initialize modules
	ModuleManagement moduler(myConsole);  // -> main module management instance
	
	moduler._OpenModule("Login");		
	
	//	Game loop
	while (true) 
		moduler._OpenNextModule();

	delete[]::currentDate;
	return 0;
}

// 10350 total lines of code (13.10.2018.)