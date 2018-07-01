#pragma once
#include "ModuleManagement.h"

class AddCategoryExt : public Module {
	template<class T>
	friend class ModuleRegistrar;
	friend class ModuleInitializer;
	friend class ModuleManagement;

private:

	AddCategoryExt() = default;
	~AddCategoryExt() = default;

	static AddCategoryExt& _LoadModule();
	Module& _GetInstance() override;
	void _StartModule() override;

public:	
	AddCategoryExt(AddCategoryExt const&) = delete;
	void operator=(AddCategoryExt const&) = delete;
};

// register module name into modules list registar
ModuleRegistrar<AddCategoryExt> _name("AddCategoryExt");