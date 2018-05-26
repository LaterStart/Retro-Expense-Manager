#include <fstream>
#include "ProfileController.h"
using namespace std;

ProfileController::ProfileController() : profile(nullptr){}

ProfileController::~ProfileController(){
	delete profile;
}

void ProfileController::_GetLastUsedProfile(const char* filePath) {
	_SetFilePath(filePath);
	ifstream* file = _OpenFile();
	if (file != nullptr) {
		profile = new Profile;


		
	}
	else 
		status = false;
}