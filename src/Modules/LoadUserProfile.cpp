#include "LoadUserProfile.h"
#include "../Controllers/ProfileController.h"
#include "../View/IOComponent.h"
#include "../config.h"

LoadUserProfile::LoadUserProfile() : initialized(false) {}

LoadUserProfile::~LoadUserProfile() {}

Module& LoadUserProfile::_GetInstance() {
	return _LoadModule();
}

LoadUserProfile& LoadUserProfile::_LoadModule() {
	static LoadUserProfile module;
	return module;
}

void LoadUserProfile::_StartModule() {
	//	Check for last used user profile in preconfigured "dataPath" binary file
	ProfileController profCtrl;
	profCtrl._GetLastUsedProfile(::dataPath);

	//	if profile contoller has found last used user
	if (profCtrl._GetStatus() == true) {

	}
	else {
		//	if no recent user profile is found
		Display moduleDsp;
		moduleDsp._DisplayFrame_Default();

		Menu menu;
		menu._AddElements({ "Create new profile", "Load existing profile", 0 });
		menu._AddLinks({ "CreateUserProfile",  0 });

		menu._ShowMenu();


		int test = 0;





	}

}