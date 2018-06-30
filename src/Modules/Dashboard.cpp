#include "Dashboard.h"
#include "../IO/IOComponent.h"
#include "../IO/Input.h"

Module& Dashboard::_GetInstance() {
	return _LoadModule();
}

Dashboard& Dashboard::_LoadModule() {
	static Dashboard module;
	return module;
}

void Dashboard::_StartModule() {
	//	Create default frame layout
	Frame* mainFrame = console->_GetMainFrame();
	Layout layout(mainFrame);
	Display display;

	layout._DefaultFrameTemplate(display);
	layout._Select("MenuHeader")->_AddElements(Label("Main Menu ", ::headerSymbol, "center"));
	layout._Select("Date")->_AddElements(Label(utility::_GetCurrentDate(), "left"));

	Label title("Dashboard ", ::headerSymbol, "left");
	layout._Select("SelectionTitle")->_AddElements(title);

	//	Main menu
	Menu mainMenu;
	mainMenu._AddItems(
		MenuItem("Add Transaction", "AddTransaction"),
		MenuItem("Another item", "LoadDatabase")
	);
	layout._Select("Menu")->_AddElements(mainMenu);
	layout._ShowElements();

	Cursor(2, ::height - 4);
	UserInput select(InputType::select);
	int selection = 0;
	while (selection <  1 || selection > mainMenu.size) {
		select._ReadUserInput();
		selection = select.selection;
		select._ClearInput();
	}
	moduler->_SetNextModule(mainMenu._GetLink(selection), this);
}