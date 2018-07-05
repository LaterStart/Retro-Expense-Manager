#include "ModuleManagement.h"
using namespace std;

ProfileController Module::profileController;
TransactionController Module::transactionController;
CategoryController Module::categoryController;

ModuleManagement::ModuleManagement(Console* myConsole) : moduleList(nullptr), moduleNum(0), initialized(false), console(myConsole){
	_InitializeModules();
}

ModuleManagement::~ModuleManagement(){
	delete[]moduleList;
}

//	Wanna be framework implementation - each module class registers its name at compile time
void ModuleInitializer::_RegisterModule(string moduleName, function<Module*(void)> moduleInitializer){
	moduleRegistry[moduleName] = moduleInitializer;
}

//	Module initializer is activated at compile time
ModuleInitializer& ModuleInitializer::_GetInstance(){
	static ModuleInitializer instance;
	return instance;
}

//	Create instance of module at run time
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

//	Find existing module classes and initialize singleton instances
void ModuleManagement::_InitializeModules() {
	ModuleInitializer& initializer = initializer._GetInstance();
	ModuleController modCtrl;
	utility::LinkedList<string>* classList = modCtrl._ReadModules();
	utility::LinkedList<string>* deleter = nullptr;

	while (classList != nullptr) {
		auto module = initializer._CreateInstance(classList->element);	
		if (module != nullptr) {
			Module& ref = module->_GetInstance();
			_AddModule(ref, classList->element);
		}	
		deleter = classList;
		classList = classList->nextNode;
		delete deleter;
	}	
}

//	Open module from active modules list using module name
void ModuleManagement::_OpenModule(const char* name) {
	for (int i = 0; i < moduleNum; i++)
		if (moduleList[i].name == name) {
			moduleList[i].module->moduler = this;
			moduleList[i].module->console = this->console;
			moduleList[i].module->previousModule = this->previousModule;
			moduleList[i].module->name = name;
			this->currentModule = moduleList[i].module;
			moduleList[i].module->_StartModule();
		}
}

//	Open module as extension module
void ModuleManagement::_OpenModule(const char* name, Module* mainModule) {
	for (int i = 0; i < moduleNum; i++)
		if (moduleList[i].name == name) {
			moduleList[i].module->moduler = this;
			moduleList[i].module->console = this->console;
			moduleList[i].module->previousModule = mainModule;
			moduleList[i].module->name = name;
			this->currentModule = moduleList[i].module;
			moduleList[i].module->_StartModule();
			this->currentModule = mainModule;
		}
}

//	Returns pointer to selected module
Module* ModuleManagement::_SelectModule(const char* name) {
	for (int i = 0; i < moduleNum; i++)
		if (moduleList[i].name == name)
			return moduleList[i].module;
	return nullptr;
}