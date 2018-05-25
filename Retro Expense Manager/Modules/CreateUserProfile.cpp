#include "CreateUserProfile.h"

CreateUserProfile::CreateUserProfile(ModuleManagement& moduler) : moduler(moduler), initialized(false) {}

CreateUserProfile::~CreateUserProfile() {}

void CreateUserProfile::_Initialize() {	
	moduler._AddModule(this, "CreateUserProfile");
	initialized = true;
}

CreateUserProfile& CreateUserProfile::_GetInstance(ModuleManagement& moduler) {
	static CreateUserProfile myRef(moduler);
	if (myRef.initialized == false) 
		myRef._Initialize();
	return myRef;
}

void CreateUserProfile::_Module() {

	




}