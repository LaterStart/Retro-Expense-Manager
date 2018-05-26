#include "CreateUserProfile.h"

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

	int test = 0;
}