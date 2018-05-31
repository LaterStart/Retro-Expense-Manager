#include "Login.h"
#include "../Controllers/ProfileController.h"
#include "../View/IOComponent.h"
#include "../View/OComponent.h"
#include "../config.h"

Login::Login() : initialized_(false) {}

Login::~Login() = default;

Module& Login::_GetInstance() {
	return _LoadModule();
}

Login& Login::_LoadModule() {
	static Login module;
	return module;
}

void Login::_StartModule() {
	//	Check for last used user profile in preconfigured "dataPath" binary file
	ProfileController profCtrl;
	profCtrl._GetLastUsedProfile(::dataPath);

	//	if profile contoller has found last used user
	if (profCtrl._GetStatus()) {

	}
	else {
		//	if no recent user profile is found, display available options
		Frame* mainFrame = console->_GetMainFrame();	
		Layout layout(mainFrame);
		Display display;		

		layout._DefaultFrameTemplate(display);
		layout._Select("MenuHeader")->_AddElement(Label("Main Menu ",::headerSymbol, "center"));		
		layout._Select("Date")->_AddElement(Label(utility::_GetCurrentDate(), "left"));

		Label userProfile("User Profile ", ::headerSymbol, "left");
		userProfile._SetPadding(4);
		layout._Select("SelectionTitle")->_AddElement(userProfile);
		layout._ShowElements();








		
		/*Menu menu(moduleDsp.)
		menu._AddElements({ "Create Profileasdasdasdasdasdasdasd", "Load Profile", 0 });
		menu._AddLinks({ "CreateUserProfile", "LoadUserProfile", 0 });
		menu._SetPosition("right");
		menu._ModifyBorder(moduleDsp._GetFrameSeparators(), moduleDsp._GetFrameSeparatorsNum());
		menu._ShowMenu();*/
	}
}