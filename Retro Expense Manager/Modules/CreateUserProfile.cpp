#include "CreateUserProfile.h"

CreateUserProfile::CreateUserProfile() : initialized(false) {}

CreateUserProfile::~CreateUserProfile() {}

void CreateUserProfile::_Initialize() {
	modulePtr link = &this->_Module;
	if (!initialized) {
		_AddModule(link, "CreateUserProfile");
		initialized = true;
	}
}

void CreateUserProfile::_Module() {

	
}