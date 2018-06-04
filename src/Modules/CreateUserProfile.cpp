#include "CreateUserProfile.h"
#include "../IO/IOComponent.h"
#include "../config.h"

CreateUserProfile::CreateUserProfile() : initialized_(false) {}

CreateUserProfile::~CreateUserProfile() = default;

Module& CreateUserProfile::_GetInstance() {
	return _LoadModule();
}

CreateUserProfile& CreateUserProfile::_LoadModule() {
	static CreateUserProfile module;	
	return module;
}

void CreateUserProfile::_StartModule() {

	

}