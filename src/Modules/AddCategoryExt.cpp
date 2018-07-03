#include "AddCategoryExt.h"
#include "../Models/Profile.h"
#include "../IO/IOComponent.h"
#include "../IO/Input.h"

Module& AddCategoryExt::_GetInstance() {
	return _LoadModule();
}

AddCategoryExt& AddCategoryExt::_LoadModule() {
	static AddCategoryExt module;
	return module;
}

void AddCategoryExt::_StartModule() {
	//	Create default frame layout
	Frame* mainFrame = console->_GetMainFrame();
	Layout layout(mainFrame);
	Display display; // -change

	layout._DefaultFrameTemplate(display);
	Label title("Add Category ", ::headerSymbol, "left");
	layout._Select("ExtensionTitle")->_AddElements(title);
	Frame* content = layout._Select("ExtensionForm");

	Label info("Please fill out the form:");

	//	Input form
	Form form;
	form._SetParentFrame(content);
	form._AddFields(
		SelectionField("Type:", categoryController.categoryType, Field::categoryType),
		FormField("Name:", InputType::text, Field::categoryName),
		ConfirmField("Save?:")
	);
	content->_AddElements(info, form);

	//	Form events
	//	
	const std::function<void(Form&, FormField*)> newCategoryEvent = [](Form& form, FormField* currentField) {

	};

	layout._ShowElements();
	
}