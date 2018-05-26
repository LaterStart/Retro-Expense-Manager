#include "CreateUserProfile.h"

CreateUserProfile::CreateUserProfile() : initialized(false) {}

CreateUserProfile::~CreateUserProfile() {}

void CreateUserProfile::_Initialize() {	
	//moduler._AddModule(this, "CreateUserProfile");
	initialized = true;
}

CreateUserProfile& CreateUserProfile::_GetInstance() {
	static CreateUserProfile myRef;
	if (myRef.initialized == false) 
		myRef._Initialize();
	return myRef;
}

void CreateUserProfile::_Module() {


}