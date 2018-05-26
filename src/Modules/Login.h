#pragma once
#include "ModuleManagement.h"

class Login : public Module {
	template<class T>
	friend class ModuleRegistrar;
	friend class ModuleInitializer;
	friend class ModuleManagement;

private:
	bool initialized;

	Login();
	~Login();

	static Login& _LoadModule();
	Module & _GetInstance();
	void _StartModule();

public:	
	Login(Login const&) = delete;
	void operator=(Login const&) = delete;
};

// register module name into modules list registar
ModuleRegistrar<Login> _name("Login");