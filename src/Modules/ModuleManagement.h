#pragma once
#include <map>
#include <memory>
#include <functional>

class Module {
	template<class T>
	friend class ModuleRegistrar;
	friend class ModuleManagement;

protected:
	virtual void _Module() = 0;
	virtual void _Initialize() = 0;
};

class ModuleList {
public:
	const char* name;
	Module* module;

	ModuleList();
	~ModuleList();
};

class ModuleManagement {
private:
	ModuleList* moduleList;
	void _InitializeModules();
	bool initialized;

public:
	void _OpenModule(const char* name);
	void _OpenModule();

	ModuleManagement();
	~ModuleManagement();

	template <typename module>
	void _AddModule(module newModule, const char* name) {
		moduleList = new ModuleList[1];
		moduleList[0].module = newModule;

		int test = 0;
	}

	ModuleList* _GetModuleList() const;
};

inline ModuleList* ModuleManagement::_GetModuleList() const {
	return moduleList;
}

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

template<class T>
class ModuleRegistrar {
public:
	// registrar constructor - register the class name at compile time 
	ModuleRegistrar(std::string moduleName) {		
		ModuleInitializer& instance = instance._GetInstance();
		instance._RegisterModule(moduleName, [](void) -> Module* { return new T(); });
	}	
};