#pragma once
#include "ModuleManagement.h"

class Module {
protected:
	virtual void _Module() = 0;
	virtual void _Initialize() = 0;	
};