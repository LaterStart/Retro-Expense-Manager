#pragma once
#include "Module.h"

class CreateUserProfile : Module, ModuleManagement {
private:
	bool initialized;
	CreateUserProfile();
	~CreateUserProfile();

public:
	void _Module();
	void _Initialize();
};