#pragma once
#include "ModuleManagement.h"

class AddAccountExt : public Module {
	template<class T>
	friend class ModuleRegistrar;
	friend class ModuleInitializer;
	friend class ModuleManagement;

private:

	AddAccountExt() = default;
	~AddAccountExt() = default;

	static AddAccountExt& _LoadModule();
	Module& _GetInstance() override;
	void _StartModule() override;

public:	
	AddAccountExt(AddAccountExt const&) = delete;
	void operator=(AddAccountExt const&) = delete;
};

// register module name into modules list registar
ModuleRegistrar<AddAccountExt> _name("AddAccountExt");