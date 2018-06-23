#pragma once
#include "_Controller.h"

class Profile; class ModelHeader;
class ProfileController : public Controller {
private:
	static ModelHeader header;
	Profile* activeProfile = nullptr;

public:
	ProfileController();
	~ProfileController() = default;

	Profile* _GetLastUsedProfile();
	bool _Exists(char* username);
	void _AddNewProfile(utility::LinkedList<Data*>*data);	
};