#include <fstream>
#include "ProfileController.h"
#include "../Models/Header.h"
#include "../Models/Profile.h"
using namespace std;

ModelHeader ProfileController::header(ModelName::profile);

ProfileController::ProfileController() {
	this->model = ModelName::profile;
	if (this->header._Loaded() == false) {
		_LoadHeader(this->header);
	}
}

ProfileController::~ProfileController(){}

Profile* ProfileController::_GetLastUsedProfile() {	
	//fstream* stream = _OpenStream();
	//if (stream != nullptr) {
		/*Profile profile;
		return new Profile;*/
		return nullptr;
	//}
	//else
		//return nullptr;
}

//	Add new user profile
bool ProfileController::_WriteNewProfile(utility::LinkedList<Data*>*data) {		
	Profile profile(data);
	fstream* stream = _OpenStream();
	bool control = true;
	if (stream != nullptr) { // check if database is created
		// and if user profile with the given username already exists		
		char** buffer = _GetModels(stream, this->header, Query(Range::all));
		for (int i = 0; i < header._NodeCount(); i++) {
			Profile temporary(buffer[i]);
			if (utility::_CompareChar(temporary._Username(), profile._Username()))
				control = false;
			delete[]buffer[i];
		}
		delete[]buffer;		
	}
	else { // Create database file	
		this->_CreateDatabase();
		// add profile model header
		stream = _OpenStream();
		this->_WriteNewModelHeader(stream, this->header);
	}

	if (control) { // write model		
		char* buffer = profile._Serialize();
		_WriteModel(stream, this->header, buffer);		
	}
	
	stream->close();
	delete stream;
	return control;
}