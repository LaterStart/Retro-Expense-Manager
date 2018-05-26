#pragma once
#include "Controller.h"
#include "../config.h"

class ModuleController : public Controller {
public:	
	ModuleController();
	~ModuleController();

	utility::LinkedList<std::string>* _ReadModules();
};