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
	Profile* _GetProfile(char* username);
	void _SwitchProfile(Profile* profile);
	Profile* _ActiveProfile();
};

inline Profile* ProfileController::_ActiveProfile() {
	return this->activeProfile;
}