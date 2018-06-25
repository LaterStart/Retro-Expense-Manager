#pragma once
#include "ModuleManagement.h"

class Dashboard : public Module {
	template<class T>
	friend class ModuleRegistrar;
	friend class ModuleInitializer;
	friend class ModuleManagement;

private:

	Dashboard() = default;
	~Dashboard() = default;

	static Dashboard& _LoadModule();
	Module& _GetInstance() override;
	void _StartModule() override;

public:	
	Dashboard(Dashboard const&) = delete;
	void operator=(Dashboard const&) = delete;
};

// register module name into modules list registar
ModuleRegistrar<Dashboard> _name("Dashboard");