#include <fstream>
#include "ProfileController.h"
using namespace std;

//	static profile model header
ModelHeader ProfileController::header(ModelName::profile);

//	profile controller constructor - loads profile model header from database
ProfileController::ProfileController() {
	if (this->header._Loaded() == false) {
		_LoadHeader(this->header);
	}
}

//	find last active user profile
Profile* ProfileController::_GetLastUsedProfile() {	
	fstream* stream = _OpenStream();
	if (stream != nullptr) {
		char** buffer = _GetModels(stream, this->header, Query(Range::all));
		for (unsigned int i = 0; i < header._NodeCount(); i++) {
			Profile temporary(buffer[i]);
			delete[]buffer[i];
			if (temporary._Active() == true) {
				this->activeProfile = new Profile(temporary);
				break;
			}
		}
		delete[]buffer;
		stream->close();
	}
	return this->activeProfile;
}

//	check if user profile with the given username already exists
bool ProfileController::_Exists(char* username) {
	fstream* stream = _OpenStream();
	bool result = false;
	if (stream != nullptr) {
		char** buffer = _GetModels(stream, this->header, Query(Range::all));
		for (unsigned int i = 0; i < header._NodeCount(); i++) {
			Profile temporary(buffer[i]);
			delete[]buffer[i];
			if (utility::_CompareChar(temporary._Username(), username)) {
				result = true;				
				break;
			}
		}
		delete[]buffer;
		stream->close();
	}
	delete stream;
	return result;
}

//	loads profile with the given username
Profile* ProfileController::_GetProfile(char* username) {
	fstream* stream = _OpenStream();
	if (stream != nullptr) {
		char** buffer = _GetModels(stream, this->header, Query(Range::all));
		for (unsigned int i = 0; i < header._NodeCount(); i++) {
			Profile temporary(buffer[i]);
			delete[]buffer[i];
			if (utility::_CompareChar(temporary._Username(), username)) {
				Profile* profile = new Profile(temporary);
				return profile;
			}
		}
		delete[]buffer;
		stream->close();
	}
	delete stream;
	return nullptr;
}

//	Add new user profile
void ProfileController::_AddNewProfile(utility::LinkedList<Data*>*data) {
	Profile* newProfile = new Profile(data, header._GiveID());
	fstream* stream = _OpenStream();

	// check if database is created
	if (stream == nullptr) { 
		this->_CreateDatabase();
		// add profile model header
		stream = _OpenStream();
		this->_WriteNewModelHeader(stream, this->header);
	}
	// write model		
	char* buffer = newProfile->_Serialize();
	_WriteModel(stream, this->header, buffer);	

	// deactivate current profile
	if (this->activeProfile != nullptr) {
		activeProfile->_Deactivate();
		char*buffer = activeProfile->_Serialize();
		_UpdateModel(stream, header, activeProfile->_ID(), buffer);		
	}

	delete this->activeProfile;
	activeProfile = newProfile;
	
	stream->close();
	delete stream;
}

//	Change current user profile
void ProfileController::_SwitchProfile(Profile* profile) {
	fstream* stream = _OpenStream();
	activeProfile->_Deactivate();
	char*buffer = activeProfile->_Serialize();
	_UpdateModel(stream, header, activeProfile->_ID(), buffer);

	this->activeProfile = profile;
	activeProfile->_Activate();
	buffer = activeProfile->_Serialize();
	_UpdateModel(stream, header, activeProfile->_ID(), buffer);

	stream->close();
	delete stream;
}