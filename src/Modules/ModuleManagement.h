#pragma once
#include <map>
#include <memory>
#include <functional>
#include "../Controllers/ProfileController.h"
#include "../Controllers/TransactionController.h"
#include "../Controllers/CategoryController.h"
#include "../Controllers/ModuleController.h"

class Console; class ProfileController; class TransactionController; class CategoryController;
//	abstract class - main definition for each new module
class Module {
	template<class T>
	friend class ModuleRegistrar;
	friend class ModuleManagement;

protected:
	virtual void _StartModule() = 0;
	virtual Module& _GetInstance() = 0;

	ModuleManagement* moduler;
	Console* console;
	Module* previousModule;	

	static ProfileController profileController;
	static TransactionController transactionController;
	static CategoryController categoryController;

public:
	const char* name;
	operator const char*() {
		return name;
	}
};

//	container class - enables storage of active modules singleton references
class ModuleList {
public:
	std::string name;
	Module* module;

	ModuleList() = default;
	~ModuleList() = default;
};

//	class provides module management, instance initialization
class ModuleManagement {
private:
	ModuleList* moduleList;
	int moduleNum;

	void _InitializeModules();
	bool initialized;

	Console* console;

	const char* nextModule;
	Module* previousModule;

public:
	void _OpenModule(const char* name);
	void _OpenNextModule();
	void _SetNextModule(const char* name, Module* previousModule = nullptr);
	void _SetNextModule(Module* nextModule, Module* previousModule = nullptr);

	ModuleManagement(Console* myConsole);
	~ModuleManagement();

	//	Add reference for each pre-registered module from Modules folder to active modules list at run time
	template <typename module>
	void _AddModule(module& newModule, std::string name) {
		ModuleList* newSlot = new ModuleList;
		utility::_AddElement(moduleList, *newSlot, moduleNum);
		moduleList[moduleNum - 1].module = &newModule;
		moduleList[moduleNum - 1].name = name;
	}
};

inline void ModuleManagement::_OpenNextModule() {	
	_OpenModule(this->nextModule);
}

inline void ModuleManagement::_SetNextModule(const char* name, Module* previousModule) {	
	this->nextModule = name;
	this->previousModule = previousModule;
}

inline void ModuleManagement::_SetNextModule(Module* nextModule, Module* previousModule) {
	this->nextModule = nextModule->name;
	this->previousModule = previousModule;
}

//	compile time class used to add each module into register
class ModuleInitializer {
	template<class T>
	friend class ModuleRegistrar;
	friend class ModuleManagement;

	//	registry with created modules
	std::map<std::string, std::function<Module*(void)>> moduleRegistry;

	//	activate module initializer at compile time
	static ModuleInitializer& ModuleInitializer::_GetInstance();

	//	register modules into registry
	void _RegisterModule(std::string moduleName, std::function<Module*(void)> moduleInitializer);

	//	creates instance of requested module at run time
	std::shared_ptr<Module>ModuleInitializer::_CreateInstance(std::string name);	
};

//	storage class - compile time register
template<class T>
class ModuleRegistrar {
public:
	// registrar constructor - register the class name at compile time 
	ModuleRegistrar(std::string moduleName) {		
		ModuleInitializer& instance = ModuleInitializer::_GetInstance();
		instance._RegisterModule(moduleName, [](void) -> Module* { return new T(); });
	}	
};