#pragma once
#include "ModuleManagement.h"

class CreateUserProfile : public Module {
private:
	bool initialized;
	void _Initialize();

	ModuleManagement& moduler;
	CreateUserProfile(ModuleManagement& moduler);
	~CreateUserProfile();

public:
	static CreateUserProfile& _GetInstance(ModuleManagement& moduler);
	CreateUserProfile(CreateUserProfile const&) = delete;
	void operator=(CreateUserProfile const&) = delete;

	void _Module();
};