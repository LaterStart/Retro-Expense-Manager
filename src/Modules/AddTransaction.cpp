#include "AddTransaction.h"
#include "../Controllers/ProfileController.h"
#include "../Models/Profile.h"
#include "../IO/IOComponent.h"
#include "../IO/Input.h"

Module& AddTransaction::_GetInstance() {
	return _LoadModule();
}

AddTransaction& AddTransaction::_LoadModule() {
	static AddTransaction module;
	return module;
}

void AddTransaction::_StartModule() {
	//	Create default frame layout
	Frame* mainFrame = console->_GetMainFrame();
	Layout layout(mainFrame);
	Display display;

	layout._DefaultFrameTemplate(display);
	layout._Select("MenuHeader")->_AddElements(Label("Main Menu ", ::headerSymbol, "center"));
	layout._Select("Date")->_AddElements(Label(utility::_GetCurrentDate(), "left"));

	Label title("Add Transaction ", ::headerSymbol, "left");
	layout._Select("SelectionTitle")->_AddElements(title);

	Label info("Please fill out the form:");

	//	Main menu
	Menu mainMenu;
	mainMenu._AddItems(
		MenuItem("AddTransaction", this->name),
		MenuItem("Another Item", "LoadDatabase")
	);
	layout._Select("Menu")->_AddElements(mainMenu);

	//	Control menu
	Menu controlMenu;
	MenuItem F1("Menu", this);
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
		SelectionField("Type:", transactionType, transactionType_num, Field::transactionType),					
		FormField("Category:", InputType::text, Field::category),
		FormField("Amount", InputType::value, Field::amount),
		FormField("Currency:", InputType::text, Field::currency),
		FormField("Account:", InputType::text, Field::account),
		FormField("Description:", InputType::text, Field::description),
		ConfirmField("Save?:")
	);
	form._SetYpos(++content->nextYpos);
	content->_AddElements(form);

	do {
		form._Show();
		if (form._GetStatus()) {
			//	Get form data and pass it to controller
			utility::LinkedList<Data*>* data = form._GetData();
			transactionController._AddNewTransaction(data, profileController._ActiveProfile()->_ID());
			moduler->_SetNextModule("Dashboard", this);
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
				if (select.control == -1)
					goto exit;
				select._ClearInput();
			}
			nextModule = mainMenu._GetLink(selection);
			if (utility::_CompareChar(nextModule, this->name))
				continue;
			else {
			exit:
				if (form._Exit()) {
					if (nextModule == nullptr)
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