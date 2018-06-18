#include <fstream>
#include "ProfileController.h"
using namespace std;

ProfileController::ProfileController() {
	this->model = ModelName::profile;
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
	fstream* stream = _OpenStream();
	bool control = true;
	if (stream != nullptr) { //	check if database is created
		//	check if user profile with the given username already exists
		//this->_GetModels(stream, this->model);








	}
	else { //Create database file	
		this->_CreateDatabase();
		// add profile model header
		//ModelHeader profiles(ModelName::profile);
		//this->_AddNewModelHeader(stream, profiles);
	}

	if (control) { // write model
		/*Profile profile(data);
		char* buffer = profile._Serialize();
		_WriteModel(stream, ModelName::profile, buffer);
		streamoff pos = stream->tellp();		*/	
	}

	stream->close();
	delete stream;
	return control;
}