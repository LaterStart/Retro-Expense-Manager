#include "CreateUserProfile.h"
#include "../IO/IOComponent.h"
#include "../IO/Input.h"
#include "../config.h"

Module& CreateUserProfile::_GetInstance() {
	return _LoadModule();
}

CreateUserProfile& CreateUserProfile::_LoadModule() {
	static CreateUserProfile module;	
	return module;
}

void CreateUserProfile::_StartModule() {
	//	Create default frame layout
	Frame* mainFrame = console->_GetMainFrame();
	Layout layout(mainFrame);
	Display display;

	layout._DefaultFrameTemplate(display);
	layout._Select("MenuHeader")->_AddElements(Label("Main Menu ", ::headerSymbol, "center"));
	layout._Select("Date")->_AddElements(Label(utility::_GetCurrentDate(), "left"));

	Label title("Create Profile ", ::headerSymbol, "left");
	layout._Select("SelectionTitle")->_AddElements(title);	

	Label info("Please fill out the form:");

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
	MenuItem ESC("Cancel", previousModule);
	F1._SetSpecialPrefix("[F1] ");
	ESC._SetSpecialPrefix("[ESC] ");
	ESC._SetPadding(F1._Length() + 2);
	ESC._SetYpos(0);
	controlMenu._AddItems(F1, ESC);
	layout._Select("Footer")->_AddElements(controlMenu);

	Frame* content = layout._Select("Content");
	content->_AddElements(info);
	layout._ShowElements();	

	//	Input form
	Form form;
	form._SetParentFrame(content);
	form._AddFields(
		UsernameField("Username:", this->profileController),
		OptionField("Password protected? Y/N:", Field::pwStatus,
			// add optional fields 
			// true marks as key password field
			PasswordField("Password:", true),	
			PasswordField("Repeat password:")),
		ScrollDown<Currency>("Default currency:", *exchangeRateController.currencies, Field::currency),
		ConfirmField("Save?:")
	);
	form._SetYpos(++content->nextYpos);
	content->_AddElements(form);

	do {		
		form._Show();
		if (form._Status()) {
			//	Get form data and pass it to controller
			utility::LinkedList<Data*>* data = form._GetData();
			profileController._AddNewProfile(data);
			moduler->_SetNextModule("Dashboard", this);
			exchangeRateController._WriteExchangeRate();
			break;
		}
		else if (form._IsPaused()) {			
		menu: // Menu selection
			Cursor(2, ::height - 4);
			UserInput select(InputType::select);
			int selection = 0;
			const char* nextModule = nullptr;
			while (selection <  1 || selection > mainMenu.size) {
				select._ReadUserInput();
				selection = select.selection;
				if (select.control == ControlKey::esc)
					goto exit;
				select._ClearInput();
			}
			nextModule = mainMenu._GetLink(selection);
			if (utility::_CompareChar(nextModule,this->name))
				continue;
			else {
				exit:
				if (form._Exit()) {
					if(nextModule == nullptr)
						moduler->_SetNextModule(previousModule);
					else moduler->_SetNextModule(nextModule, this);
					break;
				}
				else goto menu;
			}
		}
		else {
			moduler->_SetNextModule(previousModule);
			break;
		}
	} while (true);	

	if (form._Status()) {
		// change default currency at top of the currencies list
		Currency* ccy = exchangeRateController._GetCurrency(profileController._ActiveProfile()->_DefaultCurrency());
		exchangeRateController._SetDefaultCurrency(ccy);
	}

	//	Set module name (link) as the next one to be opened in main.cpp game loop.
	//	provide this module pointer as previoous module to enable ESC key in next module (get back to this module) option
}