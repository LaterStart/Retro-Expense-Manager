#pragma once
#include "ModuleManagement.h"

class CreateUserProfile : public Module {
	template<class T>
	friend class ModuleRegistrar;
	friend class ModuleInitializer;
private:
	bool initialized;
	void _Initialize();

	CreateUserProfile();
	~CreateUserProfile();	

public:
	static CreateUserProfile& _GetInstance();
	CreateUserProfile(CreateUserProfile const&) = delete;
	void operator=(CreateUserProfile const&) = delete;

	void _Module();
};

// register module name into modules list registar
ModuleRegistrar<CreateUserProfile> _name = "CreateUserProfile";