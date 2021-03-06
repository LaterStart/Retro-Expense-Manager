#pragma once
#include "ModuleManagement.h"

class CreateUserProfile : public Module {
	template<class T>
	friend class ModuleRegistrar;
	friend class ModuleInitializer;
	friend class ModuleManagement;

private:

	CreateUserProfile() = default;
	~CreateUserProfile() = default;

	static CreateUserProfile& _LoadModule();
	Module & _GetInstance();
	void _StartModule();

public:	
	CreateUserProfile(CreateUserProfile const&) = delete;
	void operator=(CreateUserProfile const&) = delete;
};

// register module name into modules list registar
ModuleRegistrar<CreateUserProfile> _name("CreateUserProfile");