#include "AddCategoryExt.h"
#include "../Models/Profile.h"
#include "../IO/IOComponent.h"
#include "../IO/Input.h"
using namespace std;

Module& AddCategoryExt::_GetInstance() {
	return _LoadModule();
}

AddCategoryExt& AddCategoryExt::_LoadModule() {
	static AddCategoryExt module;
	return module;
}

void AddCategoryExt::_StartModule() {
	Display dsp;
	Frame* extTitle = layout->_Select("ExtensionTitle");
	Frame* content = layout->_Select("ExtensionForm");
	// Set frames to have independend display from main frame
	extTitle->dsp = &dsp;
	content->dsp = &dsp;

	Label title("Add Category ", ::headerSymbol, "left");
	extTitle->_AddElements(title);
	title._Show();

	Label info("Please fill out the form:");

	//	Input form
	Form form;
	form._SetParentFrame(content);
	form._AddFields(
		SelectionField("Type:", categoryController.categoryType, Field::categoryType),
		FormField("Category Name:", InputType::text, Field::categoryName),
		ConfirmField("Save?:")
	);
	content->_AddElements(info, form);

	//	Form events
	//	If user selects to add new sub category - provide option to select parent main category
	const function<void(Form&, FormField*)> subCategoryEvent = [](Form& form, FormField* currentField) {
		if (form._EventStatus(0) == false) {
			FormField* cField = form._SelectField(Field::categoryType);
			if (cField->inputField->selection == 2) {			
				vector<Category>* mainCategories = new vector<Category>(categoryController._GetMainCategoryList());
				// provide option to add new main category within sub category selection
				mainCategories->insert(mainCategories->begin(), Category("New main category->", CategoryType::temporary));
				form._InsertFields(tuple<ScrollDown<Category>&, int>{ 
					ScrollDown<Category>("Main Category:", *mainCategories, Field::parentCategory), 1 
				});
				form._InitializeFields();
				form._SetEventStatus(0, true);
			}			
		}		
		else {
			// If user changes his mind and wants to add new main category
			FormField* cField = form._SelectField(Field::categoryType);
			if (cField->inputField->selection == 1) {
				FormField* sField = form._SelectField(Field::parentCategory);
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

	//	If user selects to add new main category within sub category selection
	const function<void(Form&, FormField*)> newMainCategoryEvent = [](Form& form, FormField* currentField) {
		if (form._EventStatus(1) == false) {
			FormField* cField = form._SelectField(Field::categoryType);			
			if (form._EventStatus(0) == true && cField->inputField->selection == 2) {
				if (currentField->field == Field::parentCategory && currentField->inputField->selection == 0) {
					form._InsertFields(tuple<FormField&, int>{
						FormField("Name:", InputType::text, Field::categoryName), 2
					});
					form._InitializeFields();
					form._SetEventStatus(1, true);
				}
			}
		}
		else {
			// If user adds new main category within sub category selection - update scrolldown & form
			if (currentField->field == Field::categoryName) {
				ScrollDown<Category>* cField = dynamic_cast<ScrollDown<Category>*>(form._SelectField(Field::parentCategory));
				vector<Category>* mainCategories = &cField->_Items();
				mainCategories->push_back(Category(currentField->inputField->input, CategoryType::mainCategory));				
				categoryController._AddNewCategory(mainCategories->back(), profileController._ActiveProfile()->_ID());

				form._RemoveFields(2, 1);
				form._InitializeFields();
				form._SetEventStatus(1, false);
				cField->_UpdateScrollDown();
				cField->_Show();
			}
		}
	};
	form._AddEvent(newMainCategoryEvent);

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
			bool subCategory = false;
			int mainIndex;
			UserInput* field = form._GetData(Field::categoryType);
			if (field->selection - 1 == 1) {
				//	If subcategory - pass parent category ID
				field = form._GetData(Field::parentCategory);
				mainIndex = field->selection;
				field->selection = categoryController.categoryList->at(field->selection).at(0)._ID();
				subCategory = true;
			}
			utility::LinkedList<Data*>* data = form._GetData();
			categoryController._AddNewCategory(data, profileController._ActiveProfile()->_ID());	

			//  Update main form scroll control			
			std::vector<FrameElement*>elements = layout->_Select("MainForm")->_SelectElements(ComponentType::form);
			Form* mainForm = dynamic_cast<Form*>(elements.at(0));
			ScrollDown_2D<Category>* scroll = dynamic_cast<ScrollDown_2D<Category>*>(mainForm->_SelectField(Field::category));
			scroll->_UpdateScrollControl();					
			if (subCategory) {
				field = form._GetData(Field::parentCategory);
				scroll->_ToggleSubSelect(true, mainIndex);
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