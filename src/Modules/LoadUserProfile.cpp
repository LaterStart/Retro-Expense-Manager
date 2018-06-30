#include "LoadUserProfile.h"
#include "../Controllers/ProfileController.h"
#include "../Models/Profile.h"
#include "../IO/IOComponent.h"
#include "../IO/Input.h"

Module& LoadUserProfile::_GetInstance() {
	return _LoadModule();
}

LoadUserProfile& LoadUserProfile::_LoadModule() {
	static LoadUserProfile module;
	return module;
}

void LoadUserProfile::_StartModule() {
	//	Create default frame layout
	Frame* mainFrame = console->_GetMainFrame();
	Layout layout(mainFrame);
	Display display;

	layout._DefaultFrameTemplate(display);
	layout._Select("MenuHeader")->_AddElements(Label("Main Menu ", ::headerSymbol, "center"));
	layout._Select("Date")->_AddElements(Label(utility::_GetCurrentDate(), "left"));

	Label title("Change User ", ::headerSymbol, "left");
	layout._Select("SelectionTitle")->_AddElements(title);
	Frame* content = layout._Select("Content");

	//	Main menu
	Menu mainMenu;
	mainMenu._AddItems(
		MenuItem("Create Profile", "CreateUserProfile"),
		MenuItem("Change User", this->name)
	);
	layout._Select("Menu")->_AddElements(mainMenu);

	//	Control menu
	Menu controlMenu;
	MenuItem F1("Menu", this);
	MenuItem ESC("Cancel", previousModule);
	F1._SetSpecialPrefix("[F1] ");
	ESC._SetSpecialPrefix("[ESC] ");
	ESC._SetPadding(F1._Length() + 2);
	ESC._SetYpos(F1._Ypos());
	controlMenu._AddItems(F1, ESC);
	layout._Select("Footer")->_AddElements(controlMenu);
	layout._ShowElements();

	//	Labels & messages
	Label info("Please login:");
	Label wrongUs("User not found.");
	Label wrongPw("Incorrect password.");
	Label success("Success");
	Label locked("Please wait: ");
	wrongUs._SetYpos(4);
	wrongPw._SetYpos(5);
	success._SetYpos(2);
	locked._SetYpos(5);

	//	Input fields
	InputField username("Username:", InputType::text);
	InputField password("Password:", InputType::password);
	username._SetYpos(2);
	password._SetYpos(3);

	content->_AddElements(info, wrongUs, wrongPw, success, username, password, locked);
	info._Show();

	//	Loop
	Profile* profile = nullptr;
	username._Show();
	int controlKey = false;
	bool showMenu = false;
	bool cancel = false;
	bool OK = false;
	int loginTries = 5;
	do {
		controlKey = (controlKey == 0) ? username.inputField->control : controlKey;
		// verify if control key is pressed		
		if (controlKey) {	
			int switcher = controlKey;
			username.inputField->control = 0;
			controlKey = 0;
			switch (switcher) {
			case -1:
				// ESC
				cancel = true;
				break;
			case 3:
				// F1
				showMenu = true;
				break;
			default:
				username._Show();
				continue;
			}
		}
		// load profile from database for first time or user when has changed username during input
		else if (profile == nullptr || !utility::_CompareChar(profile->_Username(), username.inputField->input)) {
			delete profile;
			profile = profileController._GetProfile(username.inputField->input);			
		}
		// control key is pressed - F1 
		if (showMenu) {
			showMenu = false;
			Cursor(2, ::height - 4);
			UserInput select(InputType::select);
			int selection = 0;
			const char* nextModule = nullptr;
			while (selection <  1 || selection > mainMenu.size) {
				select._ReadUserInput();
				selection = select.selection;
				if (select.control == -1)
					//esc
					int test = 0;
				select._ClearInput();
			}
			nextModule = mainMenu._GetLink(selection);
			// get back to login
			if (utility::_CompareChar(nextModule, this->name))
				continue;
			else {
				// switch module
				moduler->_SetNextModule(nextModule, this);
				break;
			}
		}
		// control key is pressed - ESC
		else if (cancel) {
			moduler->_SetNextModule(previousModule);
			break;
		}
		// verify username
		else if (profile != nullptr) {
			wrongUs._Hide();
			// check if is password protected
			if (profile->_PwStatus()) {
				password._Show();
				// verify if control key is pressed
				if (password.inputField->_ControlKey()) {
					controlKey = password.inputField->control;
					password._Hide();
					wrongPw._Hide();
					continue;
				}					
				// verify password
				else if (!profile->_VerifyPassword(password.inputField->input)) {
					// wrong password
					loginTries--;
					password.inputField->_ClearInput();
					wrongPw._Show();
					if (loginTries == 0) {
						password._Hide();
						wrongPw._Hide();
						wrongPw._SetYpos(4);
						wrongPw._Show();
						locked._Show();
						utility::_Countdown(10);
						loginTries = 5;
						locked._Hide();
						wrongPw._Hide();
						wrongPw._SetYpos(5);
					}
				}
				// correct password
				else {
					OK = true;
					break;
				}
			}
			// not password protected
			else {
				OK = true;
				break;
			}
		}
		// wrong username
		else {
			wrongUs._Show();
			username._Show();
		}
	} while (true);

	if (OK) {
		username._Hide();
		password._Hide();
		wrongUs._Hide();
		wrongPw._Hide();
		success._Show();
		profile->_Unlock();
		this->profileController._SwitchProfile(profile);
		display._Loading();
		moduler->_SetNextModule(previousModule);
	}
}