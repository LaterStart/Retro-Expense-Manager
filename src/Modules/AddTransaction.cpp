#include "AddTransaction.h"
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
		MenuItem("Add Account", "AddAccountExt"),
		MenuItem("Add Category", "AddCategoryExt")
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

	Frame* content = layout._Select("MainForm");
	content->_AddElements(info);
	layout._ShowElements();

	//	Input form
	Form form;
	form._SetParentFrame(content);
	form._AddFields(
		SelectionField("Type:", transactionController.transactionType, Field::transactionType),
		ScrollDown_2D<Category>("Category:", *categoryController.categoryList, Field::category),
		FormField("Amount:", InputType::value, Field::amount),
		ScrollDown<Currency>("Currency:", *exchangeRateController.currencies, Field::currency),
		ScrollDown<Account>("Account:", *accountController.accounts, Field::account),
		DateField("Date:", Field::date),
		FormField("Description:", InputType::text, Field::description),
		ConfirmField("Save?:")
	);
	form._SetYpos(2);

	//	Form events
	//	Transfer event - If user selects transfer as transaction type, change account field
	const function<void(Form&, FormField*)> transferEvent = [](Form& form, FormField* currentField) {
		if (form._EventStatus(0) == false) {
			FormField* cField = form._SelectField(Field::transactionType);
			if (cField->inputField->selection == 3) {
				// remove one field at position 4
				form._RemoveFields(4, 1);
				form._InsertFields(
					//	Adds two new fields at position 4 and 5
					tuple<ScrollDown<Account>&, int>{ ScrollDown<Account>("From Account:", *accountController.accounts, Field::account), 4 },
					tuple<ScrollDown<Account>&, int>{ ScrollDown<Account>("To Account:", *accountController.accounts, Field::account), 5 }
				);				
				form._InitializeFields();
				form._SetEventStatus(0, true);
			}
		}
		else {
			//  If user changed his mind and wants not to select transfer as transaction type
			FormField* cField = form._SelectField(Field::transactionType);
			if (cField->inputField->selection != 3) {
				// remove two fields starting at position 4 and insert one new field at position 4
				form._RemoveFields(4, 2);
				form._InsertFields(tuple<ScrollDown<Account>&, int>{ ScrollDown<Account>("Account:", *accountController.accounts, Field::account), 4 });
				form._InitializeFields();				
				form._SetEventStatus(0, false);
			}
		}
	};
	form._AddEvent(transferEvent);

	//	New category event - Provide option to add new category
	const function<void(Form&, FormField*)> newCategoryEvent = [](Form& form, FormField* currentField) {
		if (form._EventStatus(1) == false) {
			ScrollDown_2D<Category>* cField = dynamic_cast<ScrollDown_2D<Category>*>(form._SelectField(Field::category));
			// insert new element at beggining of Category list 2D vector
			vector<Category> newItem{ Category("Add new category->", CategoryType::temporary) };
			cField->_InsertItem(newItem);
			form._SetEventStatus(1, true);
		}
		else {
			// if user selects option to add new category - open add new category extension module
			if (currentField != nullptr && utility::_CompareChar(currentField->text, (char*)"Category:")) {
				if (currentField->inputField->selection == 0 && currentField->inputField->control == ControlKey::none) {
					// open add new category extension form
					bool status = form._SwitchToExtension("AddCategoryExt");
					if (!status)
						currentField->_Show();					
				}
				else if (currentField->inputField->selection == 0 && currentField->inputField->control == ControlKey::downArrow) {
					currentField->_SetFilledStatus(false);
					currentField->_Show();
				}
			}
		}
	};
	form._AddEvent(newCategoryEvent);

	//	New account event - Provide option to add new account
	const function<void(Form&, FormField*)> newAccountEvent = [](Form& form, FormField* currentField) {
		if (form._EventStatus(2) == false) {
			ScrollDown<Account>* cField = dynamic_cast<ScrollDown<Account>*>(form._SelectField(Field::account));
			// insert new element at beggining of Accounts vector	
			cField->_InsertItem(Account("Add new account->", -1));			
			form._SetEventStatus(2, true);
		}
		else {
			// if user selects option to add new account - open new account extension module
			bool a = utility::_CompareChar(currentField->text, (char*)"Account:");
			bool b = utility::_CompareChar(currentField->text, (char*)"From Account:");
			bool c = utility::_CompareChar(currentField->text, (char*)"To Account:");
			if (currentField != nullptr && (a||b||c)) {
				if (currentField->inputField->selection == 0 && currentField->inputField->control == ControlKey::none) {
					// open add new account extension form
					bool status = form._SwitchToExtension("AddAccountExt");
					if (!status)
						currentField->_Show();
				}
				else if (currentField->inputField->selection == 0 && currentField->inputField->control == ControlKey::downArrow) {
					currentField->_SetFilledStatus(false);
					currentField->_Show();
				}
			}
		}
	};
	form._AddEvent(newAccountEvent);

	//	Check transfer accounts event - Can't transfer funds to same account
	const function<void(Form&, FormField*)> checkTransferEvent = [](Form& form, FormField* currentField) {
		if (form._EventStatus(0) == true) {
			if (currentField->field == Field::account && utility::_CompareChar(currentField->text , (char*)"To Account:")) {
				FormField* fromAcc = form._SelectField(Field::account);
				if (fromAcc->inputField->selection == currentField->inputField->selection) {
					ScrollDown<Account>* scroll = dynamic_cast<ScrollDown<Account>*>(currentField);
					form._SetSpecialContentHeight(scroll->_Items().size()-1);
					form._DisplayMessage("Can't transfer funds to same account.");
					form._SetSpecialContentHeight(0);
					currentField->_Show();
				}
			}
		}
	};
	form._AddEvent(checkTransferEvent);

	form._LinkModuler(this->moduler);
	moduler->_SelectModule("AddCategoryExt")->_LinkLayout(layout);
	moduler->_SelectModule("AddAccountExt")->_LinkLayout(layout);
	content->_AddElements(form);

	do {
	form:
		form._Show();
		if (form._Status()) {
			//	Get form data and pass it to controller
			FormField* field = form._SelectField(Field::category);
			ScrollDown_2D<Category>* categoryField = dynamic_cast<ScrollDown_2D<Category>*>(field);
			Category* category = categoryField->_Value();
			field->inputField->selection = category->_ID();
			utility::LinkedList<Data*>* data = form._GetData();			
			transactionController._AddNewTransaction(data, profileController._ActiveProfile()->_ID());
			moduler->_SetNextModule("Dashboard", this);
			break;
		}
		else if (form._IsPaused()) {
		menu: // Menu selection
			controlMenu._ChangeItem("Menu", "Back");
			Cursor(2, ::height - 4);
			UserInput select(InputType::select);
			int selection = 0;
			const char* nextModule = nullptr;
			while (selection <  1 || selection > mainMenu.size) {
				select._ReadUserInput();
				selection = select.selection;
				if (select.control == ControlKey::esc)
					goto exit;
				else if (select.control == ControlKey::F1) {
					controlMenu._ChangeItem("Back", "Menu");
					goto form;
				}
				select._ClearInput();
			}
			nextModule = mainMenu._GetLink(selection);
			if (utility::_CompareChar(nextModule, this->name))
				continue;
			else if (utility::_CompareChar(nextModule, "AddAccountExt") || utility::_CompareChar(nextModule, "AddCategoryExt")) {
				controlMenu._ChangeItem("Back", "Menu");
				form._SwitchToExtension(nextModule);
			}
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

	//	remove add new category temporary category from categories vector and remove add new account temporary account from accounts vector
	categoryController.categoryList->erase(categoryController.categoryList->begin());
	accountController.accounts->erase(accountController.accounts->begin());
}
