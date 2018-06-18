#pragma once
#include "_Controller.h"
#include "../config.h"

class ModuleController {
public:	
	ModuleController();
	~ModuleController();

	utility::LinkedList<std::string>* _ReadModules();
};