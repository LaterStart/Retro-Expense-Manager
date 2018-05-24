#pragma once

class ModuleList {
public:
	//typedef void(*functionPtr) ();

	const char* name;
	//functionPtr link;

	ModuleList();
	~ModuleList();
};

class ModuleManagement {
public:
	typedef void (ModuleManagement::*modulePtr)();

protected:
	ModuleManagement* base;
	void _AddModule(modulePtr link, const char* name);
	ModuleList* modulesList;	

public:
	void _OpenModule(const char* name);
	void _OpenModule(modulePtr link);

	ModuleManagement();
	~ModuleManagement();
};