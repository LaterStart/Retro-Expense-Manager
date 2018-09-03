#include "ModuleManagement.h"
using namespace std;

ProfileController Module::profileController;
TransactionController Module::transactionController;
CategoryController Module::categoryController;
AccountController Module::accountController;
ExchangeRateController Module::exchangeRateController;

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
void ModuleInitializer::_CreateInstances(ModuleManagement* moduler) {
	Module* registar = nullptr;
	string name;
	for (auto it = moduleRegistry.begin(); it != moduleRegistry.end(); it++)	{
		name = it->first;
		registar = it->second();
		Module& ref = registar->_GetInstance();
		moduler->_AddModule(ref, name);
	}
}

//	Initialize registered modules at run time
void ModuleManagement::_InitializeModules() {
	ModuleInitializer& initializer = initializer._GetInstance();
	initializer._CreateInstances(this);	
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
			break;
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
			break;
		}
}

//	Returns pointer to selected module
Module* ModuleManagement::_SelectModule(const char* name) {
	for (int i = 0; i < moduleNum; i++)
		if (moduleList[i].name == name)
			return moduleList[i].module;
	return nullptr;
}
