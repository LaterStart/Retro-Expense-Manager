#pragma once

class Module {
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