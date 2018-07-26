#include "Modules/ModuleManagement.h"
#include "IO/IOComponent.h"
#include "config.h"

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

// 6147 lines - 01.07.2018.
// 9873 lines - 25.07.2018.