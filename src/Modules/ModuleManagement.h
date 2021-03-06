#pragma once
#include <map>
#include <memory>
#include <functional>
#include "../Controllers/ProfileController.h"
#include "../Controllers/TransactionController.h"
#include "../Controllers/CategoryController.h"
#include "../Controllers/AccountController.h"
#include "../Controllers/ExchangeRateController.h"
#include "../IO/OComponent.h"

class Console; class ProfileController; class TransactionController; class CategoryController;
//	abstract class - main definition for each new module
class Module {
	template<class T>
	friend class ModuleRegistrar;
	friend class ModuleManagement;
	friend class ModuleInitializer;

protected:
	virtual void _StartModule() = 0;
	virtual Module& _GetInstance() = 0;

	ModuleManagement* moduler = nullptr;
	Console* console = nullptr;
	Module* previousModule = nullptr;
	Layout* layout = nullptr;

	static ProfileController profileController;
	static TransactionController transactionController;
	static CategoryController categoryController;
	static AccountController accountController;
	static ExchangeRateController exchangeRateController;

	bool extensionStatus = false;	

public:
	const char* name;
	operator const char*() {
		return name;
	}

	void _LinkLayout(Layout& layout);
	void _SetExtensionStatus(bool status);
	bool _ExtensionStatus() const;
};

inline void Module::_LinkLayout(Layout& layout) {
	this->layout = &layout;
}

inline void Module::_SetExtensionStatus(bool status) {
	this->extensionStatus = status;
}

inline bool Module::_ExtensionStatus() const {
	return this->extensionStatus;
}

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
	Module* currentModule;

public:
	void _OpenModule(const char* name);
	void _OpenModule(const char* name, Module* mainModule);
	void _OpenNextModule();
	void _SetNextModule(const char* name, Module* previousModule = nullptr);
	void _SetNextModule(Module* nextModule, Module* previousModule = nullptr);
	Module* _SelectModule(const char* name);
	const char* _NextModule() const;

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
	Module* _CurrentModule() const;
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

inline Module* ModuleManagement::_CurrentModule() const {
	return this->currentModule;
}

inline const char* ModuleManagement::_NextModule() const {
	return this->nextModule;
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

	//	creates instances of registered modules at run time
	void ModuleInitializer::_CreateInstances(ModuleManagement* moduler);
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