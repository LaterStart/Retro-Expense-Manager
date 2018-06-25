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
	title._SetPadding(4);
	layout._Select("SelectionTitle")->_AddElements(title);	

	Label info("Please fill out the form:");
	info._SetPadding(4);

	//	Main menu
	Menu mainMenu;
	mainMenu._AddItems(	
		MenuItem("Create Profile", "CreateUserProfile"),
		MenuItem("Load Database", "LoadDatabase")
	);	
	mainMenu._SetPadding(1);
	layout._Select("Menu")->_AddElements(mainMenu);	
	
	//	Control menu
	Menu controlMenu;
	MenuItem F1("Menu", this);
	MenuItem ESC("Cancel", previousModule);
	F1._SetSpecialPrefix("[F1] ");
	F1._SetPadding(1);
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
		OptionField("Password protected?:", Field::pwStatus,
			// add optional fields 
			// true marks as key password field
			PasswordField("Password:", true),	
			PasswordField("Repeat password:")),
		FormField("Default currency:", InputType::text, Field::defCCYid),
		ConfirmField("Save?:")
	);
	form._SetPadding(4);
	form._SetYpos(++content->nextYpos);
	content->_AddElements(form);

	do {		
		form._Show();
		if (form._GetStatus()) {
			//	Get form data and pass it to controller
			utility::LinkedList<Data*>* data = form._GetData();
			profileController._AddNewProfile(data);
			moduler->_SetNextModule("Dashboard", this);
			break;
		}
		else if (form._IsPaused()) {			
		menu: // Menu selection
			Cursor(2, ::height - 4);
			UserInput select(InputType::menuSelect);
			int selection = 0;
			const char* nextModule = nullptr;
			while (selection <  1 || selection > mainMenu.size) {
				select._ReadUserInput();
				selection = select.selection;
				if (select.control == -1)
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
	//	Set module name (link) as the next one to be opened in main.cpp game loop.
	//	provide this module pointer as previoous module to enable ESC key in next module (get back to this module) option
}