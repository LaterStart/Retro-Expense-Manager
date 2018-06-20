#include "Login.h"
#include "../Controllers/ProfileController.h"
#include "../IO/IOComponent.h"
#include "../IO/Input.h"
#include "../config.h"

Module& Login::_GetInstance() {
	return _LoadModule();
}

Login& Login::_LoadModule() {
	static Login module;
	return module;
}

void Login::_StartModule() {
	//	Check for last used user profile in database binary file

	//Profile* profile = profCtrl._GetLastUsedProfile();
	Profile* profile = nullptr;
	//	if profile contoller has found last used user
	if (profile != nullptr) {

	}
	//	if no recent user profile is found, display available options
	else {
		//	Create default frame layout
		Frame* mainFrame = console->_GetMainFrame();	
		Layout layout(mainFrame);
		Display display;		

		layout._DefaultFrameTemplate(display);
		layout._Select("MenuHeader")->_AddElements(Label("Main Menu ", ::headerSymbol, "center"));		
		layout._Select("Date")->_AddElements(Label(utility::_GetCurrentDate(), "left"));

		Label userProfile("User Profile ", ::headerSymbol, "left");
		userProfile._SetPadding(4);
		layout._Select("SelectionTitle")->_AddElements(userProfile);

		//	Main menu
		Menu mainMenu;
		mainMenu._AddItems(
			MenuItem("Create Profile", "CreateUserProfile"),
			MenuItem("Load Profile", "LoadUserProfile")
		);
		mainMenu._SetPadding(1);
		layout._Select("Menu")->_AddElements(mainMenu);

		//	Module text
		Label text1("No recent user profile was detected.");
		Label text2("Please create new one or load existing.");
		text1._SetPadding(4); text2._SetPadding(4);
		layout._Select("Content")->_AddElements(text1, text2);

		layout._ShowElements();		

		//	Read user input - menu selection only available 
		Cursor(1, ::height - 2);
		UserInput select(InputType::menuSelect);
		while (select.selection <  1 || select.selection > mainMenu.size) {			
			select._ReadUserInput();
			select._ClearInput();
		}

		//	Set module name (link) as the next one to be opened in main.cpp game loop.
		//	provide this module pointer as previoous module to enable ESC key in next module (get back to this module) option
		moduler->_SetNextModule(this, mainMenu._GetLink(select.selection));
	}
}