#include <fstream>
#include "Controller.h"
using namespace std;

void Controller::_SetFilePath(const char* filePath) {
	this->filePath = filePath;
}

ifstream* Controller::_OpenFile() {
	ifstream* file = new ifstream;
	file->open(filePath, ios::binary);
	if (file->is_open())
		return file;
	delete file;
	return nullptr;
}
