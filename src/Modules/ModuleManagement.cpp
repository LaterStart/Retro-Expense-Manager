#include <iostream> //debug
#include <string> //debug
#include "ModuleManagement.h"
#include "../Controllers/ModuleController.h"
using namespace std;

ModuleList::ModuleList() {}

ModuleList::~ModuleList() {}

ModuleManagement::ModuleManagement() : initialized(false){
	_InitializeModules();
}

ModuleManagement::~ModuleManagement(){}

//	wanna be framework implementation - each class registers its name at compile time
void ModuleInitializer::_RegisterModule(string moduleName, function<Module*(void)> moduleInitializer){
	moduleRegistry[moduleName] = moduleInitializer;
}

//	module initializer is activated at compile time
ModuleInitializer& ModuleInitializer::_GetInstance(){
	static ModuleInitializer instance;
	return instance;
}

//	create instance of module at run time
shared_ptr<Module>ModuleInitializer::_CreateInstance(string name) {
	Module* instance = nullptr;

	//	find name in the registry and call factory method.
	auto it = moduleRegistry.find(name);
	if (it != moduleRegistry.end())
		instance = it->second();

	//	wrap instance in a shared ptr and return
	if (instance != nullptr)
		return shared_ptr<Module>(instance);
	else
		return nullptr;
}

//	find existing module classes and initialize singleton instances
void ModuleManagement::_InitializeModules() {
	ModuleInitializer& initializer = initializer._GetInstance();
	ModuleController modCtrl;
	utility::LinkedList<string>* classList = modCtrl._ReadModules();

	while (classList != nullptr) {
		initializer._CreateInstance(classList->element);
		classList = classList->nextNode;
	}	
}

void ModuleManagement::_OpenModule(const char* name) {




}

void ModuleManagement::_OpenModule() {


	


} 