#include <fstream>
#include "ProfileController.h"
#include "../IO/Input.h"
using namespace std;

ProfileController::ProfileController() {
	this->model = Model::profile;
}

ProfileController::~ProfileController(){}

Profile* ProfileController::_GetLastUsedProfile() {	
	ifstream* stream = _Read();
	if (stream != nullptr) {
		Profile profile;
		return new Profile;
	}
	else
		return nullptr;
}

bool ProfileController::_CreateNewProfile(utility::LinkedList<UserInput*>*data) {
	ifstream* stream = _Read();
	if (stream != nullptr) {
		this->_GetModels(stream, this->model);








	}
	else {
		ofstream* stream = _Write();
		this->_WriteHeader(stream);

		ModelHeader profiles(Model::profile);
		this->_AddNewModelHeader(stream, profiles);
		stream->close();
		delete stream;

		
			ifstream* sstream = _Read();
			this->_GetModels(sstream, this->model);
		



	}
	return true;
}