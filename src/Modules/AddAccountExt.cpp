#include "AddAccountExt.h"
#include "../Models/Profile.h"
#include "../IO/IOComponent.h"
#include "../IO/Input.h"
using namespace std;

Module& AddAccountExt::_GetInstance() {
	return _LoadModule();
}

AddAccountExt& AddAccountExt::_LoadModule() {
	static AddAccountExt module;
	return module;
}

void AddAccountExt::_StartModule() {
	Display dsp;
	Frame* extTitle = layout->_Select("ExtensionTitle");
	Frame* content = layout->_Select("ExtensionForm");
	// Set frames to have independend display from main frame
	extTitle->dsp = &dsp;
	content->dsp = &dsp;

	Label title("Add Account ", ::headerSymbol, "left");
	extTitle->_AddElements(title);
	title._Show();

	Label info("Please fill out the form:");

	//	Input form
	Form form;
	form._SetParentFrame(content);
	form._AddFields(
		ScrollDown<const char*>("Type:", accountController.accountType, Field::accountType),
		FormField("Name:", InputType::text, Field::accountName),
		FormField("Currency:", InputType::text, Field::currency),
		ConfirmField("Save?:")
	);
	content->_AddElements(info, form);

	//	Form event
	//	If user selects to add new sub category - provide option to select parent main category
	const function<void(Form&, FormField*)> subCategoryEvent = [](Form& form, FormField* currentField) {
		if (form._EventStatus(0) == false) {
			FormField* cField = form._SelectField("Type:");
			if (cField->inputField->selection == 2) {			
				vector<Category>* mainCategories = new vector<Category>(categoryController._GetMainCategoryList());
				form._InsertFields(tuple<ScrollDown<Category>&, int>{ 
					ScrollDown<Category>("Main Category:", *mainCategories, Field::parentCategory), 1 
				});
				form._InitializeFields();
				form._SetEventStatus(0, true);
			}			
		}		
		else {
			// If user changes his mind and wants to add new main category
			FormField* cField = form._SelectField("Type:");
			if (cField->inputField->selection == 1) {
				FormField* sField = form._SelectField("Main Category:");
				ScrollDown<Category>* scroll = dynamic_cast<ScrollDown<Category>*>(sField);
				vector<Category>* items = &scroll->_Items();
				delete items;
				form._RemoveFields(1, 1);
				form._InitializeFields();
				form._SetEventStatus(0, false);
			}
		}
	};
	form._AddEvent(subCategoryEvent);	
	form._SetYpos(2);
	info._Show();

	// use main module menu elements
	Menu* mainMenu = nullptr;
	Menu* controlMenu = nullptr;
	std::vector<FrameElement*> elements = layout->_SelectElements(ComponentType::menu);
	for (size_t i = 0; i < elements.size(); i++) {
		if (elements.at(i)->_ParentFrame() == layout->_Select("Menu"))
			mainMenu = dynamic_cast<Menu*>(elements.at(i));
		else if (elements.at(i)->_ParentFrame() == layout->_Select("Footer") )
			controlMenu = dynamic_cast<Menu*>(elements.at(i));
	}

	do {
	form:
		form._Show();
		if (form._Status()) {
			//	Get form data and pass it to controller
			utility::LinkedList<Data*>* data = form._GetData();
			categoryController._AddNewCategory(data, profileController._ActiveProfile()->_ID());	

			//  Update main form scroll control			
			std::vector<FrameElement*>elements = layout->_Select("MainForm")->_SelectElements(ComponentType::form);
			Form* mainForm = dynamic_cast<Form*>(elements.at(0));
			ScrollDown_2D<Category>* scroll = dynamic_cast<ScrollDown_2D<Category>*>(mainForm->_SelectField("Category:"));
			scroll->_UpdateScrollControl();
			UserInput* field = form._GetData(Field::categoryType);
			if (field->selection - 1 == 1) {
				field = form._GetData(Field::parentCategory);
				scroll->_ToggleSubSelect(true, field->selection+1);
			}
			break;
		}
		else if (form._IsPaused()) {
		menu: // Menu selection
			controlMenu->_ChangeItem("Menu", "Back");
			Cursor(2, ::height - 4);
			UserInput select(InputType::select);
			int selection = 0;
			const char* nextModule = nullptr;		

			while (selection <  1 || selection > mainMenu->size) {
				select._ReadUserInput();
				selection = select.selection;
				if (select.control == ControlKey::esc) {
					if (form._Exit()) 
						break;
				}
				else if (select.control == ControlKey::F1) {
					controlMenu->_ChangeItem("Back", "Menu");
					goto form;
				}
				select._ClearInput();
			}
			if (select.control == ControlKey::esc)
				break;
			nextModule = mainMenu->_GetLink(selection);
			if (utility::_CompareChar(nextModule, previousModule->name))
				break;
			else if (utility::_CompareChar(nextModule, "AddCategoryExt"))
				continue;
			else {
				if (form._Exit()) {
					moduler->_SetNextModule(nextModule);
					break;
				}							
				else goto menu;
			}
		}
		else break;
	} while (true);

	// frames on return will share display with main frame
	extTitle->dsp = nullptr;
	content->dsp = nullptr;

	// sets form outcome status
	this->previousModule->_SetExtensionStatus(false);
}