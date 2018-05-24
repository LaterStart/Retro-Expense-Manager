#pragma once
#include "Controller.h"
#include "../Models/Profile.h"


class ProfileController : public Controller {
private: 
	bool status;
public:
	Profile * profile;

	ProfileController();
	~ProfileController();

	void _GetLastUsedProfile(const char*);
	bool _GetStatus() const;
};

inline bool ProfileController::_GetStatus() const {
	return status;
}

