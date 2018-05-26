#pragma once
#include "Controller.h"
#include "../Models/Profile.h"

class ProfileController : public Controller {
public:
	Profile * profile;

	ProfileController();
	~ProfileController();

	void _GetLastUsedProfile(const char*);
};