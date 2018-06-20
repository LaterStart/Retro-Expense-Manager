#include "LoadUserProfile.h"
#include "../Controllers/ProfileController.h"
#include "../config.h"

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
	profCtrl._GetLastUsedProfile();

	//	if profile contoller has found last used user
	

}