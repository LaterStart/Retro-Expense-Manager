#include "Login.h"
#include "../Controllers/ProfileController.h"
#include "../Models/Profile.h"
#include "../IO/IOComponent.h"
#include "../IO/Input.h"

Module& Login::_GetInstance() {
	return _LoadModule();
}

Login& Login::_LoadModule() {
	static Login module;
	return module;
}

void Login::_StartModule() {
	//	Check for last used user profile in database binary file
	Profile* profile = controller._GetLastUsedProfile();

	//	if profile contoller has found last used user and is not password protected
	if (profile != nullptr && !profile->_PwStatus())		
		moduler->_SetNextModule("Dashboard");
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
			MenuItem("Change User", "LoadUserProfile")
		);
		mainMenu._SetPadding(1);
		layout._Select("Menu")->_AddElements(mainMenu);
		Frame* content = layout._Select("Content");

		//	No user profile detected
		if (profile == nullptr) {
			Label text1("No recent user profile was detected.");
			Label text2("Please create new one or load existing.");
			text1._SetPadding(4); text2._SetPadding(4);
			content->_AddElements(text1, text2);
			layout._ShowElements();

			//	Read user input - menu selection only available 
			Cursor(1, ::height - 2);
			UserInput select(InputType::menuSelect);
			while (select.selection <  1 || select.selection > mainMenu.size) {
				select._ReadUserInput();
				select._ClearInput();
			}			
			moduler->_SetNextModule(mainMenu._GetLink(select.selection), this);
		}
		//	Profile password protected
		else if (profile->_PwStatus()) {
			TextBar bar(
				Label("Welcome"),
				Label(profile->_Username())
			);
			bar._SetPadding(4);
			content->_AddElements(bar);
			layout._ShowElements();

			//	Read user input - password field 
			InputField pw("Please enter password:", InputType::password);
			pw._SetPadding(4);
			pw._SetYpos(++content->nextYpos);
			content->_AddElements(pw);

			Label wrongPw("Incorrect password.");
			wrongPw._SetParentFrame(content);
			wrongPw._SetYpos(content->nextYpos + 2);
			wrongPw._SetPadding(4);

			do {
				pw._Show();
				if (profile->_VerifyPassword(pw.inputField->input))
					break;
				else {
					pw.inputField->_ClearInput();
					wrongPw._Show();
				}
			} while (true);

			moduler->_SetNextModule("Dashboard");
		}	
		//	Set module name (link) as the next one to be opened in main.cpp game loop.
		//	provide this module pointer as previoous module to enable ESC key in next module (get back to this module) option
	}
}