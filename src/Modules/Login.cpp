#include "Login.h"
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
	Profile* profile;
	//	Check for last used user profile in database binary file
	if(profileController._ActiveProfile() == nullptr)
		profile = profileController._GetLastUsedProfile();
	else profile = profileController._ActiveProfile();

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

		Label title("User Profile ", ::headerSymbol, "left");
		layout._Select("SelectionTitle")->_AddElements(title);

		//	Main menu
		Menu mainMenu;
		mainMenu._AddItems(
			MenuItem("Create Profile", "CreateUserProfile"),
			MenuItem("Load Database", "LoadDatabase")
		);
		layout._Select("Menu")->_AddElements(mainMenu);

		//	Control menu
		Menu controlMenu;
		MenuItem F1("Menu");
		MenuItem F2("Enter password");
		F1._SetSpecialPrefix("[F1] ");
		F2._SetSpecialPrefix("[F2] ");
		F2._SetPadding(F1._Length() + 2);
		F2._SetYpos(F1._Ypos());
		controlMenu._AddItems(F1, F2);		

		Frame* content = layout._Select("Content");

		//	No user profile detected
		if (profile == nullptr) {
			//	Download exchange rate data for currency list
			exchangeRateController._DownloadExchangeRate();

			Label text1("No recent user profile was detected.");
			Label text2("Please create new one or load existing.");			
			content->_AddElements(text1, text2);
			layout._ShowElements();	

			//	Read user input - menu selection only available 
			Cursor(2, ::height - 4);
			UserInput select(InputType::select);
			int selection = 0;
			while (selection <  1 || select.selection > mainMenu.size) {
				select._ReadUserInput();
				selection = select.selection;
				select._ClearInput();
			}			
			moduler->_SetNextModule(mainMenu._GetLink(selection), this);			
		}
		//	Profile password protected
		else if (profile->_PwStatus() && profile->_Locked()) {
			TextBar bar(
				Label("Welcome"),
				Label(profile->_Username())
			);			
			content->_AddElements(bar);
			layout._Select("Footer")->_AddElements(controlMenu);
			mainMenu._ChangeItem("Load Database", "Change User", "LoadUserProfile");
			layout._ShowElements();

			//	Read user input - password field 
			InputField password("Please enter password:", InputType::password);
			password._SetYpos(2);			
			
			Label wrongPw("Incorrect password.");
			Label success("Success");
			Label locked("Please wait: ");
			wrongPw._SetYpos(4);
			success._SetYpos(2);
			locked._SetYpos(3);
			content->_AddElements(password, wrongPw, success, locked);
			int loginTries = 5;
			do {
			passwordField:
				F2._Hide();
				password._Show();
				if (password.inputField->control == ControlKey::F1) {
					// Menu selection
					F2._Show();
					wrongPw._Hide();
					Cursor(2, ::height - 4);
					UserInput select(InputType::select);
					int selection = 0;
					const char* nextModule = nullptr;
					while (selection <  1 || selection > mainMenu.size) {
						select._ReadUserInput();
						selection = select.selection;
						if (select.control == ControlKey::F2)
							goto passwordField;
						select._ClearInput();
					}
					nextModule = mainMenu._GetLink(selection);
					moduler->_SetNextModule(nextModule, this);
					break;
				}
				else if (password.inputField->_ControlKey())
					continue;
				else if (profile->_VerifyPassword(password.inputField->input)) {
					wrongPw._Hide();
					password._Hide();
					success._Show();
					display._Loading();
					moduler->_SetNextModule("Dashboard");
					break;
				}
				else {
					loginTries--;
					password.inputField->_ClearInput();
					wrongPw._Show();
					if (loginTries == 0) {
						password._Hide();
						wrongPw._Hide();
						wrongPw._SetYpos(2);
						wrongPw._Show();
						locked._Show();
						utility::_Countdown(10);
						loginTries = 5;						
						locked._Hide();
						wrongPw._Hide();						
						wrongPw._SetYpos(4);
					}
				}
			} while (true);
		}	
		// Profile is already loeaded
		else moduler->_SetNextModule("Dashboard");		
	}

	// If profile is loaded - add default currency at top of the currencies list
	if (profile != nullptr) {
		std::vector<Currency>* ccyList = exchangeRateController.currencies;
		Currency* ccy = exchangeRateController._GetCurrency(profile->_DefaultCurrency());
		ccyList->insert(ccyList->begin(), Currency(*ccy));
	}

	//	Set module name (link) as the next one to be opened in main.cpp game loop.
	//	provide this module pointer as previoous module to enable ESC key in next module (get back to this module) option
}