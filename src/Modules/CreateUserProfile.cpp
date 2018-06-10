#include "CreateUserProfile.h"
#include "../IO/IOComponent.h"
#include "../IO/Input.h"
#include "../config.h"

CreateUserProfile::CreateUserProfile() : initialized_(false) {}

CreateUserProfile::~CreateUserProfile() = default;

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

	Label userProfile("Create Profile ", ::headerSymbol, "left");
	userProfile._SetPadding(4);
	layout._Select("SelectionTitle")->_AddElements(userProfile);

	//	Main menu
	Menu mainMenu;
	MenuItem esc("Back", previousModule);
	esc._SetSpecialPrefix("[ESC] ");
	esc._SetYpos(1);
	mainMenu._SetPadding(1);
	mainMenu._AddItems(
		MenuItem("Load Profile", "LoadUserProfile"),		
		esc
	);	
	layout._Select("Menu")->_AddElements(mainMenu);	
	
	//	Input form
	Form form;
	form._AddFields(
		FormField("Username:", InputType::text),
		OptionField("Password protected?:",
			// add optional fields - true marks as key password field
			PasswordField("Password:", true),	
			PasswordField("Repeat password:")),
		FormField("Default currency:", InputType::text),
		ConfirmField("Save?:")
	);
	form._SetPadding(4);
	layout._Select("Content")->_AddElements(form);
	layout._ShowElements(); 
}