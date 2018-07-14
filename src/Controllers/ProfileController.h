#pragma once
#include "_Controller.h"
#include "../Models/Profile.h"

class Profile; class ModelHeader;
class ProfileController : public Controller {
private:
	static ModelHeader header;
	Profile* activeProfile = nullptr;	

public:
	ProfileController();
	~ProfileController() = default;
	
	bool _Exists(char* username);
	void _AddNewProfile(utility::LinkedList<Data*>*data);
	void _SwitchProfile(Profile* profile);
	Profile* _GetProfile(char* username);	
	Profile* _GetLastUsedProfile();
	Profile* _ActiveProfile();
};

inline Profile* ProfileController::_ActiveProfile() {
	return this->activeProfile;
}