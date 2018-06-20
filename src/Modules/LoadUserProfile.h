#pragma once
#include "ModuleManagement.h"
#include "../Controllers/ProfileController.h"

class LoadUserProfile : public Module {
	template<class T>
	friend class ModuleRegistrar;
	friend class ModuleInitializer;
	friend class ModuleManagement;

private:
	ProfileController controller;

	LoadUserProfile() = default;
	~LoadUserProfile() = default;

	static LoadUserProfile& _LoadModule();
	Module & _GetInstance();
	void _StartModule();

public:	
	LoadUserProfile(LoadUserProfile const&) = delete;
	void operator=(LoadUserProfile const&) = delete;
};

// register module name into modules list registar
ModuleRegistrar<LoadUserProfile> _name("LoadUserProfile");