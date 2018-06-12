#pragma once
#include "Controller.h"
#include "../Models/Profile.h"

class ProfileController : public Controller {
public:
	ProfileController();
	~ProfileController();

	Profile* _GetLastUsedProfile();
	bool _CreateNewProfile(utility::LinkedList<UserInput*>*data);
};