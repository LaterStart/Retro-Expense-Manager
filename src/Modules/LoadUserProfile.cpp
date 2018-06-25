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


}