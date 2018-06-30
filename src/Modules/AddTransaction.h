#pragma once
#include "ModuleManagement.h"
#include "../Controllers/TransactionController.h"

class AddTransaction : public Module {
	template<class T>
	friend class ModuleRegistrar;
	friend class ModuleInitializer;
	friend class ModuleManagement;

private:

	AddTransaction() = default;
	~AddTransaction() = default;

	static AddTransaction& _LoadModule();
	Module& _GetInstance() override;
	void _StartModule() override;

public:	
	AddTransaction(AddTransaction const&) = delete;
	void operator=(AddTransaction const&) = delete;
};

// register module name into modules list registar
ModuleRegistrar<AddTransaction> _name("AddTransaction");