#pragma once
#include "ModuleManagement.h"

class LoadUserProfile : public Module {
	template<class T>
	friend class ModuleRegistrar;
	friend class ModuleInitializer;
	friend class ModuleManagement;

private:
	bool initialized;

	LoadUserProfile();
	~LoadUserProfile();

	static LoadUserProfile& _LoadModule();
	Module & _GetInstance();
	void _StartModule();

public:	
	LoadUserProfile(LoadUserProfile const&) = delete;
	void operator=(LoadUserProfile const&) = delete;
};

// register module name into modules list registar
ModuleRegistrar<LoadUserProfile> _name("LoadUserProfile");