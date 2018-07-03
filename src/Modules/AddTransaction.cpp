#include "AddTransaction.h"
#include "../Models/Profile.h"
#include "../IO/IOComponent.h"
#include "../IO/Input.h"
using namespace std;

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
		MenuItem("Add Transaction", this->name),
		MenuItem("Add Account", "AddAccount"),
		MenuItem("Add Category", "AddCategoryExt")
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
		SelectionField("Type:", transactionController.transactionType, Field::transactionType),
		ScrollDown_2D<Category>("Category:", categoryController.categoryList, Field::category),
		FormField("Amount:", InputType::text, Field::amount),
		FormField("Currency:", InputType::text, Field::currency),
		FormField("Account:", InputType::text, Field::account),		
		FormField("Description:", InputType::text, Field::description),
		ConfirmField("Save?:")
	);
	form._SetYpos(++content->nextYpos);

	//	Form events
	//	Transfer event - If user selects transfer as transaction type, change account field
	const function<void(Form&, FormField*)> transferEvent = [](Form& form, FormField* currentField) {
		if (form._EventStatus(0) == false) {
			FormField* cField = form._SelectField("Type:");
			if (cField->inputField->selection == 3) {
				// remove one field at position 4
				form._RemoveFields(4, 1);
				form._InsertFields(
					//	Adds two new fields at position 4 and 5
					tuple<FormField&, int>{ FormField("From Account:", InputType::text, Field::account), 4 },
					tuple<FormField&, int>{ FormField("To Account:", InputType::text, Field::account), 5 }
				);				
				form._InitializeFields();
				form._SetEventStatus(0, true);
			}
		}
		else {
			//  If user changed his mind and wants not to select transfer as transaction type
			FormField* cField = form._SelectField("Type:");
			if (cField->inputField->selection != 3) {
				// remove two fields starting at position 4 and insert one new field at position 4
				form._RemoveFields(4, 2);
				form._InsertFields(tuple<FormField&, int>{ FormField("Account:", InputType::text, Field::account), 4 });
				form._InitializeFields();				
				form._SetEventStatus(0, false);
			}
		}
	};
	form._AddEvent(transferEvent);

	//	New category event - Provide option to add new category
	const function<void(Form&, FormField*)> newCategoryEvent = [](Form& form, FormField* currentField) {
		if (form._EventStatus(1) == false) {
			ScrollDown_2D<Category>* cField = dynamic_cast<ScrollDown_2D<Category>*>(form._SelectField("Category:"));
			vector<vector<Category>>& items = cField->_Items();
			// insert new element at beggining of Category list 2D vector		
			//items.insert(items.begin(), vector<Category>(1));
			//items[0].insert(items[0].begin(), Category("Add new category->"));
			form._SetEventStatus(1, true);
		}
		else if (form._EventStatus(1) == true) {
			// if user selects option to add new category
			if (utility::_CompareChar(currentField->text, (char*)"Category:")) {
				if (currentField->inputField->selection == 0 && currentField->inputField->control == ControlKey::none) {
					// open add new category extension form
					form._SwitchToExtension("AddCategoryExt");
				}
			}
		}
	};
	form._AddEvent(newCategoryEvent);
	form._LinkModuler(this->moduler);
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
				if (select.control == ControlKey::esc)
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