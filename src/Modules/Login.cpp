#include "Login.h"
#include "../Controllers/ProfileController.h"
#include "../View/IOComponent.h"
#include "../config.h"

Login::Login() : initialized(false) {}

Login::~Login() {}

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
	if (profCtrl._GetStatus() == true) {

	}
	else {
		//	if no recent user profile is found, display available options
		Display moduleDsp;
		moduleDsp._DisplayFrame_Default();

		Menu menu;
		menu._AddElements({ "Create Profileasdasdasdasdasdasdasd", "Load Profile", 0 });
		menu._AddLinks({ "CreateUserProfile", "LoadUserProfile", 0 });
		menu._ModifyBorder(moduleDsp._GetFrameSeparators(), moduleDsp._GetFrameSeparatorsNum());
		menu._ShowMenu();
	}

}