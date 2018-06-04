#include "Login.h"
#include "../Controllers/ProfileController.h"
#include "../IO/IOComponent.h"
#include "../config.h"

Login::Login() : initialized_(false) {}

Login::~Login() = default;

Module& Login::_GetInstance() {
	return _LoadModule();
}

Login& Login::_LoadModule() {
	static Login module;
	return module;
}

void Login::_StartModule() {
	//	Check for last used user profile in preconfigured "dataPath" binary file
	ProfileController profCtrl;
	profCtrl._GetLastUsedProfile(::dataPath);

	//	if profile contoller has found last used user
	if (profCtrl._GetStatus()) {

	}
	else {
		//	if no recent user profile is found, display available options
		Frame* mainFrame = console->_GetMainFrame();	
		Layout layout(mainFrame);
		Display display;		

		layout._DefaultFrameTemplate(display);
		layout._Select("MenuHeader")->_AddElements(Label("Main Menu ", ::headerSymbol, "center"));		
		layout._Select("Date")->_AddElements(Label(utility::_GetCurrentDate(), "left"));

		Label userProfile("User Profile ", ::headerSymbol, "left");
		userProfile._SetPadding(4);
		layout._Select("SelectionTitle")->_AddElements(userProfile);

		Menu mainMenu;
		mainMenu._AddItems(
			MenuItem("Create Profile", "CreateUserProfile"),
			MenuItem("Load Profile", "LoadUserProfile")
		);
		mainMenu._SetPadding(1);
		layout._Select("Menu")->_AddElements(mainMenu);

		Label text1("No recent user profile was detected.");
		Label text2("Please create new one or load existing.");
		text1._SetPadding(4); text2._SetPadding(4);
		layout._Select("Content")->_AddElements(text1, text2);

		layout._ShowElements();		

		Cursor(1, ::height - 2);
		UserInput select(menuSelect);
		while (select.selection <  1 || select.selection > mainMenu.size) {
			select._ClearInput();
			select._ReadUserInput();
		}
		moduler->_OpenModule(mainMenu._OpenLink(select.selection));
	}
}