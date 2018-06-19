#pragma once
#include "_Controller.h"

class Profile; class ModelHeader;
class ProfileController : public Controller {
private:
	static ModelHeader header;

public:
	ProfileController();
	~ProfileController();	

	Profile* _GetLastUsedProfile();
	bool _WriteNewProfile(utility::LinkedList<Data*>*data);
};