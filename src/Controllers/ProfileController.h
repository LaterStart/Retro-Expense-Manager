#pragma once
#include "_Controller.h"
#include "../Models/Profile.h"

class ProfileController : public Controller {
public:
	ProfileController();
	~ProfileController();

	Profile* _GetLastUsedProfile();
	bool _WriteNewProfile(utility::LinkedList<Data*>*data);
};