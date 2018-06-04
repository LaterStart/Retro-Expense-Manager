#include "LoadUserProfile.h"
#include "../Controllers/ProfileController.h"
#include "../config.h"

LoadUserProfile::LoadUserProfile() : initialized(false) {}

LoadUserProfile::~LoadUserProfile() = default;

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
	if (profCtrl._GetStatus()) {

	}
	else {
	



	}

}