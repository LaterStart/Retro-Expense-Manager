#include <iostream> //debug
#include "config.h"
#include "utility.h"
#include "Controllers/ProfileController.h"
#include "Modules/ModuleManagement.h"
#include "Modules/Module.h"
#include "Modules/CreateUserProfile.h"
#include "View/Console.h"
#include "View/Cursor.h"
#include "View/Display.h"
#include "View/Menu.h" //debug

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
	
	//	Check for last used user profile in preconfigured dataPath binary file
	ProfileController profCtrl;
	profCtrl._GetLastUsedProfile(dataPath);
	
	ModuleManagement moduler;	
	if (profCtrl._GetStatus() == true) {

	}
	

	



	return 0;
}