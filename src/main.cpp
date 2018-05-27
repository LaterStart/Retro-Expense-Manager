#include "Modules/ModuleManagement.h"
#include "View/IOComponent.h"
#include "config.h"

int main() {
	//	Initialize program
	Console* myConsole = Initialize::_Console();

	//	Display version label
	Display version;
	Cursor versionPos(width - utility::_CharLength(::version) - 2, height - 2);
	version._DisplayContent(::version, versionPos);
	version._LockContent(versionPos); // -> makes label text on given position permanent

	//	Initialize modules
	ModuleManagement moduler(myConsole);  // -> main module management instance
	moduler._OpenModule("Login");	

	while (true) {};
	return 0;
}