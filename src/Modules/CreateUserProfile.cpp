#include "CreateUserProfile.h"
#include "../View/IOComponent.h"
#include "../config.h"

CreateUserProfile::CreateUserProfile() : initialized(false) {}

CreateUserProfile::~CreateUserProfile() {}

Module& CreateUserProfile::_GetInstance() {
	return _LoadModule();
}

CreateUserProfile& CreateUserProfile::_LoadModule() {
	static CreateUserProfile module;	
	return module;
}

void CreateUserProfile::_StartModule() {
	Separator menuLine(20, 1, 25, 1);
	Separator headerLine(2, 2, ::width - 4, 0);
	Display moduleDsp;
	Menu menu;	
	menu._AddElements({ "Menu 01", "Menu 02", "Menu 03", 0 });
	menu._AddLinks({ "Dashboard", 0 });
	

}